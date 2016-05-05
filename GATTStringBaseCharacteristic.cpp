#include "GATTStringBaseCharacteristic.h"
#include "vmlog.h"
#include "vmmemory.h"
#include "vmlog.h"
#include "string.h"
#include "vmstdlib.h"

GATTStringBaseCharacteristic::GATTStringBaseCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission, char *str)
: GATTCharacteristic(uuid, properties, permission)
{
}

GATTStringBaseCharacteristic::GATTStringBaseCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission, char *str)
: GATTCharacteristic(hex, properties, permission)
{
}

void
GATTStringBaseCharacteristic::readRequest(vm_bt_gatt_attribute_value_t *att_value, const VMUINT16 offset)
{
	const char *value = onRead();

	vm_log_info("read object %s", value);
	memcpy(&(att_value->data[offset]), value, strlen(value));
	att_value->length = strlen(value);
}

void
GATTStringBaseCharacteristic::writeRequest(const vm_bt_gatt_attribute_value_t *value)
{
	onWrite((const char *)value->data, value->length);
}
