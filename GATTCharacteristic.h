#ifndef GATTCharacteristic_h
#define GATTCharacteristic_h

#include "vmtype.h"
#include "vmmemory.h"
#include "string.h"
#include "vmstdlib.h"
#include "vmlog.h"

#include "vmbt_gatt.h"
#include "vmbt_cm.h"

#include <functional>

#include "GATTBase.h"

// probably template on type of object inside
// subclass for one with no callback but holds storage
// subclass for one which holds storage
// subclass for no storage but callback
class GATTCharacteristic : public GATTBase
{
public:
	GATTCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission)
		: _serviceHandle(0)
		, _context(NULL)
		, _properties(properties)
		, _permission(permission)
		, _isRegistered(false)
		, _charHandle(0)
	{
		memcpy(_uuid, uuid, 32);
		initializeHexUUID();
		initializeAttribute();
	}

	GATTCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission)
		: _serviceHandle(0)
		, _context(NULL)
		, _properties(properties)
		, _permission(permission)
		, _isRegistered(false)
		, _charHandle(0)
	{
		memcpy(_hexUUID, hex, sizeof(_hexUUID)); // sizeof(VMUINT8) * 16); //sizeof(_hexUUID));
		initializeCharUUID();
		initializeAttribute();
	}

	void setReadHook(std::function<const int(void)> hook)
	{
		_readHook = hook;
	}

	void setWriteHook(std::function<const int(void)> hook)
	{
		_writeHook = hook;
	}
	// maybe storage is not in class, constructor gets pointer to storage elsewhere
	// so this just updates or reads that storage and then notifies the owner; if so it would have to use mutexes
	// notification would best be done via closure; maybe no storage here at all?
	// maybe there's a onRead() and onWrite(), defaults to nothing, which is overwritten.  but that means each instance requires subclassing
	//

	void readRequest(vm_bt_gatt_connection_t *conn, VMUINT16 trans_id, VM_BT_GATT_ATTRIBUTE_HANDLE attr_handle, VMUINT16 offset)
	{
	    vm_bt_gatt_attribute_value_t att_value;
	    VMINT value = read();

	    vm_log_info("read object %d", value);
	    memcpy(&(att_value.data[offset]), &value, sizeof(value));
	    att_value.length = sizeof(value);

	    vm_bt_gatt_server_send_response(conn, trans_id, 0, attr_handle, &att_value);
	}

	void registerMe(void *contextHandle, VM_BT_GATT_ATTRIBUTE_HANDLE serviceHandle)
	{
		_serviceHandle = serviceHandle;
		_context = contextHandle;

        vm_bt_gatt_server_add_characteristic(_context, _serviceHandle, &(_attribute.uuid), _properties, _permission);
        _isRegistered = true;
	}

	// would be pure virtual
	void write(const int value)
	{
		static int bar = 0;

		bar = value;
		vm_log_info("wrote value %d", bar);
	}

	void changeProperties()
	{

	}

	void changePermission()
	{

	}

	const vm_bt_gatt_attribute_uuid_t *getAttribute() const
	{
		return &_attribute;
	}

	void registered(VM_BT_GATT_ATTRIBUTE_HANDLE handle)
	{
		vm_log_info("characteristic %s is now registered as %d", uuid(), handle);
		_charHandle = handle;
	}

protected:
	void initializeAttribute()
	{
		memset(&_attribute, 0x0, sizeof(_attribute));
		_attribute.uuid.length = 16;
		memcpy((_attribute.uuid.uuid), _hexUUID, (sizeof(VMUINT8) * 16));
	}

	// would be pure virtual
	const int read()
	{
		static int foo = 42;

		if (_readHook)
		{
			vm_log_info("about to invoke readhook");
			foo = _readHook();
			vm_log_info("back from invoke readhook");
		}
		else
		{
			foo += 3;
			vm_log_info("no read hook");
		}
		return foo;
	}

	vm_bt_gatt_attribute_uuid_t _attribute;
	VM_BT_GATT_CHAR_PROPERTIES _properties;
	VM_BT_GATT_PERMISSION _permission;
	VM_BT_GATT_ATTRIBUTE_HANDLE _charHandle;
	std::function<const int(void)> _readHook, _writeHook;
	bool _isRegistered;

	// do these need to be cached?
	void *_context;
	VM_BT_GATT_ATTRIBUTE_HANDLE _serviceHandle;
};


#endif //  GATTCharacteristic_h
