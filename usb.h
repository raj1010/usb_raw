#ifndef __USB_H
#define __USB_H

#include "stm32f30x_gpio.h"
#include "usb_regs.h"
#include "usart.h"
#include "stm32f30x_misc.h"

#define USB_INTERRRUPT_MASK (CNTR_CTRM | CNTR_ERRM | CNTR_DOVRM | CNTR_WKUPM | CNTR_SUSPM  /*| CNTR_SOFM | CNTR_ESOFM */| CNTR_RESETM )


#define BTABLE_ADDRESS      (0x00)
#define EP0RX_ADDRESS       (0x18)
#define EP0TX_ADDRESS       (0x58)
#define EP1TX_ADDRESS       (0x100)


/*BmRequestType bitmask macros*/
#define DEVICE_RECIEPIENT       (0x00)
#define INTERFACE_RECIEPIENT    (0x01)
#define ENDPOINT_RECIEPIENT     (0x02)
#define RECIEPIENT_BITS         (0x1F)


#define STANDARD_REQUEST        (0x00)
#define CLASS_REQUEST           (0x20)
#define BMREQUESTTYPE_TYPE_BITS (0x60)  // bit 6 and 5 tell use request type (0 for standard and 1 for class specfic and bla bla) 


//Standard Requests
#define CLEAR_FEATURE            (0x01)
#define GET_CONFIGURATION        (0x08)
#define GET_DESCRIPTOR           (0x06)
#define GET_INTERFACE            (0x0A)
#define GET_STATUS               (0x00)
#define SET_ADDRESS              (0x05)
#define SET_CONFIGURATION        (0x09)
#define SET_DESCRIPTOR           (0x07)
#define SET_FEATURE              (0x03)
#define SET_INTERFACE            (0x0B)
#define SYNC_FRAME               (0x0C)


// genral Macros
#define MAX_PACKET_SIZE          (0x40) //64


enum RequestState
{
    FAULT=0,
    SUPPORTED=1,
    NOTREADY=2,
    UNSUPPORTED=3,
    NOTFOUND=4
}; 

enum ControlState
{
  WAIT_SETUP,       /* 0 */
  SETTING_UP,       /* 1 */
  IN_DATA,          /* 2 */
  OUT_DATA,         /* 3 */
  LAST_IN_DATA,     /* 4 */
  LAST_OUT_DATA,    /* 5 */
  WAIT_STATUS_IN,   /* 7 */
  WAIT_STATUS_OUT,  /* 8 */
  STALLED,          /* 9 */
  PAUSE             /* 10 */
};    /* The state machine states of a control pipe */


enum DeviceState
{
  ATTACHED,     /* 0 */
  POWERED,      /* 1 */
  DEFUALT,      /* 2 */
  ADDRESSED,    /* 3 */
  CONFIGURED,   /* 4 */
  SUSPENDED     /* 5 */
};


#define Send0LengthData() { _SetEPTxCount(ENDP0, 0); \
    _SetEPTxStatus(ENDP0,EP_TX_VALID); \
}
  

struct Request
{
  uint8_t bmRequestType;
  uint8_t bRequest;
  uint16_t wValue;
  uint16_t wIndex;
  uint16_t wLength;
};

extern struct Request stRequest;

struct Enumeration
{
  uint8_t unCurrentFeature; 
  uint8_t unCurrentConfiguration;    /* Selected configuration */
  uint8_t unCurrentInterface;        /* Selected interface of current configuration */
  uint8_t unCurrentAlternateSetting; /* Selected Alternate Setting of current*/

  uint16_t udGetStatus; 

  uint16_t udTotalSizePresent;
  uint16_t udSizeRem;
  uint8_t  *unpData;
  uint8_t  bZPLStage;     // ZPL required or not 
	
  enum DeviceState  enDeviceState;             /* of type CONTROL_STATE */
  enum ControlState enControlState;
  enum RequestState enRequestState;
};

extern struct Enumeration stEnumeration;

//*****************************************//
void UsbGpioInit(void);
void UsbPowerClockInit(void);
void UsbInterruptInit(void);
void USBHardwareInit(void);
void SetDeviceAddress(uint8_t Val);
void USB_LP_CAN1_RX0_IRQHandler(void);
void EndPointZeroHandle(void);
void vPrintRequest(void);
void HandleSetup0NoData(void); // address request etc  // ye sari in ka wait krengi
void HandleSetup0Data(void);
void vIN0Proecess(void);
enum RequestState StandardGetStatus(struct Enumeration *pstEnumeration);
enum RequestState GetConfiguration(struct Enumeration *pstEnumeration);

#endif
