#ifndef GATTLongCharacteristic_h
#define GATTLongCharacteristic_h

#include "GATTCharacteristic.h"
#include "vmlog.h"

class GATTLongCharacteristic : public GATTCharacteristic
{
public:
	GATTLongCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission, long *storage = NULL)
	 : 	GATTCharacteristic(uuid, properties, permission)
	, _updated(false)
	, _value(storage)
	, _own(false)
	{
		initialize();
	}

	GATTLongCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission, long *storage = NULL)
	 : GATTCharacteristic(hex, properties, permission)
	 , _updated(false)
	 , _value(storage)
	 , _own(false)
	 {
		initialize();
	 }

	virtual ~GATTLongCharacteristic()
	{
		if (_own)
		{
			delete _value;
		}
	}

	void setValue(const long value)
	{
		*_value = value;
	}

	const long getValue() const
	{
		_updated = false;
		return *_value;
	}

	const bool updated() const
	{
		return _updated;
	}

protected:

	void initialize()
	{
		if (! _value)
		{
			_own = true;
			_value = new long;
			*_value = 0;
		}
	}

	virtual const long onRead()
	{
		return getValue();
	}

	virtual void onWrite(long value)
	{
		_updated = true;
		setValue(value);
		vm_log_info("wrote value %d", *_value);
	}

	long *_value;
	bool _own;
	mutable bool _updated;
};

#endif // GATTLongCharacteristic_h
