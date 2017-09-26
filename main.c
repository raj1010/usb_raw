/**
  ******************************************************************************
  * @file    GPIO_IOToggle/main.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    20-September-2012
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x.h"
#include "usart.h"
#include "usb_regs.h"
#include "usb.h"
#include "stdio.h"
#include "stdlib.h"
#include "usb_mem.h"
#include "usb_hid.h"
/** @addtogroup STM32F3_Discovery_Peripheral_Examples
  * @{
  */
#define USE_FULL_ASSERT    1
/** @addtogroup GPIO_IOToggle
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BSRR_VAL 0xff00
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

uint8_t Prev = 0;

static inline void delay(uint32_t delay);
static inline void Delay_Initialize(void);


char buffer[8];

int main(void)
{

  
  Serial_initialize();
  USBHardwareInit();
  Delay_Initialize();

  uint8_t m[4]={0,0,20,0};
  while (1)
  {
   Print(USART1,"hello\n\r");
   if((*DADDR&0b01111111)>0)
   {
    Print(USART1,"ok\n\r"); 
   }
   Print(USART1,buffer);
   delay(1000000);


   if(Prev)
    {
      /* Reset the control token to inform upper layer that a transfer is ongoing */
      Prev = 0;
      
      /* Copy mouse position info in ENDP1 Tx Packet Memory Area*/
      UserToPMABufferCopy(m,(uint16_t)EP1TX_ADDRESS,4);
      /* Enable endpoint for transmission */
      SetEPTxValid(ENDP1);
    }
  }
}










static inline void delay(uint32_t delay)
{
  // look every 72 ticks are equal to one microsecond 
    delay = delay * 72;
    Delay_Initialize();
    while(*DWT_CYCCNT < delay);
}

static inline void Delay_Initialize(void)
{
    /* Enable DWT */
    DEMCR |= DEMCR_TRCENA; 
    *DWT_CYCCNT = 0;             
    /* Enable CPU cycle counter */
    DWT_CTRL |= CYCCNTENA;
}

 
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
