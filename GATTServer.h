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

class GATTServer : public GATTBase
{
public:
	GATTServer(VMUINT8 *hex)
	: _handle(0)
	, _context(NULL)
	{
		memcpy(_hexUUID, hex, sizeof(_hexUUID));
		initializeCharUUID();
		//setCallbacks();
	}

	void addService(GATTService *service)
	{
		_services[service->uuid()] =  service;
	}

	const bool enable()
	{
        if ((_handle = vm_bt_cm_init(btcm_callback, VM_BT_CM_EVENT_BLE_ACTIVATE | VM_BT_CM_EVENT_DEACTIVATE, this)) < 0)
        {
        	return false;
        }

	    VM_BT_CM_POWER_STATUS power = vm_bt_cm_get_power_status();
	    vm_log_info("enabling BLE : power is '%s'", power ? "on" : "off");

	    if (power != VM_BT_CM_POWER_ON)
	    {
            vm_bt_cm_switch_on();
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
	        case VM_BT_CM_EVENT_ACTIVATE:
	        {
	        	((GATTServer *)user_data)->start();
	            break;
	        }
	        case VM_BT_CM_EVENT_DEACTIVATE:
	        {
	            break;
	        }
	        default:
	        	break;
	    }
	}

	static void register_server_callback(VM_BT_GATT_CONTEXT_HANDLE reg_ctx, VMBOOL status, VMUINT8 *app_uuid);
	static void service_added_callback(VMBOOL status, VM_BT_GATT_CONTEXT_HANDLE reg_ctx, vm_bt_gatt_service_info_t *srvc_id, VM_BT_GATT_SERVICE_HANDLE srvc_handle);
	static void characteristic_added_callback(VMBOOL status, VM_BT_GATT_CONTEXT_HANDLE reg_ctx, vm_bt_gatt_attribute_uuid_t *uuid, VM_BT_GATT_SERVICE_HANDLE srvc_handle, VM_BT_GATT_CHARACTERISTIC_HANDLE char_handle);
	static void descriptor_added_callback(VMBOOL status, VM_BT_GATT_CONTEXT_HANDLE reg_ctx, vm_bt_gatt_attribute_uuid_t *uuid, VM_BT_GATT_SERVICE_HANDLE srvc_handle, VM_BT_GATT_DESCRIPTOR_HANDLE descr_handle);
	static void service_started_callback(VMBOOL status, VM_BT_GATT_CONTEXT_HANDLE reg_ctx, VM_BT_GATT_SERVICE_HANDLE srvc_handle);
	static void listen_callback(VM_BT_GATT_CONTEXT_HANDLE reg_ctx, VMBOOL status);
	static void connection_callback(const vm_bt_gatt_connection_t *conn, VMBOOL connected, const vm_bt_gatt_address_t *bd_addr);
	static void request_read_callback(vm_bt_gatt_connection_t *conn, VMUINT16 trans_id, vm_bt_gatt_address_t *bd_addr, VM_BT_GATT_ATTRIBUTE_HANDLE attr_handle, VMUINT16 offset, VMBOOL is_long);
	static void request_write_callback(vm_bt_gatt_connection_t *conn, VMUINT16 trans_id, vm_bt_gatt_address_t *bd_addr, VM_BT_GATT_ATTRIBUTE_HANDLE attr_handle, vm_bt_gatt_attribute_value_t *value, VMUINT16 offset, VMBOOL need_rsp, VMBOOL is_prep);

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

#endif // GATTServer_h
