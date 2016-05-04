#ifndef GATTCharacteristic_h
#define GATTCharacteristic_h

#include "vmtype.h"
#include "vmmemory.h"
#include "string.h"
#include "vmstdlib.h"
#include "vmlog.h"

#include "vmbt_gatt.h"
#include "vmbt_cm.h"

#include <functional>

#include "GATTBase.h"

// probably template on type of object inside
// subclass for one with no callback but holds storage
// subclass for one which holds storage
// subclass for no storage but callback
class GATTCharacteristic : public GATTBase
{
public:
	GATTCharacteristic(const char *uuid, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission);
	GATTCharacteristic(VMUINT8 *hex, VM_BT_GATT_CHAR_PROPERTIES properties, VM_BT_GATT_PERMISSION permission);
	virtual ~GATTCharacteristic();

	void readRequest(vm_bt_gatt_attribute_value_t *att_value, const VMUINT16 offset);
	void writeRequest(const vm_bt_gatt_attribute_value_t *value);

	void registerMe(void *contextHandle, VM_BT_GATT_ATTRIBUTE_HANDLE serviceHandle);
	void registered(VM_BT_GATT_ATTRIBUTE_HANDLE handle);

protected:
	void initializeAttribute();

	virtual const long onRead() = 0;
	virtual void onWrite(long value) = 0;

	vm_bt_gatt_attribute_uuid_t _attribute;
	VM_BT_GATT_CHAR_PROPERTIES _properties;
	VM_BT_GATT_PERMISSION _permission;
	VM_BT_GATT_ATTRIBUTE_HANDLE _charHandle;
	bool _isRegistered;
};


#endif //  GATTCharacteristic_h
