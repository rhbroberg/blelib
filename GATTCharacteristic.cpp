#include "GATTCharacteristic.h"

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
GATTCharacteristic::registerMe(void *contextHandle, VM_BT_GATT_ATTRIBUTE_HANDLE serviceHandle)
{
	vm_bt_gatt_server_add_characteristic(contextHandle, serviceHandle, &(_attribute.uuid), _properties, _permission);
	_isRegistered = true;
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
