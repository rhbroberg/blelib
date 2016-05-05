#ifndef GATTLongBaseCharacteristic_h
#define GATTLongBaseCharacteristic_h

#include "GATTCharacteristic.h"

class GATTLongBaseCharacteristic : public GATTCharacteristic
{
public:
	GATTLongBaseCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission);
	GATTLongBaseCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission);

	virtual void readRequest(vm_bt_gatt_attribute_value_t *att_value, const VMUINT16 offset);
	virtual void writeRequest(const vm_bt_gatt_attribute_value_t *att_value);

protected:

	virtual const long onRead() = 0;
	virtual void onWrite(const long value) = 0;
};

#endif //  GATTLongBaseCharacteristic_h
