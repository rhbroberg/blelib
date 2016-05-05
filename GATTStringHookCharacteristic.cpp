#include "GATTStringHookCharacteristic.h"
#include "vmlog.h"

GATTStringHookCharacteristic::GATTStringHookCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission)
: 	GATTStringBaseCharacteristic(uuid, properties, permission)
{
}

GATTStringHookCharacteristic::GATTStringHookCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission)
: GATTStringBaseCharacteristic(hex, properties, permission)
{
}

void
GATTStringHookCharacteristic::setReadHook(std::function<const char *(void)> hook)
{
	_readHook = hook;
}

void
GATTStringHookCharacteristic::setWriteHook(std::function<void(const char *, const unsigned)> hook)
{
	_writeHook = hook;
}

const char *
GATTStringHookCharacteristic::onRead()
{
	return _readHook ? _readHook() : 0;
}

void
GATTStringHookCharacteristic::onWrite(const char *value, const unsigned length)
{
	if (_writeHook)
	{
		_writeHook(value, length);
	}
}
