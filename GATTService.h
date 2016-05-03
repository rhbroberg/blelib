#ifndef GATTService_h
#define  GATTService_h

#include "vmtype.h"
#include "vmmemory.h"
#include "string.h"
#include "vmstdlib.h"
#include "vmlog.h"

#include "vmbt_gatt.h"
#include "vmbt_cm.h"

#include <unordered_map>
#include <string>

#include "GATTCharacteristic.h"

class GATTService : public GATTBase
{
public:
	GATTService(const char *uuid, const bool primary = false);
	GATTService(VMUINT8 *hex, const bool primary = false);

	void addCharacteristic(GATTCharacteristic *gattChar);
	void registerCharacteristic(vm_bt_gatt_attribute_uuid_t *key, VM_BT_GATT_ATTRIBUTE_HANDLE handle);
	GATTCharacteristic *findCharacteristic(vm_bt_gatt_attribute_uuid_t *key);
	GATTCharacteristic *find(VM_BT_GATT_ATTRIBUTE_HANDLE key);

	void registered(VM_BT_GATT_ATTRIBUTE_HANDLE handle);
	void registerMe(void *context);
	void start(void *context_handle, VM_BT_GATT_ATTRIBUTE_HANDLE srvc_handle);

protected:
	void initializeInfo();

	vm_bt_gatt_service_info_t _serviceInfo;
	std::unordered_map<std::string, GATTCharacteristic *> _byUUID;
	std::unordered_map<VM_BT_GATT_ATTRIBUTE_HANDLE, GATTCharacteristic *> _byAttributeHandle;
	bool _primary;
	bool _started;

	void *_context;
	VM_BT_GATT_ATTRIBUTE_HANDLE _serviceHandle;
};

#endif //  GATTService_h
