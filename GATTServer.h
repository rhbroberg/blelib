#ifndef GATTServer_h
#define GATTServer_h

#include "vmtype.h"
#include "vmmemory.h"
#include "string.h"
#include "vmstdlib.h"
#include "vmlog.h"

#include "vmbt_gatt.h"
#include "vmbt_cm.h"

#include <unordered_map>
#include <string>

#include "GATTService.h"

const int myReadHook()
{
	static int foo = 13;

	vm_log_info("in the readhook");
	return foo++;
}
GATTCharacteristic *myChar = NULL;
GATTCharacteristic *c2 = NULL;

static VMUINT8 myserviceUUID[] = { 0xFD, 0x36, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x19, 0x2A, 0x01, 0xFE };
GATTService myService(myserviceUUID, true);

class GATTServer: public GATTBase
{
public:
	static GATTServer *_singleton;

	GATTServer(VMUINT8 *hex)
	: _handle(0)
	, _context(NULL)
	{
		memcpy(_hexUUID, hex, sizeof(_hexUUID));
		initializeCharUUID();
		setCallbacks();
		_singleton = this;
	}

	void addService(GATTService *service)
	{
		_services[service->uuid()] = service;
	}

	const bool enable()
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

	static void btcm_callback(VM_BT_CM_EVENT evt, void *param, void *user_data)
	{
		vm_log_info("btcm_callback");

		switch (evt)
		{
        case VM_BT_CM_EVENT_BLE_ACTIVATE:
		{
			vm_log_info("btcm_callback activate");
			((GATTServer *) user_data)->start();
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

	static void register_server_callback(VM_BT_GATT_CONTEXT_HANDLE reg_ctx, VMBOOL status, VMUINT8 *app_uuid)
	{
		// fix: check uuid matches server uuid
		_singleton->_context = reg_ctx;

		if (status == 0)
		{
			vm_log_info("registering rhb service");
			myService.registerMe(reg_ctx);
		}
	}
	static void service_added_callback(VMBOOL status, VM_BT_GATT_CONTEXT_HANDLE reg_ctx,
			vm_bt_gatt_service_info_t *srvc_id, VM_BT_GATT_SERVICE_HANDLE srvc_handle)
	{
		// fix: check reg_ctx == context
		if (status == 0)
		{
			myService.serviceHandle(srvc_handle);

			vm_log_info("rhb service add callback adding characteristics");
			static VMUINT8 myUUID[] =
			{ 0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x19, 0x2A, 0x00, 0xFF };
			myChar = new GATTCharacteristic((VMUINT8 *) &myUUID,
					VM_BT_GATT_CHAR_PROPERTY_READ | VM_BT_GATT_CHAR_PROPERTY_WRITE,
					VM_BT_GATT_PERMISSION_WRITE | VM_BT_GATT_PERMISSION_READ);
			std::function<const int()> myhook = [&] ()
			{	return myReadHook();};
			myChar->setReadHook(myhook);

			myService.addCharacteristic(myChar);

			static VMUINT8 myc2UUID[] =
			{ 0xFC, 0x35, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x19, 0x2A, 0x02, 0xFD };
			c2 = new GATTCharacteristic(myc2UUID,
					VM_BT_GATT_CHAR_PROPERTY_READ | VM_BT_GATT_CHAR_PROPERTY_WRITE,
					VM_BT_GATT_PERMISSION_WRITE | VM_BT_GATT_PERMISSION_READ);

			myService.addCharacteristic(c2);
		}
	}
	static void characteristic_added_callback(VMBOOL status, VM_BT_GATT_CONTEXT_HANDLE reg_ctx,
			vm_bt_gatt_attribute_uuid_t *uuid, VM_BT_GATT_SERVICE_HANDLE srvc_handle,
			VM_BT_GATT_CHARACTERISTIC_HANDLE char_handle)
	{
		// fix: check context matches
		if (status == 0)
		{
			myService.registerCharacteristic(uuid, char_handle);
			myService.start(reg_ctx, srvc_handle);
		}
	}
	static void descriptor_added_callback(VMBOOL status, VM_BT_GATT_CONTEXT_HANDLE reg_ctx,
			vm_bt_gatt_attribute_uuid_t *uuid, VM_BT_GATT_SERVICE_HANDLE srvc_handle,
			VM_BT_GATT_DESCRIPTOR_HANDLE descr_handle)
	{

	}
	static void service_started_callback(VMBOOL status, VM_BT_GATT_CONTEXT_HANDLE context_handle,
			VM_BT_GATT_SERVICE_HANDLE srvc_handle)
	{
		// fix: check context matches, maybe lookup service for good measure
		if (status == 0)
		{
			vm_bt_gatt_server_listen(context_handle, VM_TRUE);
			vm_log_info("listening on service");
		}
	}
	static void listen_callback(VM_BT_GATT_CONTEXT_HANDLE reg_ctx, VMBOOL status)
	{
		vm_log_info("listen_callback");
	}
	static void connection_callback(const vm_bt_gatt_connection_t *conn, VMBOOL connected,
			const vm_bt_gatt_address_t *bd_addr)
	{
		// this may be unnecessary
		// vm_bt_gatt_server_listen(conn->connection_handle, VM_FALSE);
		// can determine service by conn->context_handle
		// maybe invoke callback for listener
		vm_log_info(
				"connection_callback connected [%d] [0x%x, 0x%x] -s", connected, conn->context_handle, conn->connection_handle);
	}
	static void request_read_callback(vm_bt_gatt_connection_t *conn, VMUINT16 trans_id, vm_bt_gatt_address_t *bd_addr,
			VM_BT_GATT_ATTRIBUTE_HANDLE attr_handle, VMUINT16 offset, VMBOOL is_long)
	{
		// find appropriate GATTCharacteristic in service to invoke method
		if (GATTCharacteristic *activeChar = myService.find(attr_handle))
		{
			activeChar->readRequest(conn, trans_id, attr_handle, offset);
		}
	}
	static void request_write_callback(vm_bt_gatt_connection_t *conn, VMUINT16 trans_id, vm_bt_gatt_address_t *bd_addr,
			VM_BT_GATT_ATTRIBUTE_HANDLE attr_handle, vm_bt_gatt_attribute_value_t *value, VMUINT16 offset,
			VMBOOL need_rsp, VMBOOL is_prep)
	{
		if (GATTCharacteristic *activeChar = myService.find(attr_handle))
		{
			int myInt = 0;
			memcpy(&myInt, value->data, value->length < sizeof(myInt) ? value->length : sizeof(myInt));

			activeChar->write(myInt);

		    if (need_rsp)
			{
				vm_bt_gatt_server_send_response(conn, trans_id, 0, attr_handle, value);
			}
		}
	}

protected:
	void setCallbacks()
	{
		_callbacks.register_server = register_server_callback;
		_callbacks.service_added = service_added_callback;
		_callbacks.characteristic_added = characteristic_added_callback;
		_callbacks.descriptor_added = descriptor_added_callback;
		_callbacks.service_started = service_started_callback;
		_callbacks.listen = listen_callback;
		_callbacks.connection = connection_callback;
		_callbacks.request_read = request_read_callback;
		_callbacks.request_write = request_write_callback;

#ifdef NOTYET
		_callbacks.included_service_added = add_included_service_callback;
		_callbacks.service_stopped = service_stopped_callback;
		_callbacks.service_deleted = service_deleted_callback;
		_callbacks.request_exec_write = request_exec_write_callback;
		_callbacks.response_confirmation = response_confirmation_callback;
		_callbacks.read_tx_power = NULL;
#endif
	}

	void start()
	{
		vm_bt_gatt_server_register(_hexUUID, &_callbacks);
		vm_log_debug("starting server %s", uuid());
	}

	// static void callbacks go here
	std::unordered_map<std::string, GATTService *> _services;
	VMINT _handle;
	void *_context;
	vm_bt_gatt_server_callback_t _callbacks;
};

GATTServer *GATTServer::_singleton;

#endif // GATTServer_h
