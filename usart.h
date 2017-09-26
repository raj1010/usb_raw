#ifndef __USART_H
#define __USART_H
 
#include "stm32f30x_rcc.h"
#include "stm32f30x_gpio.h"
 
#define MUSART1_Clock_Enable (1<<14)
#define MUSART2_Clock_Enable (1<<17)
#define MUSART3_Clock_Enable (1<<18)
#define MUSART4_Clock_Enable (1<<19)
#define MUSART5_Clock_Enable (1<<20)



#define MUSART_OVER_CLOCK8		(1<<15)
#define MUSART_OVER_CLOCK16		(0)
#define MUSART_PARITY_NONE 		(0)
#define MUSART_PARITY_ODD 		((1<<9)|(1<<10))
#define MUSART_PARITY_EVEN 		(0|(1<<10))
#define MUSART_TX_ENABLE 		(1<<3 )
#define MUSART_RX_ENABLE 		(1<<2)	
#define MUSART_ENABLE 			(1<<0 )
#define MUSART_M1				(1<<28)
#define MUSART_M0				(1<<12)
#define MUSART_ONE_SB			(0x00000000)
#define MUSART_HALF_SB			(1<<12)
#define MUSART_TWO_SB			(1<<13)
#define MUSART_OH_SB			(1<<12|1<<13)
#define MUSART_TXE 				(1<<7)
#define MUSART_RXNE				(1<<5)


uint8_t FUsart_Clock_Enable( USART_TypeDef* USARTx);
void FUsart_Init(USART_TypeDef* USARTx, uint32_t Buadrate, uint32_t Stop_Bits, uint32_t Parity, uint32_t Word_Length, uint32_t Hardware_Flow_Control);
void FUSART_Send(USART_TypeDef* USARTx , uint8_t Data);
uint8_t FUSART_Recieve(USART_TypeDef* USARTx);
void Print(USART_TypeDef* USARTx , char *a);
void PrintHex(unsigned char a);
void Serial_initialize();


#endif