#include "usb.h"
#include "usb_mem.h"
#include "usb_desc.h"
#include "stdio.h"
#include "stdlib.h"
#include "usb_hid.h"



extern uint8_t Prev;
#define TRUE    1
#define FALSE   0

struct Enumeration stEnumeration; // hold all enumeration related data ok!!
struct Request 	   stRequest;

uint16_t address=0;
#define vSetEPRxStatus(st) (SaveRState = st)
#define vSetEPTxStatus(st) (SaveTState = st)




void UsbGpioInit(void)
{
	RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_Struct;
	GPIO_Struct.GPIO_Pin=GPIO_Pin_11|GPIO_Pin_12;
	GPIO_Struct.GPIO_Mode=GPIO_Mode_AF;
	GPIO_Struct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Struct.GPIO_OType=GPIO_OType_PP;
	GPIO_Struct.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_Struct);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_14);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_14);
}


void UsbPowerClockInit(void)
{


	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
  	/* Enable USB clock */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);

  	_SetCNTR(~CNTR_PDWN); // clear FRES and PWDN
  	_SetISTR(0); // clear pending interrupts
  	_SetCNTR(~CNTR_FRES);
  	
}

void UsbInterruptInit(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel=USB_LP_CAN1_RX0_IRQn;
	//USB_HP_CAN1_TX_IRQn ;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}



void USBHardwareInit(void)
{
	UsbGpioInit();
	UsbInterruptInit();
	UsbPowerClockInit();
	_SetCNTR(0);
	_SetCNTR(USB_INTERRRUPT_MASK);
}


void SetDeviceAddress(uint8_t Val)
{
  uint32_t i;
  uint32_t nEP = 2;

  /* set address in every used endpoint */
  for (i = 0; i < nEP; i++)
  {
    _SetEPAddress((uint8_t)i, (uint8_t)i);
  } /* for */
  _SetDADDR(Val | DADDR_EF); /* set device address and enable function */ 
}

// isme endpoint0 ko set kro end descriptor wagera k path do

 


void USB_LP_CAN1_RX0_IRQHandler(void)
{
	uint16_t wIstr = _GetISTR();
	if (wIstr & ISTR_RESET)
 	{
    	_SetISTR((uint16_t)CLR_RESET);
	    USBReset();
        FUSART_Send(USART1 , '0');
	}
    else if (wIstr & ISTR_CTR)
    {
    	// here we will check endp number and direction to further redirect control    
    	if( (wIstr  & ISTR_EP_ID ) == ENDP0)
    	{
		   EndPointZeroHandle();
    	}
    	else
    	{
       		_ClearEP_CTR_TX(ENDP1);
       		Prev=1;
        	FUSART_Send(USART1 , '1');           
    	}
    }
    else if (wIstr & ISTR_DOVR)
    {
        _SetISTR((uint16_t)CLR_DOVR);
        FUSART_Send(USART1 , '2');
    }
    else if (wIstr & ISTR_ERR)
    {
        _SetISTR((uint16_t)CLR_ERR);
        FUSART_Send(USART1 , '3');
    }
    else if (wIstr & ISTR_WKUP)
    {
        _SetISTR((uint16_t)CLR_WKUP);
        FUSART_Send(USART1 , '4');
    }
    else if (wIstr & ISTR_SUSP)
    {
        _SetISTR((uint16_t)CLR_SUSP);
        FUSART_Send(USART1 , '5');
    }
    else
    {
        FUSART_Send(USART1 , '6');
    }
	
}

uint8_t flag=0;
uint16_t SaveRState,SaveTState, reg;

void StoreSetupRequest(struct Request *stReq)
{
    uint8_t *Pointer = PMAAddr + (uint8_t *)(_GetEPRxAddr(ENDP0) * 2); /* *2 for 32 bits addr */
    stReq->bmRequestType = *Pointer++;
    stReq->bRequest      = *Pointer++; 
    stReq->wValue        = (*((uint16_t *)(Pointer+2))); /* wValue */
    stReq->wIndex        = (*((uint16_t *)(Pointer+6))); /* wIndex */
    stReq->wLength       = (*((uint16_t *)(Pointer+10))); /* wLength */
}

void EndPointZeroHandle(void)
{

    //SaveRState = _GetENDPOINT(ENDP0);
    reg=_GetENDPOINT(ENDP0);
    //SaveTState = SaveRState & EPTX_STAT;
    //SaveRState &=  EPRX_STAT;   
    //_SetEPRxTxStatus(ENDP0,EP_RX_NAK,EP_TX_NAK);
    if (reg & EP_CTR_TX)  // INo process
    {
        _ClearEP_CTR_TX(ENDP0);
        vIN0Proecess();
    }
    else if(reg & EP_SETUP) // If setup bit is high do parsing and data storing
    {

        _ClearEP_CTR_RX(ENDP0);

        stEnumeration.enControlState = SETTING_UP;
        stEnumeration.enRequestState  = NOTFOUND;
        
        StoreSetupRequest(&stRequest);  
        
        FUSART_Send(USART1,'\r');
        FUSART_Send(USART1,'\n');
        vPrintRequest(&stRequest);
        if (stRequest.wLength == 0)
        {
            HandleSetup0NoData(&stRequest);
        }
        else
        {
            HandleSetup0Data(&stRequest);
        }
    }
    else if (reg & EP_CTR_RX)
    {
        _ClearEP_CTR_RX(ENDP0);
        FUSART_Send(USART1 , '7');
    }
    else
    {
       FUSART_Send(USART1 , '$');
       FUSART_Send(USART1 , '1');
    }
}

// genral Macros
#define MAX_PACKET_SIZE          (0x40) //64


void vPrintRequest(struct Request *stReq)
{
    PrintHex(stReq->bmRequestType );
    PrintHex(stReq->bRequest      );
    PrintHex(((stReq->wValue&0xff00)>>8));
    PrintHex(((stReq->wValue&0x00ff)));
    PrintHex(((stReq->wIndex&0xff00)>>8));
    PrintHex(((stReq->wIndex&0x00ff)));
    PrintHex(((stReq->wLength&0xff00)>>8));
    PrintHex(((stReq->wLength&0x00ff)));
    FUSART_Send(USART1 , ' ');
}


void HandleSetup0NoData(struct Request *stReq) // address request etc  // ye sari in ka wait krengi
{
    if ((stReq->bmRequestType & BMREQUESTTYPE_TYPE_BITS) == 0x00) // standard requests 
    {       
        if(stReq->bRequest == SET_ADDRESS)
        {
            stEnumeration.enRequestState=SUPPORTED;
            FUSART_Send(USART1 , 'a');
        }
        else if (stReq->bRequest == CLEAR_FEATURE)
        {
            FUSART_Send(USART1 , 'b');
            stEnumeration.enRequestState=SUPPORTED;
        }
        else if(stReq->bRequest == SET_CONFIGURATION)
        {
            stEnumeration.unCurrentConfiguration = stEnumeration.wValue;
            stEnumeration.enDeviceState  = CONFIGURED;         
            stEnumeration.enRequestState = SUPPORTED;      
            FUSART_Send(USART1 , 'c');
        }
        else if(stReq->bRequest == SET_FEATURE)
        {
            FUSART_Send(USART1 , 'd');
            stEnumeration.enRequestState=SUPPORTED;
        }
        else if(stReq->bRequest == SET_INTERFACE)
        {
            FUSART_Send(USART1 , 'e');
            stEnumeration.enRequestState=SUPPORTED;
        }
    }

    if (stEnumeration.enRequestState == NOTFOUND) // class specific requests hen re baba
    {
        vClassNoDataSetup();
    }

    if (stEnumeration.enRequestState==SUPPORTED)
    {
        FUSART_Send(USART1 , 'f');
        stEnumeration.enControlState = WAIT_STATUS_IN;
        Send0LengthData();// status stage
        _SetEPRxStatus(ENDP0,EP_RX_VALID);
    }
    else if (stEnumeration.enRequestState == UNSUPPORTED || stEnumeration.enRequestState == NOTFOUND)
    {
        FUSART_Send(USART1 , '$');
        FUSART_Send(USART1 , '2');
        _SetEPRxStatus(ENDP0,EP_RX_VALID); // for new setup request
        _SetEPTxStatus(ENDP0,EP_TX_STALL); // to stall current request
    }
}

        
/*
Device specific requests can also come in standard request form

*/
void HandleSetup0Data(struct Request *stReq)
{

    if ((stReq->bmRequestType & BMREQUESTTYPE_TYPE_BITS) == 0x00) // standard requests
    {
        if((stReq->bRequest == GET_CONFIGURATION) && (stEnumeration.enDeviceState >= ADDRESSED) \
            &&(stReq->wValue == 0)&&(stReq->wIndex == 0)&&(stReq->wLength == 1)&&(stReq->bmRequestType == 0x80))
        {
            (void) GetConfiguration(&stEnumeration);
            FUSART_Send(USART1 , 'h');
        }
        else if((stReq->bRequest == GET_DESCRIPTOR))
        {
            stEnumeration.udTotalSizePresent=0;
            switch(((stEnumeration.wValue&0xFF00)>>8))
            {
                case DEVICE_DESC_TYPE:
                    GetDeviceDescriptor(&stEnumeration);        
                    FUSART_Send(USART1 , 'i');    
                break;

                case DEVICE_QUALIFIER_TYPE:
                    stEnumeration.enRequestState=UNSUPPORTED;        
                    FUSART_Send(USART1 , 'T');    
                break;

                case CONFIG_DESC_TYPE:
                    GetConfigDescriptor(&stEnumeration);
                    FUSART_Send(USART1 , 'j');
                break;
  
                case STRING_DESC_TYPE:
                    GetStringDescriptor(&stEnumeration);
                    FUSART_Send(USART1 , 'k');
                break;
    
                default:              // remove it so that it can be forwarded to Class specific requests
                    stEnumeration.enRequestState=NOTFOUND;
                break;
            }
        }
        else if((stReq->bRequest == GET_INTERFACE) && (stEnumeration.enDeviceState == CONFIGURED) \
            &&(stReq->wValue == 0)&&(stReq->wLength == 1)&&(stReq->bmRequestType == 0x81))
        {
            stEnumeration.unpData = (uint8_t *)(&(stEnumeration.unCurrentAlternateSetting));
            stEnumeration.udTotalSizePresent = 1;
            stEnumeration.enRequestState=SUPPORTED;
            FUSART_Send(USART1 , 'l');
        }
        else if(stReq->bRequest == GET_STATUS && stReq->wValue==0 && stReq->wLength == 2)
        {
            (void)StandardGetStatus(&stEnumeration);
            FUSART_Send(USART1 , 'm');    
        }
        else if(stReq->bRequest == SET_DESCRIPTOR)// isme data out aa skta he
        {
            FUSART_Send(USART1 , 'n');
            stEnumeration.enRequestState=UNSUPPORTED; // so to set tx stall to show not supported
        }
        else if(stReq->bRequest == SYNC_FRAME)
        {
            FUSART_Send(USART1 , 'o');
            stEnumeration.enRequestState=UNSUPPORTED;
        }           
    }
    
    if (stEnumeration.enRequestState == NOTFOUND) // class specific requests or abhi tak request nhi mili to bhi isme jaega
    {
        FUSART_Send(USART1 , 'p');
        vClassDataSetup();        
    }

    if (stEnumeration.udTotalSizePresent > 0 || stEnumeration.enRequestState == SUPPORTED) 
    {
        stEnumeration.udSizeRem = stEnumeration.udTotalSizePresent;
        stEnumeration.enControlState = IN_DATA;
        if (stEnumeration.wLength == stEnumeration.udTotalSizePresent) 
        {
            stEnumeration.bZPLStage = FALSE;
        }
        else if(stEnumeration.wLength > stEnumeration.udTotalSizePresent)
        {
            if ((stEnumeration.udTotalSizePresent % MAX_PACKET_SIZE) == 0 )
            {
                stEnumeration.bZPLStage = TRUE;    
            }
            else
            {
                stEnumeration.bZPLStage = FALSE;
            }
        }
        FUSART_Send(USART1 , 'q');
        vIN0Proecess();
    }
    else if (stEnumeration.enRequestState == UNSUPPORTED || stEnumeration.enRequestState == NOTFOUND) // still UNSUPPORTED than give a stall
    {

        FUSART_Send(USART1 , 'r');            
        _SetEPTxStatus(ENDP0,EP_TX_STALL); // to stall next request
        _SetEPRxStatus(ENDP0,EP_RX_VALID); // for next setup packet
    } 

}



void vIN0Proecess(struct Request *stReq)
{
    if ( stEnumeration.enControlState == WAIT_STATUS_IN )
    {
        if(stReq->bRequest == SET_ADDRESS)
        {
            SetDeviceAddress(stEnumeration.wValue);
            stEnumeration.enDeviceState = ADDRESSED;
            stEnumeration.enControlState=WAIT_SETUP;
        }
        else if (stReq->bRequest == CLEAR_FEATURE)
        {
            FUSART_Send(USART1 , 's');
            stEnumeration.enControlState=WAIT_SETUP;
        }
        else if(stReq->bRequest == SET_CONFIGURATION)
        {
            stEnumeration.enControlState=WAIT_SETUP;
        }
        else if(stReq->bRequest == SET_FEATURE)
        {
            FUSART_Send(USART1 , 't');
            stEnumeration.enControlState=WAIT_SETUP;
        }
        else if(stReq->bRequest == SET_INTERFACE)
        {
            FUSART_Send(USART1 , 'u');
            stEnumeration.enControlState=WAIT_SETUP;
        }
        _SetEPRxStatus(ENDP0, EP_RX_VALID);
    }
    else if( stEnumeration.bZPLStage == TRUE && stEnumeration.udSizeRem == 0 )
    {
        Send0LengthData();
        stEnumeration.enControlState = WAIT_STATUS_OUT;
        FUSART_Send(USART1 , 'v');   
    }
    else if (stEnumeration.udSizeRem == 0 && stEnumeration.enControlState == WAIT_STATUS_OUT)
    {
        _SetEPRxStatus(ENDP0,EP_RX_VALID); // for out stage  
        FUSART_Send(USART1 , 'w'); 
    }
    else if ( stEnumeration.enControlState == IN_DATA )
    {
        if (stEnumeration.udTotalSizePresent <= MAX_PACKET_SIZE)
        {
            UserToPMABufferCopy(stEnumeration.unpData, (uint16_t)EP0TX_ADDRESS, stEnumeration.udTotalSizePresent);
            _SetEPTxCount(ENDP0,stEnumeration.udTotalSizePresent);
            stEnumeration.udSizeRem=0;        
            stEnumeration.enControlState = WAIT_STATUS_OUT;
            FUSART_Send(USART1 , 'x');
        }
        else
        {
            if(stEnumeration.udSizeRem > MAX_PACKET_SIZE)
            {
                UserToPMABufferCopy((stEnumeration.unpData + (stEnumeration.udTotalSizePresent - stEnumeration.udSizeRem)) ,(uint16_t)EP0TX_ADDRESS,MAX_PACKET_SIZE);
               _SetEPTxCount(ENDP0,MAX_PACKET_SIZE);
               stEnumeration.udSizeRem-=MAX_PACKET_SIZE;
               stEnumeration.enControlState = IN_DATA;
               FUSART_Send(USART1 , 'y');
        
            } 
            else
            {
                UserToPMABufferCopy((stEnumeration.unpData + (stEnumeration.udTotalSizePresent - stEnumeration.udSizeRem)) \
                                    ,(uint16_t)EP0TX_ADDRESS,stEnumeration.udSizeRem);
                _SetEPTxCount(ENDP0,stEnumeration.udSizeRem);
                _SetEPRxStatus(ENDP0,EP_RX_VALID);
                Set_Status_Out(ENDP0);
                stEnumeration.udSizeRem=0;
                stEnumeration.enControlState = WAIT_STATUS_OUT;  
                FUSART_Send(USART1 , 'z');
        
            }          
        }
        _SetEPTxStatus(ENDP0,EP_TX_VALID);
    }
}



void vClearFeature(void)
{

}

void vSetFeature(void)
{
    
}



enum RequestState StandardGetStatus(struct Enumeration *pstEnumeration)
{
  if (pstEnumeration->wLength == 0)
  {
    pstEnumeration->enRequestState = UNSUPPORTED;
    return UNSUPPORTED;
  }
  else
  {
    pstEnumeration->unpData = (uint8_t *)(&(pstEnumeration->udGetStatus));
    pstEnumeration->udTotalSizePresent = 2;
  }

  /* Reset Status Information */
  pstEnumeration->unpData = 0;

  if ((pstEnumeration->bmRequestType & (RECIEPIENT_BITS|BMREQUESTTYPE_TYPE_BITS))  == (DEVICE_RECIEPIENT|STANDARD_REQUEST))
  {
    /*Get Device Status */
    pstEnumeration->udGetStatus = (uint16_t)(pstEnumeration->unCurrentFeature);
    pstEnumeration->enRequestState = SUPPORTED;
    return SUPPORTED;
  }
  /*Interface Status*/
  else if ((pstEnumeration->bmRequestType & (RECIEPIENT_BITS|BMREQUESTTYPE_TYPE_BITS))  == (INTERFACE_RECIEPIENT|STANDARD_REQUEST))
  {

    pstEnumeration->udGetStatus = 0;
    pstEnumeration->enRequestState = SUPPORTED;
    return SUPPORTED;
  }
  /*Get EndPoint Status*/
  else if ((pstEnumeration->bmRequestType & (RECIEPIENT_BITS|BMREQUESTTYPE_TYPE_BITS))  == (ENDPOINT_RECIEPIENT|STANDARD_REQUEST))
  {
    uint8_t Related_Endpoint;
    uint8_t wIndex0 = pstEnumeration->wIndex & 0x00ff;

    Related_Endpoint = (wIndex0 & 0x0f);
    if (wIndex0 & 0x70)
    {
      /* IN endpoint */
      if (_GetTxStallStatus(Related_Endpoint))
      {
        pstEnumeration->udGetStatus = 0x0001; /* IN Endpoint stalled */
      }
      pstEnumeration->enRequestState = SUPPORTED;
      return SUPPORTED;
    }
    else
    {
      /* OUT endpoint */
      if (_GetRxStallStatus(Related_Endpoint))
      {
        pstEnumeration->udGetStatus = 0x0001; /* OUT Endpoint stalled */
      }
      pstEnumeration->enRequestState = SUPPORTED;
      return SUPPORTED;
    }

  }
  pstEnumeration->enRequestState = NOTFOUND;
  return NOTFOUND;
}


enum RequestState GetConfiguration(struct Enumeration *pstEnumeration)
{
    pstEnumeration->unpData = (uint8_t *)(&(pstEnumeration->unCurrentConfiguration));
    pstEnumeration->udTotalSizePresent = 1;
    pstEnumeration->enRequestState=SUPPORTED;
    return SUPPORTED;
}
