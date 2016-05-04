#include "GATTCharacteristic.h"

// probably template on type of object inside
// subclass for one with no callback but holds storage
// subclass for one which holds storage
// subclass for no storage but callback
GATTCharacteristic::GATTCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission)
 : _properties(properties)
, _permission(permission)
, _isRegistered(false)
, _charHandle(0)
{
	memcpy(_uuid, uuid, 32);
	initializeHexUUID();
	initializeAttribute();
}

GATTCharacteristic::GATTCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission)
 : _properties(properties)
, _permission(permission)
, _isRegistered(false)
, _charHandle(0)
{
	memcpy(_hexUUID, hex, sizeof(_hexUUID));
	initializeCharUUID();
	initializeAttribute();
}

GATTCharacteristic::~GATTCharacteristic()
{

}

void
GATTCharacteristic::readRequest(vm_bt_gatt_attribute_value_t *att_value, const VMUINT16 offset)
{
	long value = onRead();

	vm_log_info("read object %d", value);
	memcpy(&(att_value->data[offset]), &value, sizeof(value));
	att_value->length = sizeof(value);
}

void
GATTCharacteristic::registerMe(void *contextHandle, VM_BT_GATT_ATTRIBUTE_HANDLE serviceHandle)
{
	vm_bt_gatt_server_add_characteristic(contextHandle, serviceHandle, &(_attribute.uuid), _properties, _permission);
	_isRegistered = true;
}

void
GATTCharacteristic::writeRequest(const vm_bt_gatt_attribute_value_t *value)
{
	long myInt = 0;
	memcpy(&myInt, value->data, value->length < sizeof(myInt) ? value->length : sizeof(myInt));

	onWrite(myInt);
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
