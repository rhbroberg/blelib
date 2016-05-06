#pragma once

#include "LongBaseCharacteristic.h"
#include <functional>

namespace gatt
{

class LongHookCharacteristic : public LongBaseCharacteristic
{
public:
	LongHookCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission);
	LongHookCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission);

	void setReadHook(std::function<const long(void)> hook);
	void setWriteHook(std::function<void(const long)> hook);

protected:

	virtual const long onRead();
	virtual void onWrite(const long value);

	std::function<const long(void)> _readHook;
	std::function<void(const long)> _writeHook;
};
}
