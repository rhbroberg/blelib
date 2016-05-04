#ifndef GATTLongBaseCharacteristic_h
#define GATTLongBaseCharacteristic_h

#include "GATTCharacteristic.h"

class GATTLongBaseCharacteristic : public GATTCharacteristic
{
public:
	GATTLongBaseCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission)
	: GATTCharacteristic(uuid, properties, permission)
	{

	}
	GATTLongBaseCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission)
	: GATTCharacteristic(hex, properties, permission)
	{

	}

	virtual void
	readRequest(vm_bt_gatt_attribute_value_t *att_value, const VMUINT16 offset)
	{
		long value = onRead();

		vm_log_info("read object %d", value);
		memcpy(&(att_value->data[offset]), &value, sizeof(value));
		att_value->length = sizeof(value);
	}

	virtual void
	writeRequest(const vm_bt_gatt_attribute_value_t *att_value)
	{
		long value = 0;
		memcpy(&value, att_value->data, att_value->length < sizeof(value) ? att_value->length : sizeof(value));

		onWrite(value);
	}

protected:
	void initialize();
	virtual const long onRead() = 0;
	virtual void onWrite(long value) = 0;
};

#endif //  GATTLongBaseCharacteristic_h
