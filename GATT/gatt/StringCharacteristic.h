#pragma once

#include "gatt/StringBaseCharacteristic.h"

namespace gatt
{

class StringCharacteristic : public StringBaseCharacteristic
{
public:
	StringCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission, char *str = NULL, const unsigned short maxLen = 16);
	StringCharacteristic(const VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission, char *str = NULL, const unsigned short maxLen = 16);
	virtual ~StringCharacteristic();

	void setValue(const char *str);
	const char *getValue() const;

protected:
	void initialize();
	virtual const char *onRead();
	virtual void onWrite(const char *value, const unsigned length);

	bool _own;
	char *_string;
	const short _length;
};
}
