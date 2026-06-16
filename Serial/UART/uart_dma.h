#ifndef __UART_DMA_H
#define __UART_DMA_H

#include "main.h"

#define UART_DMA_RX_BUFFER_SIZE 128

extern uint8_t  UART_DMA_Rx_Buf_2[UART_DMA_RX_BUFFER_SIZE];
extern uint8_t  UART_DMA_Rx_Len;
extern uint8_t  UART_DMA_Rx_Ready_2;

void UART_DMA_Init_2(void);
void UART_DMA_Process(void);
void UART_DMA_Send(uint8_t *data, uint16_t len);
#endif
