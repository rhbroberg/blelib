#include "GATTStringCharacteristic.h"
#include "vmlog.h"

#define STRING_MAX_SIZE 128

GATTStringCharacteristic::GATTStringCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission, char *str)
: GATTCharacteristic(uuid, properties, permission)
, _string(str)
, _own(false)
{
	initialize();
}

GATTStringCharacteristic::GATTStringCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission, char *str)
: GATTCharacteristic(hex, properties, permission)
, _string(str)
, _own(false)
{
	initialize();
}

GATTStringCharacteristic::~GATTStringCharacteristic()
{
	if (_own)
	{
		delete _string;
	}
}

void
GATTStringCharacteristic::setValue(const char *str)
{
	memcpy(_string, str, strlen(str));
}

const char *
GATTStringCharacteristic::getValue() const
{
	return _string;
}

void
GATTStringCharacteristic::readRequest(vm_bt_gatt_attribute_value_t *att_value, const VMUINT16 offset)
{
	const char *value = onRead();

	vm_log_info("read object %s", value);
	memcpy(&(att_value->data[offset]), value, strlen(value));
	att_value->length = strlen(value);
}

void
GATTStringCharacteristic::writeRequest(const vm_bt_gatt_attribute_value_t *value)
{
	onWrite((const char *)value->data, value->length);
}

void
GATTStringCharacteristic::initialize()
{
	if (! _string)
	{
		_string = new char[STRING_MAX_SIZE];
		_own = true;
	}
}

const char *
GATTStringCharacteristic::onRead() //= 0;
{
	static const char *foo = "abc 123 do ray me";

	vm_log_info("returning string %s", foo);
	return foo;
}

void
GATTStringCharacteristic::onWrite(const char *value, const unsigned length) //= 0;
{
	static char localString[STRING_MAX_SIZE];
	memcpy(localString, value, length);
	localString[length] = 0;

	vm_log_info("string %s was written of length %d", localString, length);
}
