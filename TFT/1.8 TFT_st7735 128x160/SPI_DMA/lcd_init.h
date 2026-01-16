#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "main.h"

#define USE_HORIZONTAL 1  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏

#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 128
#define LCD_H 160
#else
#define LCD_W 160
#define LCD_H 128
#endif

//-----------------LCD端口定义（仅保留控制引脚，SPI使用硬件）---------------- 
// SPI1硬件通信，不再需要模拟SCLK/MOSI引脚
//#define LCD_SCLK_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_0)//SCL=SCLK
//#define LCD_SCLK_Set() GPIO_SetBits(GPIOA,GPIO_Pin_0)
//#define LCD_MOSI_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_1)//SDA=MOSI
//#define LCD_MOSI_Set() GPIO_SetBits(GPIOA,GPIO_Pin_1)

#define LCD_RES_Clr()  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET)//RES
#define LCD_RES_Set()  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_SET)

#define LCD_DC_Clr()   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET)//DC
#define LCD_DC_Set()   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET)
		     
#define LCD_CS_Clr()   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET)//CS
#define LCD_CS_Set()   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET)

#define LCD_BLK_Clr()  //HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET)//BLK
#define LCD_BLK_Set()  //HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET)

#define u8 uint8_t
#define u16 uint16_t

// 声明SPI句柄（需要在main.c中初始化hspi1）
extern SPI_HandleTypeDef hspi1;

void LCD_GPIO_Init(void);//初始化GPIO
void LCD_Writ_Bus(u8 dat);//硬件SPI时序
void LCD_WR_DATA8(u8 dat);//写入一个字节
void LCD_WR_DATA(u16 dat);//写入两个字节
void LCD_WR_REG(u8 dat);//写入一个指令
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//设置坐标函数
void LCD_Init(void);//LCD初始化

#endif