#ifndef GATTStringCharacteristic_h
#define GATTStringCharacteristic_h

#include "GATTCharacteristic.h"

class GATTStringCharacteristic : public GATTCharacteristic
{
public:
	GATTStringCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission, char *str = NULL);
	GATTStringCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission, char *str = NULL);
	virtual ~GATTStringCharacteristic();

	void setValue(const char *str);
	const char *getValue() const;

	virtual void readRequest(vm_bt_gatt_attribute_value_t *att_value, const VMUINT16 offset);
	virtual void writeRequest(const vm_bt_gatt_attribute_value_t *value);

protected:
	void initialize();
	virtual const char *onRead(); //= 0;
	virtual void onWrite(const char *value, const unsigned length); //= 0;

	bool _own;
	char *_string;
};

#endif //  GATTStringCharacteristic_h
