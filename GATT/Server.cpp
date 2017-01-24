#include "vmlog.h"
#include "vmmemory.h"
#include "vmlog.h"
#include "string.h"
#include "vmstdlib.h"

#include "gatt/Server.h"

using namespace gatt;

Server *Server::_singleton;

Server::Server(const VMUINT8 *hex, const char *name)
: UUIDBase(hex)
, _handle(0)
, _context(NULL)
{
	setCallbacks();
	_singleton = this;

	if (name)
	{
		changeName(name);
	}
}

void
Server::changeName(const char *name)
{
	// empty string crashes ble service, forces part to be reflashed
	// while we're at it, force part to have a name at least 2 bytes long
	if (! name || (strlen(name) < 2))
	{
		vm_log_info("anemic ble server name defined; defaulting to safe value");
		name = "mytracker";
	}

	vm_bt_cm_set_host_name((VMUINT8 *)name);
}

void
Server::addService(Service *service)
{
	_services[service->uuid()] = service;
}

void
Server::bindConnectionListener(std::function<void()> connect, std::function<void()> disconnect)
{
	_connect = connect;
	_disconnect = disconnect;
}

// make disable take a completion handler closure for what to do once completely disabled; in the case of firmware upgrade, it will be to invoke the magic line
void
Server::disable(std::function<void()> completionHandler)
{
	vm_log_info("stopping server %s", uuid());
	_disableCallback = completionHandler;

	// must stop each mapped Service, otherwise upon next powerup they won't re-register
	for (const auto & each : _services)
	{
		each.second->stop(true);
	}
}

void
Server::poweroff()
{
	vm_log_info("bluetooth power status:%d", vm_bt_cm_get_power_status());
	vm_bt_cm_switch_off();
	vm_log_info("bluetooth power status after switching off :%d", vm_bt_cm_get_power_status());

	vm_bt_cm_exit(_handle);
	if (_disableCallback)
	{
		_disableCallback();
	}
}

const bool
Server::enable()
{
	if ((_handle = vm_bt_cm_init(btcm_callback, VM_BT_CM_EVENT_BLE_ACTIVATE | VM_BT_CM_EVENT_DEACTIVATE, this)) < 0)
	{
		return false;
	}

	VM_BT_CM_POWER_STATUS power = vm_bt_cm_get_power_status();
	vm_log_info("enabling BLE : power is '%s'", power ? "off" : "on");

	if (power != VM_BT_CM_POWER_ON)
	{
		VM_RESULT result = vm_bt_cm_switch_on();

		vm_log_info("result of poweron is %d", result);
		// callback will invoke start()
	}
	else
	{
		start();
	}

	return true;
}

void
Server::registerServices()
{
	for (const auto & each : _services)
	{
		each.second->registerMe(_context);
	}
}

Service *
Server::findService(const VMUINT8 *hex) const
{
	char tmpKey[32];

	stringify(hex, tmpKey);
	vm_log_info("searching service named %s", tmpKey);
	auto search = _services.find(tmpKey);
	if (search != _services.end())
	{
		return search->second;
	}
	return NULL;
}

Service *
Server::findService(const VM_BT_GATT_ATTRIBUTE_HANDLE key) const
{
	auto search = _byHandle.find(key);
	if (search != _byHandle.end())
	{
		return search->second;
	}
	return NULL;
}

Characteristic *
Server::findCharacteristic(const VM_BT_GATT_ATTRIBUTE_HANDLE key) const
{
	Characteristic *activeChar = NULL;

	// iterate over services, search in each one
	for (const auto & each : _byHandle)
	{
		if (activeChar = each.second->findCharacteristic(key))
		{
			break;
		}
	}
	return activeChar;
}

const bool
Server::contextValid(const VM_BT_GATT_CONTEXT_HANDLE context) const
{
	return context == _context;
}

void
Server::clientActivity(const bool connected)
{
	if (connected)
	{
		if (_connect)
		{
			_connect();
		}
	}
	else
	{
		if (_disconnect)
		{
			_disconnect();
		}
	}
}

const bool
Server::areAllServicesStarted()
{
	vm_log_info("checking if all services are registered");

	for (const auto & each : _services)
	{
		if (!each.second->isStarted())
		{
			return false;
		}
	}
	return true;
}

const bool
Server::areAllServicesStopped()
{
	vm_log_info("checking if all services are stopped/deleted");

	for (const auto & each : _services)
	{
		if (each.second->isStarted())
		{
			return false;
		}
	}
	return true;
}

// static void callbacks go here
// static
void
Server::btcm_callback(VM_BT_CM_EVENT evt, void *param, void *user_data)
{
	vm_log_info("btcm_callback");

	switch (evt)
	{
	case VM_BT_CM_EVENT_BLE_ACTIVATE:
	{
		vm_log_info("btcm_callback activate");
		((Server *) user_data)->start();
		break;
	}
	case VM_BT_CM_EVENT_DEACTIVATE:
	{
		vm_log_info("btcm_callback deactivate");
		break;
	}
	case VM_BT_CM_EVENT_ACTIVATE:
	default:
		vm_log_info("btcm_callback something else %d", evt);
		break;
	}
}

// static
void
Server::register_server_callback(VM_BT_GATT_CONTEXT_HANDLE context_handle, VMBOOL status, VMUINT8 *app_uuid)
{
	// is it necessary to check if uuid matches server uuid?
	_singleton->_context = context_handle;

	if (status == 0)
	{
#ifdef INEFFECTIVE
		vm_log_debug("disabling listener");
		vm_bt_gatt_server_listen(context_handle, VM_TRUE);
#endif

		vm_log_info("registering services");
		_singleton->registerServices();
	}
}

// static
void
Server::service_added_callback(VMBOOL status, VM_BT_GATT_CONTEXT_HANDLE context_handle,
		vm_bt_gatt_service_info_t *srvc_id, VM_BT_GATT_SERVICE_HANDLE srvc_handle)
{
	if (_singleton->contextValid(context_handle) && status == 0)
	{
		vm_log_info("service add callback adding characteristics");

		if (Service *activeService = _singleton->findService(srvc_id->uuid.uuid.uuid))
		{
			activeService->registered(srvc_handle);
			_singleton->_byHandle[srvc_handle] = activeService;
		}
	}
}

// static
void
Server::service_stopped_callback(VMBOOL status, VM_BT_GATT_CONTEXT_HANDLE context_handle, VM_BT_GATT_SERVICE_HANDLE service_handle)
{
	if (_singleton->contextValid(context_handle) && status == 0)
	{
		vm_log_info("service stopped callback for %x", service_handle);

		vm_bt_gatt_server_delete_service(context_handle, service_handle);
	}
}

// static
void
Server::service_deleted_callback(VMBOOL status, VM_BT_GATT_CONTEXT_HANDLE context_handle , VM_BT_GATT_SERVICE_HANDLE service)
{
	vm_log_info("service deleted callback for %x", service);
	if (_singleton->contextValid(context_handle) && status == 0)
	{
		if (Service *activeService = _singleton->findService(service))
		{
			activeService->didStop();
#ifdef NOT_WANTED
// actually do not want to erase entries, or it won't start up again after sleep; only want to set some new attribute that they are removed
			vm_log_info("found activeService %s", activeService->uuid());
			if (_singleton->_byHandle.erase(service) == 1)
			{
				vm_log_info("and erase it byHandle");
			}
			if (_singleton->_services.erase(activeService->uuid()) == 1)
			{
				vm_log_info("and erase it services");
			}

			delete activeService;

			if (_singleton->_byHandle.size() == 0)
			{
				vm_log_info("and now his watch is ended");
				poweroff();
			}
#endif
			if (_singleton->areAllServicesStopped())
			{
				vm_log_info("and now his watch is ended");
				_singleton->poweroff();
			}
		}
	}
}

// static
void
Server::characteristic_added_callback(VMBOOL status, VM_BT_GATT_CONTEXT_HANDLE context_handle,
		vm_bt_gatt_attribute_uuid_t *uuid, VM_BT_GATT_SERVICE_HANDLE srvc_handle,
		VM_BT_GATT_CHARACTERISTIC_HANDLE char_handle)
{
	if (_singleton->contextValid(context_handle) && status == 0)
	{
		if (Service *activeService = _singleton->findService(srvc_handle))
		{
			activeService->registerCharacteristic(uuid, char_handle);
			activeService->start(context_handle, srvc_handle);
		}
	}
}

// static
void
Server::descriptor_added_callback(VMBOOL status, VM_BT_GATT_CONTEXT_HANDLE context_handle,
		vm_bt_gatt_attribute_uuid_t *uuid, VM_BT_GATT_SERVICE_HANDLE srvc_handle,
		VM_BT_GATT_DESCRIPTOR_HANDLE descr_handle)
{

}

// static
void
Server::service_started_callback(VMBOOL status, VM_BT_GATT_CONTEXT_HANDLE context_handle,
		VM_BT_GATT_SERVICE_HANDLE srvc_handle)
{
	// this should probably not happen until after all services and characteristics have fully registered themselves, else a client can get an incomplete
	// view of what's available.  'starting service %x' is when each is ready
	// too bad - it seems like this callback is never invoked
	if (_singleton->contextValid(context_handle) && status == 0)
	{
		vm_bt_gatt_server_listen(context_handle, VM_TRUE);
		vm_log_info("service started");
	}
}

// static
void
Server::listen_callback(VM_BT_GATT_CONTEXT_HANDLE context_handle, VMBOOL status)
{
	vm_log_info("gatt server now listening");
}

// static
void
Server::connection_callback(const vm_bt_gatt_connection_t *conn, VMBOOL connected,
		const vm_bt_gatt_address_t *bd_addr)
{
	vm_log_info("connection_callback connected [%d] [0x%x, 0x%x]", connected, conn->context_handle, conn->connection_handle);

	_singleton->clientActivity(connected);
}

// static
void
Server::request_read_callback(vm_bt_gatt_connection_t *conn, VMUINT16 trans_id, vm_bt_gatt_address_t *bd_addr,
		VM_BT_GATT_ATTRIBUTE_HANDLE attr_handle, VMUINT16 offset, VMBOOL is_long)
{
	// find appropriate Characteristic in service to invoke method
	if (Characteristic *activeChar = _singleton->findCharacteristic(attr_handle))
	{
		vm_bt_gatt_attribute_value_t att_value;

		activeChar->readRequest(&att_value, offset);
		vm_bt_gatt_server_send_response(conn, trans_id, 0, attr_handle, &att_value);
	}
}

// static
void
Server::request_write_callback(vm_bt_gatt_connection_t *conn, VMUINT16 trans_id, vm_bt_gatt_address_t *bd_addr,
		VM_BT_GATT_ATTRIBUTE_HANDLE attr_handle, vm_bt_gatt_attribute_value_t *value, VMUINT16 offset,
		VMBOOL need_rsp, VMBOOL is_prep)
{
	if (Characteristic *activeChar = _singleton->findCharacteristic(attr_handle))
	{
		activeChar->writeRequest(value);

		if (need_rsp)
		{
			vm_bt_gatt_server_send_response(conn, trans_id, 0, attr_handle, value);
		}
	}
}

void
Server::setCallbacks()
{
	_callbacks.register_server = register_server_callback;
	_callbacks.service_added = service_added_callback;
	_callbacks.service_stopped = service_stopped_callback;
	_callbacks.characteristic_added = characteristic_added_callback;
	_callbacks.descriptor_added = descriptor_added_callback;
	_callbacks.service_started = service_started_callback;
	_callbacks.listen = listen_callback;
	_callbacks.connection = connection_callback;
	_callbacks.request_read = request_read_callback;
	_callbacks.request_write = request_write_callback;
	_callbacks.service_deleted = service_deleted_callback;

#ifdef NOTYET
	_callbacks.included_service_added = add_included_service_callback;
	_callbacks.request_exec_write = request_exec_write_callback;
	_callbacks.response_confirmation = response_confirmation_callback;
	_callbacks.read_tx_power = NULL;
#endif
}

void
Server::start()
{
	vm_bt_gatt_server_register(_hexUUID, &_callbacks);
#ifdef BROKEN_INTERACTS_WITH_SERVER_REGISTER
	_client_callbacks.register_client = register_client_callback;
	_client_callbacks.set_advertisement_data = set_advertisement_data_callback;

	vm_bt_gatt_client_register(_singleton->_hexUUID, & _client_callbacks);
#endif
	vm_log_debug("starting server %s", uuid());
}

#ifdef BUSTED
const VMUINT8 copy_of_gsm_service[] = { 0xF0, 0x40, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x19, 0x2A, 0x09, 0x00 };

/** Callback triggered in response to set_advertisement_data. */
// static
void
Server::set_advertisement_data_callback(VM_BT_GATT_CONTEXT_HANDLE context, VMBOOL has_error)
{
	vm_log_info("set_advertisement_data cb has error %d", has_error);
}

static vm_bt_gatt_client_callback_t _client_callbacks;
// static
void
Server::register_client_callback(VM_BT_GATT_CONTEXT_HANDLE context, VMBOOL has_error, VMUINT8 app_uuid[16])
{
	vm_log_debug("client callback sez registered");

		static VMCHAR manufacturer[6];
		memcpy(manufacturer, "rikker", 6);
		static VMCHAR service_data[6];
		memcpy(service_data, "foobar", 6);

		static vm_bt_uuid_with_length_t advertisedUuids;
		advertisedUuids.length = 2;
		memcpy(advertisedUuids.uuid, copy_of_gsm_service, 16);

		VM_RESULT advertising = vm_bt_gatt_server_set_advertisement_data(context, 0, VM_TRUE, VM_TRUE,
				1000, 5000, 1111,
		        6, manufacturer,
		        6, service_data,
		        &advertisedUuids);
		vm_log_info("set advertising status: %d", advertising);
}

#endif
