#pragma once

#include "Characteristic.h"

namespace gatt
{

class LongBaseCharacteristic : public Characteristic
{
public:
	LongBaseCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission);
	LongBaseCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission);

	virtual void readRequest(vm_bt_gatt_attribute_value_t *att_value, const VMUINT16 offset);
	virtual void writeRequest(const vm_bt_gatt_attribute_value_t *att_value);

protected:

	virtual const long onRead() = 0;
	virtual void onWrite(const long value) = 0;
};
}
