#include "uart_dma.h"
#include <string.h>
#include <stdio.h>
#include "stm32f1xx_hal.h"
#include "main.h"
#include "usart.h"

uint8_t  UART_DMA_Rx_Buf_2[UART_DMA_RX_BUFFER_SIZE] = {0};
uint8_t  UART_DMA_Rx_Len = 0;
uint8_t  UART_DMA_Rx_Ready_2 = 0;

extern UART_HandleTypeDef huart1;

void UART_DMA_Init_2(void)
{
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, UART_DMA_Rx_Buf_2, UART_DMA_RX_BUFFER_SIZE);
    __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
}

void UART_DMA_Process(void)
{
    if (UART_DMA_Rx_Ready_2 == 1)
    {
        char debug_msg[60];

        for(int i = 0; i < UART_DMA_Rx_Len && i < UART_DMA_RX_BUFFER_SIZE; i++)
        {
            sprintf(debug_msg, "RxData[%d]=0x%02X\r\n", i, UART_DMA_Rx_Buf_2[i]);
            HAL_UART_Transmit(&huart2, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
        }

        if (UART_DMA_Rx_Len >= 4 &&
            UART_DMA_Rx_Buf_2[0] == 0x55 &&
            UART_DMA_Rx_Buf_2[3] == 0xAA)
        {
            uint8_t led_num = UART_DMA_Rx_Buf_2[1];
            uint8_t led_state = UART_DMA_Rx_Buf_2[2];

            sprintf(debug_msg, "Cmd: LED=%d, State=%d\r\n", led_num, led_state);
            HAL_UART_Transmit(&huart2, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);

            if (led_num >= 1 && led_num <= 4 && (led_state == 0x00 || led_state == 0x01))
            {
                GPIO_PinState pinState = (led_state == 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET;

                switch (led_num)
                {
                    case 1:
                        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, pinState);
                        sprintf(debug_msg, "LED1 %s\r\n", led_state ? "ON" : "OFF");
                        break;
                    case 2:
                        HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, pinState);
                        sprintf(debug_msg, "LED2 %s\r\n", led_state ? "ON" : "OFF");
                        break;
                    case 3:
                        HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, pinState);
                        sprintf(debug_msg, "LED3 %s\r\n", led_state ? "ON" : "OFF");
                        break;
                    case 4:
                        HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, pinState);
                        sprintf(debug_msg, "LED4 %s\r\n", led_state ? "ON" : "OFF");
                        break;
                }
                HAL_UART_Transmit(&huart2, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
            }
        }

        memset(UART_DMA_Rx_Buf_2, 0, UART_DMA_RX_BUFFER_SIZE);
        UART_DMA_Rx_Ready_2 = 0;
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART1)
    {
        UART_DMA_Rx_Len = Size;
        UART_DMA_Rx_Ready_2 = 1;

        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, UART_DMA_Rx_Buf_2, UART_DMA_RX_BUFFER_SIZE);
        __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
    }
}

void UART_DMA_Send(uint8_t *data, uint16_t len)
{
    HAL_UART_Transmit_DMA(&huart1, data, len);
}
