#include "gatt/LongCharacteristic.h"
#include "vmlog.h"

using namespace gatt;

LongCharacteristic::LongCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission, long *storage)
: 	LongBaseCharacteristic(uuid, properties, permission)
, _updated(false)
, _value(storage)
, _own(false)
{
	initialize();
}

LongCharacteristic::LongCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission, long *storage)
: LongBaseCharacteristic(hex, properties, permission)
, _updated(false)
, _value(storage)
, _own(false)
{
	initialize();
}

LongCharacteristic::~LongCharacteristic()
{
	if (_own)
	{
		delete _value;
	}
}

void
LongCharacteristic::setValue(const long value)
{
	*_value = value;
}

const long
LongCharacteristic::getValue() const
{
	_updated = false;
	return *_value;
}

const bool
LongCharacteristic::updated() const
{
	return _updated;
}

void
LongCharacteristic::initialize()
{
	if (! _value)
	{
		_own = true;
		_value = new long;
		*_value = 0;
	}
}

const long
LongCharacteristic::onRead()
{
	return *_value;
}

void
LongCharacteristic::onWrite(const long value)
{
	_updated = true;
	setValue(value);
	vm_log_info("wrote value %d", *_value);
}
