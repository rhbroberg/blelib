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

	void setReadHook(std::function<const int(void)> hook);
	void setWriteHook(std::function<const int(void)> hook);
	void readRequest(vm_bt_gatt_attribute_value_t *att_value, const VMUINT16 offset);

	void registerMe(void *contextHandle, VM_BT_GATT_ATTRIBUTE_HANDLE serviceHandle);

	void onWrite(int value);
	void writeRequest(const vm_bt_gatt_attribute_value_t *value);

	const vm_bt_gatt_attribute_uuid_t *getAttribute() const;

	void registered(VM_BT_GATT_ATTRIBUTE_HANDLE handle);

protected:
	void initializeAttribute();

	// would be pure virtual
	const int read();

	vm_bt_gatt_attribute_uuid_t _attribute;
	VM_BT_GATT_CHAR_PROPERTIES _properties;
	VM_BT_GATT_PERMISSION _permission;
	VM_BT_GATT_ATTRIBUTE_HANDLE _charHandle;
	std::function<const int(void)> _readHook, _writeHook;
	bool _isRegistered;

	// do these need to be cached?
	void *_context;
	VM_BT_GATT_ATTRIBUTE_HANDLE _serviceHandle;
};


#endif //  GATTCharacteristic_h
