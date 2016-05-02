#ifndef GATTService_h
#define  GATTService_h

#include "vmtype.h"
#include "vmmemory.h"
#include "string.h"
#include "vmstdlib.h"
#include "vmlog.h"

#include "vmbt_gatt.h"
#include "vmbt_cm.h"

#include <unordered_map>
#include <string>

#include "GATTCharacteristic.h"

class GATTService : public GATTBase
{
public:

	GATTService(const char *uuid, const bool primary = false)
	: _serviceHandle(0)
	, _primary(primary)
	, _started(false)
	{
		memcpy(&_uuid, uuid, 16);
		initializeHexUUID();
		initializeInfo();
	}

	GATTService(VMUINT8 *hex, const bool primary = false)
	: _serviceHandle(0)
	, _primary(primary)
	, _started(false)
	{
		memcpy(_hexUUID, hex, sizeof(_hexUUID));
		initializeCharUUID();
		initializeInfo();
	}

	void serviceHandle(VM_BT_GATT_ATTRIBUTE_HANDLE handle)
	{
		_serviceHandle = handle;
	}

	void addCharacteristic(GATTCharacteristic *gattChar)
	{
		vm_log_info("adding char of %s", gattChar->uuid());
		_byUUID[gattChar->uuid()] = gattChar;
		gattChar->registerMe(_context, _serviceHandle);
	}

	GATTCharacteristic *findCharacteristic(	vm_bt_gatt_attribute_uuid_t *key)
	{
		char tmpKey[32];

		stringify(key->uuid.uuid, tmpKey);
		vm_log_info("searching char of %s", tmpKey);
		auto search = _byUUID.find(tmpKey);
		if (search != _byUUID.end())
		{
			return search->second;
		}
		return NULL;
	}

	GATTCharacteristic *find(VM_BT_GATT_ATTRIBUTE_HANDLE key)
	{
		auto search = _byAttributeHandle.find(key);
		if (search != _byAttributeHandle.end())
		{
			return search->second;
		}
		return NULL;
	}

	void registerCharacteristic( vm_bt_gatt_attribute_uuid_t *key, VM_BT_GATT_ATTRIBUTE_HANDLE handle)
	{
    	GATTCharacteristic *tmp = findCharacteristic(key);

    	if (tmp)
    	{
    		tmp->registered(handle);
    		_byAttributeHandle[handle] = tmp;
    	}
	}

	void registerMe(void *context)
	{
		_context = context;
        vm_log_info("adding service");
	    vm_bt_gatt_server_add_service(_context, &_serviceInfo, 10);
	}

	void start(void *context_handle, VM_BT_GATT_ATTRIBUTE_HANDLE srvc_handle)
	{
		if (!_started)
		{
			vm_log_info("starting service");
			_started = true;
			vm_bt_gatt_server_start_service(context_handle, srvc_handle);
		}
		else
		{
			vm_log_info("service already running, not starting");
		}
	}

	// callback sets _serviceHandle;
protected:
	void initializeInfo()
	{
	    memset(&_serviceInfo, 0x0, sizeof(_serviceInfo));
        _serviceInfo.is_primary = _primary ? 1 : 0;
        _serviceInfo.uuid.uuid.length = 16;
        memcpy(_serviceInfo.uuid.uuid.uuid, _hexUUID, (sizeof(VMUINT8) * 16));
	}

	vm_bt_gatt_service_info_t _serviceInfo;
	std::unordered_map<std::string, GATTCharacteristic *> _byUUID;
	std::unordered_map<VM_BT_GATT_ATTRIBUTE_HANDLE, GATTCharacteristic *> _byAttributeHandle;
	bool _primary;
	bool _started;

	void *_context;
	VM_BT_GATT_ATTRIBUTE_HANDLE _serviceHandle;
};

#endif //  GATTService_h
