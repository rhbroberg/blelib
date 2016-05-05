#ifndef GATTStringHookCharacteristic_h
#define GATTStringHookCharacteristic_h

#include "GATTStringBaseCharacteristic.h"
#include <functional>

class GATTStringHookCharacteristic : public GATTStringBaseCharacteristic
{
public:
	GATTStringHookCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission);
	GATTStringHookCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission);

	void setReadHook(std::function<const char *(void)> hook);
	void setWriteHook(std::function<void(const char *, const unsigned)> hook);

protected:

	virtual const char *onRead();
	virtual void onWrite(const char *value, const unsigned length);

	std::function<const char *(void)> _readHook;
	std::function<void(const char *, const unsigned)> _writeHook;
};

#endif // GATTStringHookCharacteristic_h
