#ifndef GATTLongCharacteristic_h
#define GATTLongCharacteristic_h

#include "GATTLongBaseCharacteristic.h"

class GATTLongCharacteristic : public GATTLongBaseCharacteristic
{
public:
	GATTLongCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission, long *storage = NULL);
	GATTLongCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission, long *storage = NULL);

	virtual ~GATTLongCharacteristic();

	void setValue(const long value);
	const long getValue() const;
	const bool updated() const;

protected:

	void initialize();
	virtual const long onRead();
	virtual void onWrite(long value);

	long *_value;
	bool _own;
	mutable bool _updated;
};

#endif // GATTLongCharacteristic_h
