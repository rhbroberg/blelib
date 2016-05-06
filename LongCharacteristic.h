#pragma once

#include "LongBaseCharacteristic.h"

namespace gatt
{

class LongCharacteristic : public LongBaseCharacteristic
{
public:
	LongCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission, long *storage = NULL);
	LongCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission, long *storage = NULL);

	virtual ~LongCharacteristic();

	void setValue(const long value);
	const long getValue() const;
	const bool updated() const;

protected:

	void initialize();
	virtual const long onRead();
	virtual void onWrite(const long value);

	long *_value;
	bool _own;
	mutable bool _updated;
};

}
