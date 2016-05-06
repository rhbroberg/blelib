#include "gatt/LongHookCharacteristic.h"
#include "vmlog.h"

using namespace gatt;

LongHookCharacteristic::LongHookCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission)
: 	LongBaseCharacteristic(uuid, properties, permission)
{
}

LongHookCharacteristic::LongHookCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission)
: LongBaseCharacteristic(hex, properties, permission)
{
}

void
LongHookCharacteristic::setReadHook(std::function<const long(void)> hook)
{
	_readHook = hook;
}

void
LongHookCharacteristic::setWriteHook(std::function<void(const long)> hook)
{
	_writeHook = hook;
}

const long
LongHookCharacteristic::onRead()
{
	return _readHook ? _readHook() : 0;
}

void
LongHookCharacteristic::onWrite(const long value)
{
	if (_writeHook)
	{
		_writeHook(value);
	}
}
