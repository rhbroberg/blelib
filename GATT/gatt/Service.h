#pragma once

#include <map>
#include <string>

#include "gatt/Characteristic.h"

namespace gatt
{

class Service : public UUIDBase
{
public:
	Service(const char *uuid, const bool primary = false);
	Service(const VMUINT8 *hex, const bool primary = false);

	void addCharacteristic(Characteristic *gattChar);
	void registerCharacteristic(vm_bt_gatt_attribute_uuid_t *key, VM_BT_GATT_ATTRIBUTE_HANDLE handle);
	Characteristic *findCharacteristic(vm_bt_gatt_attribute_uuid_t *key);
	Characteristic *findCharacteristic(VM_BT_GATT_ATTRIBUTE_HANDLE key);

	void registered(VM_BT_GATT_ATTRIBUTE_HANDLE handle);
	void registerMe(void *context);
	void start(void *context_handle, VM_BT_GATT_ATTRIBUTE_HANDLE srvc_handle);
	void stop(const bool updateRadio = false);
	const bool isStarted();

protected:
	void initializeInfo();

	vm_bt_gatt_service_info_t _serviceInfo;
	std::map<std::string, Characteristic *> _byUUID;
	std::map<VM_BT_GATT_ATTRIBUTE_HANDLE, Characteristic *> _byAttributeHandle;
	bool _primary;
	bool _started;

	void *_context;
	VM_BT_GATT_ATTRIBUTE_HANDLE _serviceHandle;
};

}
