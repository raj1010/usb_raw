#include "stm32f30x.h"
#include "usart.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>



uint8_t FUsart_Clock_Enable( USART_TypeDef* USARTx)
{
  if (USARTx==USART1)
  {
    RCC->APB2ENR=MUSART1_Clock_Enable;
    return 0;
  }
  else if (USARTx == USART2)
  {
    RCC->APB1ENR=MUSART2_Clock_Enable;
    return 0;
  }
  else if (USARTx == USART3)
  {
    RCC->APB1ENR=MUSART3_Clock_Enable;
    return 0;
  }
  else if (USARTx == UART4)
  {
    RCC->APB1ENR=MUSART4_Clock_Enable;
    return 0;
  }
  else if (USARTx == UART5)
  {
    RCC->APB1ENR=MUSART5_Clock_Enable;
    return 0;
  }
  else 
    return -1;
}




void FUsart_Init(USART_TypeDef* USARTx, uint32_t Buadrate, 
	uint32_t Stop_Bits, uint32_t Parity, uint32_t Word_Length, uint32_t Hardware_Flow_Control)
{
	USARTx->CR1 = MUSART_RX_ENABLE|MUSART_TX_ENABLE;
	if (Word_Length==9)
	{
		USARTx->CR1 = USARTx->CR1 | MUSART_M0;
	}
	else if (Word_Length==7)
	{
		USARTx->CR1 = USARTx->CR1 | MUSART_M1;
	}

	if(Parity==MUSART_PARITY_EVEN)
		USARTx->CR1 = USARTx->CR1 | MUSART_PARITY_EVEN;
	else if(Parity==MUSART_PARITY_ODD)
		USARTx->CR1 = USARTx->CR1 | MUSART_PARITY_ODD;

	if (Stop_Bits==MUSART_TWO_SB || Stop_Bits==MUSART_ONE_SB ||Stop_Bits==MUSART_HALF_SB || Stop_Bits==MUSART_OH_SB )
	{
		USARTx->CR2 = USARTx->CR2 | Stop_Bits;
	}
	if (USARTx==USART1)
	{
		USARTx->BRR=72000000/Buadrate;
	}
	else if ( USARTx>=USART2 || USARTx>=USART3 || USARTx>=UART4 || USARTx>=UART5)
	{
		USARTx->BRR=36000000/Buadrate;	
	}
	USARTx->CR1|=MUSART_ENABLE;
  
}

void FUSART_Send(USART_TypeDef* USARTx , uint8_t Data)
{
	while(!(((USARTx->ISR&MUSART_TXE))==MUSART_TXE));
	USARTx->TDR=Data;
}


uint8_t FUSART_Recieve(USART_TypeDef* USARTx)
{
  while(!((USARTx->ISR&MUSART_RXNE)==0x00));
  return (uint8_t)(USARTx->RDR);
}


void Print(USART_TypeDef* USARTx , char *a) 
{
  int i;
  for (i = 0; a[i] != '\0'; i++)
    FUSART_Send(USARTx , a[i]);
}


void PrintHex(unsigned char a)
{
  char strng[5];
  itoa((int) a,strng,16);
  FUSART_Send(USART1 , ' ');
  Print(USART1,strng);
} 

void Serial_initialize()
{

  RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);
  GPIO_InitTypeDef GPIO_Struct;
  GPIO_Struct.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;
  GPIO_Struct.GPIO_Mode=GPIO_Mode_AF;
  GPIO_Struct.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Struct.GPIO_OType=GPIO_OType_PP;
  GPIO_Struct.GPIO_PuPd=GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_Struct);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9 , GPIO_AF_7);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_7);

  FUsart_Clock_Enable(USART1);
  FUsart_Init(USART1,230400,MUSART_ONE_SB,MUSART_PARITY_NONE,8,0);

}
