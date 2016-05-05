#include "vmtype.h" 
#include "vmboard.h"
#include "vmsystem.h"
#include "vmres.h"
#include "vmmemory.h"
#include "vmstdlib.h"
#include "vmlog.h"

#include "bleLib.h"

#include "GATTServer.h"
#include "GATTLongCharacteristic.h"
#include "GATTLongHookCharacteristic.h"
#include "GATTStringCharacteristic.h"

VMUINT8 g_gatt_uuid[] = { 0x19, 0xA0, 0x1F, 0x49, 0xFF, 0xE5, 0x40, 0x56, 0x84, 0x5B, 0x6D, 0xF1, 0xF1, 0xB1, 0x6E, 0x9D };
GATTServer myServer(g_gatt_uuid);

VMUINT8 myserviceUUID[] = { 0xFD, 0x36, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x19, 0x2A, 0x01, 0xFE };
GATTService myService(myserviceUUID, true);

VMUINT8 myOtherServiceUUID[] = { 0xF0, 0x40, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x19, 0x2A, 0x09, 0xFA };
GATTService myOtherService(myOtherServiceUUID, true);

long myValue;

GATTLongHookCharacteristic *myChar = NULL;
GATTLongCharacteristic *c2 = NULL;
GATTLongCharacteristic *s2c1 = NULL;
GATTLongCharacteristic *s2c2 = NULL;
GATTStringCharacteristic *s2c3 = NULL;

const long myReadHook()
{
	static long foo = 13;

	vm_log_info("in the readhook");
	return foo++;
}

void myWriteHook(const long value)
{
	static long bar = 0;

	vm_log_info("in the writehook writing %d", value);
	bar = value;
}

void testme(void)
{
	VMUINT8 myUUID[] = { 0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x19, 0x2A, 0x00, 0xFF };
	std::function<const long()> myReadhook = [&] () { return myReadHook();};
	std::function<void(const long value)> myWritehook = [&] (const long value) { return myWriteHook(value); };

	myChar = new GATTLongHookCharacteristic((VMUINT8 *) &myUUID,
			VM_BT_GATT_CHAR_PROPERTY_READ | VM_BT_GATT_CHAR_PROPERTY_WRITE,
			VM_BT_GATT_PERMISSION_WRITE | VM_BT_GATT_PERMISSION_READ);
	myChar->setReadHook(myReadHook);
	myChar->setWriteHook(myWriteHook);
	myService.addCharacteristic(myChar);

	VMUINT8 myc2UUID[] = { 0xFC, 0x35, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x19, 0x2A, 0x02, 0xFD };
	c2 = new GATTLongCharacteristic(myc2UUID,
			VM_BT_GATT_CHAR_PROPERTY_READ | VM_BT_GATT_CHAR_PROPERTY_WRITE,
			VM_BT_GATT_PERMISSION_WRITE | VM_BT_GATT_PERMISSION_READ, &myValue);
	myValue = 42;
	myService.addCharacteristic(c2);

	VMUINT8 mys2c12UUID[] = { 0xFC, 0x35, 0x9B, 0x5F, 0x90, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x01, 0x19, 0x2A, 0x02, 0xFD };
	s2c1 = new GATTLongCharacteristic(mys2c12UUID,
			VM_BT_GATT_CHAR_PROPERTY_READ | VM_BT_GATT_CHAR_PROPERTY_WRITE,
			VM_BT_GATT_PERMISSION_WRITE | VM_BT_GATT_PERMISSION_READ);
	s2c1->setValue(4);
	myOtherService.addCharacteristic(s2c1);

	VMUINT8 mys2c2UUID[] = { 0xFC, 0x35, 0x9B, 0x5F, 0xA0, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x02, 0x19, 0x2A, 0x02, 0xFD };
	s2c2 = new GATTLongCharacteristic(mys2c2UUID,
			VM_BT_GATT_CHAR_PROPERTY_READ | VM_BT_GATT_CHAR_PROPERTY_WRITE,
			VM_BT_GATT_PERMISSION_WRITE | VM_BT_GATT_PERMISSION_READ);
	s2c2->setValue(5);
	myOtherService.addCharacteristic(s2c2);

	VMUINT8 mys2c3UUID[] = { 0xFB, 0x34, 0x9B, 0x5F, 0xA0, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x02, 0x19, 0x2A, 0x03, 0xFC };
	s2c3 = new GATTStringCharacteristic(mys2c3UUID,
			VM_BT_GATT_CHAR_PROPERTY_READ | VM_BT_GATT_CHAR_PROPERTY_WRITE,
			VM_BT_GATT_PERMISSION_WRITE | VM_BT_GATT_PERMISSION_READ);
	myOtherService.addCharacteristic(s2c3);

	myServer.addService(&myService);
	myServer.addService(&myOtherService);
    myServer.enable();
}

void handle_sysevt(VMINT message, VMINT param) 
{
    switch (message) 
    {
    case VM_EVENT_CREATE:
        testme();

        break;
	case VM_EVENT_PAINT:	
        /* the message of asking for application to repaint screen */
		/* cerate base layer that has same size as the screen */
        break;

    case VM_EVENT_QUIT:
        vm_log_info("Exit and Close AT Port");
        break;
    }
}

extern "C"
{
/* Entry point */
void vm_main(void)
{
    /* register system events handler */
    vm_pmng_register_system_event_callback(handle_sysevt);
    /* Init APP resource */
    vm_res_init(0);
}
}
