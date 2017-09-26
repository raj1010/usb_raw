#ifndef _USB_HID_H_
#define _USB_HID_H_

#include "usb.h"   /* For Structures and enum's only*/
#include "usb_desc.h"




void USBReset(void);				
enum RequestState GetDeviceDescriptor(struct Enumeration *pstEnumeration);
enum RequestState GetConfigDescriptor(struct Enumeration *pstEnumeration);
enum RequestState GetStringDescriptor(struct Enumeration *pstEnumeration);
void vClassDataSetup(void);
void vClassNoDataSetup(void);
#endif