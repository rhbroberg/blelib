#include "GATTLongHookCharacteristic.h"
#include "vmlog.h"

GATTLongHookCharacteristic::GATTLongHookCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission)
: 	GATTLongBaseCharacteristic(uuid, properties, permission)
{
}

GATTLongHookCharacteristic::GATTLongHookCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission)
: GATTLongBaseCharacteristic(hex, properties, permission)
{
}

void
GATTLongHookCharacteristic::setReadHook(std::function<const long(void)> hook)
{
	_readHook = hook;
}

void
GATTLongHookCharacteristic::setWriteHook(std::function<void(const long)> hook)
{
	_writeHook = hook;
}

const long
GATTLongHookCharacteristic::onRead()
{
	return _readHook ? _readHook() : 0;
}

void
GATTLongHookCharacteristic::onWrite(const long value)
{
	if (_writeHook)
	{
		_writeHook(value);
	}
}
