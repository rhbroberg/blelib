#ifndef GATTStringBaseCharacteristic_h
#define GATTStringBaseCharacteristic_h

#include "GATTCharacteristic.h"

class GATTStringBaseCharacteristic : public GATTCharacteristic
{
public:
	GATTStringBaseCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission, char *str = NULL);
	GATTStringBaseCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission, char *str = NULL);

	virtual void readRequest(vm_bt_gatt_attribute_value_t *att_value, const VMUINT16 offset);
	virtual void writeRequest(const vm_bt_gatt_attribute_value_t *value);

protected:

	virtual const char *onRead() = 0;
	virtual void onWrite(const char *value, const unsigned length) = 0;

};

#endif //  GATTStringBaseCharacteristic_h
