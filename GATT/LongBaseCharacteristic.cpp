#include "gatt/LongBaseCharacteristic.h"
#include "vmmemory.h"
#include "vmlog.h"
#include "string.h"
#include "vmstdlib.h"

using namespace gatt;

LongBaseCharacteristic::LongBaseCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission)
: Characteristic(uuid, properties, permission)
{

}

LongBaseCharacteristic::LongBaseCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission)
: Characteristic(hex, properties, permission)
{

}

void
LongBaseCharacteristic::readRequest(vm_bt_gatt_attribute_value_t *att_value, const VMUINT16 offset)
{
	long value = onRead();

	vm_log_info("read object %d", value);
	memcpy(&(att_value->data[offset]), &value, sizeof(value));
	att_value->length = sizeof(value);
}

void
LongBaseCharacteristic::writeRequest(const vm_bt_gatt_attribute_value_t *att_value)
{
	long value = 0;
	memcpy(&value, att_value->data, att_value->length < sizeof(value) ? att_value->length : sizeof(value));

	onWrite(value);
}
