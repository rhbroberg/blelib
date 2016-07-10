#include "vmlog.h"
#include "vmmemory.h"
#include "vmlog.h"
#include "string.h"
#include "vmstdlib.h"

#include "gatt/StringCharacteristic.h"

using namespace gatt;

StringCharacteristic::StringCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission, char *str, const unsigned short maxLen)
: StringBaseCharacteristic(uuid, properties, permission)
, _string(str)
, _own(false)
, _length(maxLen < VM_BT_GATT_ATTRIBUTE_MAX_VALUE_LENGTH ? maxLen : VM_BT_GATT_ATTRIBUTE_MAX_VALUE_LENGTH)
{
	initialize();
}

StringCharacteristic::StringCharacteristic(const VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission, char *str, const unsigned short maxLen)
: StringBaseCharacteristic(hex, properties, permission)
, _string(str)
, _own(false)
, _length(maxLen < VM_BT_GATT_ATTRIBUTE_MAX_VALUE_LENGTH ? maxLen : VM_BT_GATT_ATTRIBUTE_MAX_VALUE_LENGTH)
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
	unsigned safeLength = ((strlen(str) < _length) ? strlen(str) : _length) - 1;
	memcpy(_string, str, safeLength);
	*(_string + safeLength) = 0;
	// presently silently ignore truncated string
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
		_string = new char[_length + 1];
		_own = true;
		memset(_string, 0, _length + 1);
	}
}

const char *
StringCharacteristic::onRead()
{
	vm_log_info("returning string %s", _string);
	return _string;
}

void
StringCharacteristic::onWrite(const char *value, const unsigned length)
{
	unsigned safeLength = ((length < _length) ? length : _length) - 1;
	memcpy(_string, value, safeLength);
	*(_string + safeLength) = 0;
	vm_log_info("string %s was written of length %d", _string, safeLength);
	// presently silently ignore truncated string
}
