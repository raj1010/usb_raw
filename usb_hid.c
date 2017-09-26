
#include "usb_hid.h"



void USBReset(void)
{
    SetBTABLE(BTABLE_ADDRESS);
    /* Initialize Endpoint 0 */
    SetEPType(ENDP0, EP_CONTROL);
    SetEPTxStatus(ENDP0, EP_TX_STALL);
    SetEPRxAddr(ENDP0, EP0RX_ADDRESS);
    SetEPTxAddr(ENDP0, EP0TX_ADDRESS);
    Clear_Status_Out(ENDP0);
    SetEPRxCount(ENDP0, 64);
    SetEPRxValid(ENDP0);
    /* Initialize Endpoint 1 */
    SetEPType(ENDP1, EP_INTERRUPT);
    SetEPTxAddr(ENDP1, EP1TX_ADDRESS);
    SetEPTxCount(ENDP1, 4);
    SetEPRxStatus(ENDP1, EP_RX_DIS);
    SetEPTxStatus(ENDP1, EP_TX_VALID);
    /* Set this device to response on default address */
    SetDeviceAddress(0);
    stEnumeration.enControlState = WAIT_SETUP;
    stEnumeration.unCurrentFeature = ((ConfigDescriptor[8] & 0x40)>>6); // self powered or bus powered bit
    stEnumeration.unCurrentFeature |= ((ConfigDescriptor[8] & 0x20)>>5); // remote wakeup on or not
    stEnumeration.unCurrentConfiguration = 0;
}


enum RequestState GetDeviceDescriptor(struct Enumeration *pstEnumeration)
{
	pstEnumeration->unpData = (uint8_t *)DeviceDescriptor;
	pstEnumeration->udTotalSizePresent = SIZ_DEVICE_DESC;
	pstEnumeration->enRequestState = SUPPORTED;
	return SUPPORTED;
}



enum RequestState GetConfigDescriptor(struct Enumeration *pstEnumeration)
{
	pstEnumeration->unpData = (uint8_t *) ConfigDescriptor;
	if (pstEnumeration->wLength == ConfigDescriptor[0])
    {
        pstEnumeration->udTotalSizePresent = ConfigDescriptor[0];
    }
    else if(pstEnumeration->wLength >= SIZ_CONFIG_DESC)
    {
        pstEnumeration->udTotalSizePresent = SIZ_CONFIG_DESC;
    }
    else
    {
    	pstEnumeration->enRequestState = ERROR;
    	return ERROR;	
    }
    pstEnumeration->enRequestState = SUPPORTED;
	return SUPPORTED;
}


enum RequestState GetStringDescriptor(struct Enumeration *pstEnumeration)
{
	switch ( pstEnumeration->wValue&0x00FF )
    {
        case 0:   // language ID
            pstEnumeration->unpData = (uint8_t *)StringLangID;
            pstEnumeration->udTotalSizePresent = SIZ_STRING_LANGID;  
        break;
        case 1:   // Manufacturer
            pstEnumeration->unpData = (uint8_t *)StringVendor;
            pstEnumeration->udTotalSizePresent = SIZ_STRING_VENDOR;
        break;
        case 2:   // Product  
            pstEnumeration->unpData = (uint8_t *)StringVendor;
            pstEnumeration->udTotalSizePresent = SIZ_STRING_PRODUCT;
        break;
        case 3:   // Serial
            pstEnumeration->unpData = (uint8_t *)StringSerial;
            pstEnumeration->udTotalSizePresent = SIZ_STRING_SERIAL;
        break;
        default:
        	pstEnumeration->enRequestState = UNSUPPORTED;
        	return UNSUPPORTED;
        break;
    }	
    pstEnumeration->enRequestState = SUPPORTED;
	return SUPPORTED;
}


void vClassDataSetup(void)
{
    if ((stEnumeration.bRequest == GET_DESCRIPTOR) && ((stEnumeration.bmRequestType & RECIEPIENT_BITS) == INTERFACE_RECIEPIENT))
    {
        if (((stEnumeration.wValue&0xFF00)>>8) == REPORT_DESC_TYPE )
        {
            stEnumeration.unpData = (uint8_t *)Joystick_ReportDescriptor;
            stEnumeration.wLength = stEnumeration.wLength;
            stEnumeration.udTotalSizePresent = JOYSTICK_SIZ_REPORT_DESC;
            stEnumeration.enRequestState=SUPPORTED;      /* code */
            FUSART_Send(USART1 , 'Z');
        }
        else if (((stEnumeration.wValue&0xFF00)>>8) == HID_DESCRIPTOR_TYPE )
        {
            stEnumeration.unpData = (uint8_t *)(&ConfigDescriptor[18]);
            stEnumeration.wLength = stEnumeration.wLength;
            stEnumeration.udTotalSizePresent = 0x09;
            stEnumeration.enRequestState=SUPPORTED;      /* code */
            FUSART_Send(USART1 , 'Y');
        }
    }
}

#define HID_SET_IDLE        (0x0a)     


void vClassNoDataSetup(void)
{
    if(((stEnumeration.bmRequestType & BMREQUESTTYPE_TYPE_BITS) == 0x20) && (stEnumeration.bRequest == HID_SET_IDLE))
    {
        FUSART_Send(USART1 , 'X');
        stEnumeration.enRequestState=SUPPORTED;
    }
}