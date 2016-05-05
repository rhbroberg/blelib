#ifndef GATTStringCharacteristic_h
#define GATTStringCharacteristic_h

#include "GATTStringBaseCharacteristic.h"

class GATTStringCharacteristic : public GATTStringBaseCharacteristic
{
public:
	GATTStringCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission, char *str = NULL);
	GATTStringCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission, char *str = NULL);
	virtual ~GATTStringCharacteristic();

	void setValue(const char *str);
	const char *getValue() const;

protected:
	void initialize();
	virtual const char *onRead();
	virtual void onWrite(const char *value, const unsigned length);

	bool _own;
	char *_string;
};

#endif //  GATTStringCharacteristic_h
