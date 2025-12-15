// ------------------- 修改 oled.h -------------------
#ifndef __OLED_H
#define __OLED_H 

#include "main.h"
#include "stdlib.h"	

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

// 移除复位引脚宏定义（无硬件复位时）
//#define OLED_RES_Clr()  HAL_GPIO_WritePin(RES_GPIO_Port,RES_Pin, GPIO_PIN_RESET)
//#define OLED_RES_Set()  HAL_GPIO_WritePin(RES_GPIO_Port,RES_Pin, GPIO_PIN_SET)

#define OLED_I2C_ADDR    0x78  // SSD1306 I2C地址
#define OLED_CMD  0	// 写命令
#define OLED_DATA 1	// 写数据

// 函数声明（删除未用到的Scroll/Picture函数，避免警告）
void OLED_ColorTurn(u8 i);
void OLED_DisplayTurn(u8 i);
void OLED_WR_Byte(u8 dat, u8 mode);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1,u8 mode);
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode);
void OLED_Init(void);

#endif
// 新增空行，修复#1-D警告