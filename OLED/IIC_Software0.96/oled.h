#ifndef __OLED_H
#define __OLED_H

#include "stm32f1xx_hal.h"

// 引脚宏定义（PB8:SCL, PB9:SDA）
#define OLED_W_SCL(x) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, (GPIO_PinState)(x))
#define OLED_W_SDA(x) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, (GPIO_PinState)(x))

// 函数声明
void OLED_I2C_Init(void);
void OLED_I2C_Start(void);
void OLED_I2C_Stop(void);
void OLED_I2C_SendByte(uint8_t Byte);
void OLED_WriteCommand(uint8_t Command);
void OLED_WriteData(uint8_t Data);
void OLED_SetCursor(uint8_t Y, uint8_t X);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
void OLED_Init(void);

#endif