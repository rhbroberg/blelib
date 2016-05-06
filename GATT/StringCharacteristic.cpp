#include "vmlog.h"
#include "vmmemory.h"
#include "vmlog.h"
#include "string.h"
#include "vmstdlib.h"

#include "gatt/StringCharacteristic.h"

using namespace gatt;

StringCharacteristic::StringCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission, char *str)
: StringBaseCharacteristic(uuid, properties, permission)
, _string(str)
, _own(false)
{
	initialize();
}

StringCharacteristic::StringCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission, char *str)
: StringBaseCharacteristic(hex, properties, permission)
, _string(str)
, _own(false)
{
	initialize();
}

StringCharacteristic::~StringCharacteristic()
{
	if (_own)
	{
		delete _string;
	}
}

void
StringCharacteristic::setValue(const char *str)
{
	if (strlen(str) < VM_BT_GATT_ATTRIBUTE_MAX_VALUE_LENGTH)
	{
		memcpy(_string, str, strlen(str));
	}
	// fix: status value return if too big?
}

const char *
StringCharacteristic::getValue() const
{
	return _string;
}

void
StringCharacteristic::initialize()
{
	if (! _string)
	{
		_string = new char[VM_BT_GATT_ATTRIBUTE_MAX_VALUE_LENGTH];
		_own = true;
	}
}

const char *
StringCharacteristic::onRead()
{
	static const char *foo = "abc 123 do ray me";

	vm_log_info("returning string %s", foo);
	return foo;
}

void
StringCharacteristic::onWrite(const char *value, const unsigned length) //= 0;
{
	static char localString[VM_BT_GATT_ATTRIBUTE_MAX_VALUE_LENGTH];
	memcpy(localString, value, length);
	localString[length] = 0;

	vm_log_info("string %s was written of length %d", localString, length);
}
