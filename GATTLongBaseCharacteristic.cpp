#include "GATTLongBaseCharacteristic.h"

GATTLongBaseCharacteristic::GATTLongBaseCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission)
: GATTCharacteristic(uuid, properties, permission)
{

}

GATTLongBaseCharacteristic::GATTLongBaseCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission)
: GATTCharacteristic(hex, properties, permission)
{

}

void
GATTLongBaseCharacteristic::readRequest(vm_bt_gatt_attribute_value_t *att_value, const VMUINT16 offset)
{
	long value = onRead();

	vm_log_info("read object %d", value);
	memcpy(&(att_value->data[offset]), &value, sizeof(value));
	att_value->length = sizeof(value);
}

void
GATTLongBaseCharacteristic::writeRequest(const vm_bt_gatt_attribute_value_t *att_value)
{
	long value = 0;
	memcpy(&value, att_value->data, att_value->length < sizeof(value) ? att_value->length : sizeof(value));

	onWrite(value);
}

void
GATTLongBaseCharacteristic::initialize()
{

}
