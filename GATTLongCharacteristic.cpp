#include "GATTLongCharacteristic.h"
#include "vmlog.h"

GATTLongCharacteristic::GATTLongCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission, long *storage)
: 	GATTLongBaseCharacteristic(uuid, properties, permission)
, _updated(false)
, _value(storage)
, _own(false)
{
	initialize();
}

GATTLongCharacteristic::GATTLongCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission, long *storage)
: GATTLongBaseCharacteristic(hex, properties, permission)
, _updated(false)
, _value(storage)
, _own(false)
{
	initialize();
}

GATTLongCharacteristic::~GATTLongCharacteristic()
{
	if (_own)
	{
		delete _value;
	}
}

void
GATTLongCharacteristic::setValue(const long value)
{
	*_value = value;
}

const long
GATTLongCharacteristic::getValue() const
{
	_updated = false;
	return *_value;
}

const bool
GATTLongCharacteristic::updated() const
{
	return _updated;
}

void
GATTLongCharacteristic::initialize()
{
	if (! _value)
	{
		_own = true;
		_value = new long;
		*_value = 0;
	}
}

const long
GATTLongCharacteristic::onRead()
{
	return getValue();
}

void
GATTLongCharacteristic::onWrite(long value)
{
	_updated = true;
	setValue(value);
	vm_log_info("wrote value %d", *_value);
}
