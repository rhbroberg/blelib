#ifndef GATTCharacteristic_h
#define GATTCharacteristic_h

#include "GATTBase.h"

class GATTCharacteristic : public GATTBase
{
public:
	GATTCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission);
	GATTCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission);
	virtual ~GATTCharacteristic();

	void registerMe(void *contextHandle, VM_BT_GATT_ATTRIBUTE_HANDLE serviceHandle);
	void registered(VM_BT_GATT_ATTRIBUTE_HANDLE handle);

	virtual void readRequest(vm_bt_gatt_attribute_value_t *att_value, const VMUINT16 offset) = 0;
	virtual void writeRequest(const vm_bt_gatt_attribute_value_t *value) = 0;

protected:
	void initializeAttribute();

	vm_bt_gatt_attribute_uuid_t _attribute;
	VM_BT_GATT_CHAR_PROPERTIES _properties;
	VM_BT_GATT_PERMISSION _permission;
	VM_BT_GATT_ATTRIBUTE_HANDLE _charHandle;
	bool _isRegistered;
};

#endif //  GATTCharacteristic_h
