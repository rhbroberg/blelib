#ifndef GATTLongHookCharacteristic_h
#define GATTLongHookCharacteristic_h

#include "GATTLongBaseCharacteristic.h"
#include <functional>

class GATTLongHookCharacteristic : public GATTLongBaseCharacteristic
{
public:
	GATTLongHookCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission);
	GATTLongHookCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission);

	void setReadHook(std::function<const long(void)> hook);
	void setWriteHook(std::function<void(const long)> hook);

protected:

	virtual const long onRead();
	virtual void onWrite(long value);

	std::function<const long(void)> _readHook;
	std::function<void(const long)> _writeHook;
};

#endif // GATTLongHookCharacteristic_h
