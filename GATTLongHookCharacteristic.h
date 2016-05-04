#ifndef GATTLongHookCharacteristic_h
#define GATTLongHookCharacteristic_h

#include <functional>
#include "GATTCharacteristic.h"
#include "vmlog.h"

class GATTLongHookCharacteristic : public GATTCharacteristic
{
public:
	GATTLongHookCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission)
	 : 	GATTCharacteristic(uuid, properties, permission)
	{
	}

	GATTLongHookCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission)
	 : GATTCharacteristic(hex, properties, permission)
	 {
	 }

	void setReadHook(std::function<const long(void)> hook)
	{
		_readHook = hook;
	}

	void setWriteHook(std::function<void(const long)> hook)
	{
		_writeHook = hook;
	}

protected:

	virtual const long onRead()
	{
		return _readHook ? _readHook() : 0;
	}

	virtual void onWrite(long value)
	{
		if (_writeHook)
		{
			_writeHook(value);
		}
	}

	std::function<const long(void)> _readHook;
	std::function<void(const long)> _writeHook;
};

#endif // GATTLongHookCharacteristic_h
