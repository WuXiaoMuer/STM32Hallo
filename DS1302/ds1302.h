#ifndef __DS1302_H 
#define __DS1302_H  

#include "main.h" 

// 引脚重新定义：PB3=DATA, PB4=CE, PB5=SCLK
#define DATA_PIN    GPIO_PIN_3
#define CE_PIN      GPIO_PIN_4
#define SCLK_PIN    GPIO_PIN_5
#define DS1302_PORT GPIOB

// HAL库电平操作宏
#define CE_L        HAL_GPIO_WritePin(DS1302_PORT, CE_PIN, GPIO_PIN_RESET)
#define CE_H        HAL_GPIO_WritePin(DS1302_PORT, CE_PIN, GPIO_PIN_SET)
#define SCLK_L      HAL_GPIO_WritePin(DS1302_PORT, SCLK_PIN, GPIO_PIN_RESET)
#define SCLK_H      HAL_GPIO_WritePin(DS1302_PORT, SCLK_PIN, GPIO_PIN_SET)
#define DATA_L      HAL_GPIO_WritePin(DS1302_PORT, DATA_PIN, GPIO_PIN_RESET)
#define DATA_H      HAL_GPIO_WritePin(DS1302_PORT, DATA_PIN, GPIO_PIN_SET)

// 类型定义兼容
#define u16 uint16_t
#define u8  uint8_t

// 时间结构体
struct TIMEData
{
	u16 year;
	u8  month;
	u8  day;
	u8  hour;
	u8  minute;
	u8  second;
	u8  week;
};

extern struct TIMEData TimeData;//全局变量

// 函数声明
void ds1302_gpio_init(void);          // ds1302端口初始化
void ds1302_DATAOUT_init(void);       // IO端口配置为输出
void ds1302_DATAINPUT_init(void);     // IO端口配置为输入
void ds1302_write_onebyte(u8 data);   // 向ds1302发送一字节数据
void ds1302_write_reg(u8 address,u8 data); // 向指定寄存器写一字节数据
u8 ds1302_read_reg(u8 address);       // 从指定寄存器读一字节数据
void ds1302_init(void);               // ds1302初始化函数
void ds1302_read_time(void);          // 从ds1302读取实时时间（BCD码）
void ds1302_read_realTime(void);      // 将BCD码转化为十进制数据

#endif