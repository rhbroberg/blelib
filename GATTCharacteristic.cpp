#include "GATTCharacteristic.h"

// probably template on type of object inside
// subclass for one with no callback but holds storage
// subclass for one which holds storage
// subclass for no storage but callback
GATTCharacteristic::GATTCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission)
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

GATTCharacteristic::GATTCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission)
: _serviceHandle(0)
, _context(NULL)
, _properties(properties)
, _permission(permission)
, _isRegistered(false)
, _charHandle(0)
{
	memcpy(_hexUUID, hex, sizeof(_hexUUID));
	initializeCharUUID();
	initializeAttribute();
}

void
GATTCharacteristic::setReadHook(std::function<const int(void)> hook)
{
	_readHook = hook;
}

void
GATTCharacteristic::setWriteHook(std::function<const int(void)> hook)
{
	_writeHook = hook;
}
// maybe storage is not in class, constructor gets pointer to storage elsewhere
// so this just updates or reads that storage and then notifies the owner; if so it would have to use mutexes
// notification would best be done via closure; maybe no storage here at all?
// maybe there's a onRead() and onWrite(), defaults to nothing, which is overwritten.  but that means each instance requires subclassing
//

void
GATTCharacteristic::readRequest(vm_bt_gatt_attribute_value_t *att_value, const VMUINT16 offset)
{
	VMINT value = read();

	vm_log_info("read object %d", value);
	memcpy(&(att_value->data[offset]), &value, sizeof(value));
	att_value->length = sizeof(value);
}

void
GATTCharacteristic::registerMe(void *contextHandle, VM_BT_GATT_ATTRIBUTE_HANDLE serviceHandle)
{
	_serviceHandle = serviceHandle;
	_context = contextHandle;

	vm_bt_gatt_server_add_characteristic(_context, _serviceHandle, &(_attribute.uuid), _properties, _permission);
	_isRegistered = true;
}

void
GATTCharacteristic::onWrite(int value)
{
	static int bar = 0;

	bar = value;
	vm_log_info("wrote value %d", bar);
}

// could be pure virtual
void
GATTCharacteristic::writeRequest(const vm_bt_gatt_attribute_value_t *value)
{
	int myInt = 0;
	memcpy(&myInt, value->data, value->length < sizeof(myInt) ? value->length : sizeof(myInt));

	onWrite(myInt);
}

const vm_bt_gatt_attribute_uuid_t *
GATTCharacteristic::getAttribute() const
{
	return &_attribute;
}

void
GATTCharacteristic::registered(VM_BT_GATT_ATTRIBUTE_HANDLE handle)
{
	vm_log_info("characteristic %s is now registered as %d", uuid(), handle);
	_charHandle = handle;
}

void
GATTCharacteristic::initializeAttribute()
{
	memset(&_attribute, 0x0, sizeof(_attribute));
	_attribute.uuid.length = 16;
	memcpy((_attribute.uuid.uuid), _hexUUID, (sizeof(VMUINT8) * 16));
}

// would be pure virtual
const int
GATTCharacteristic::read()
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
