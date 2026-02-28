#ifndef __FDC2214_H
#define __FDC2214_H
#include "sys.h"
#include "delay.h"
#include "usart.h"

// I2C GPIO配置（PB6=SCL, PB7=SDA）
#define FDC_SDA_IN()    {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<28;}  // 输入模式
#define FDC_SDA_OUT()   {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)3<<28;}  // 推挽输出

#define FDC_IIC_SCL     PBout(6)
#define FDC_IIC_SDA     PBout(7)
#define FDC_READ_SDA    PBin(7)

// FDC2214 I2C地址
#define FDC2214_ADDR    0x2A

// 寄存器地址定义
#define DATA_CH0        0x00
#define DATA_LSB_CH0    0x01
#define DATA_CH1        0x02
#define DATA_LSB_CH1    0x03
#define DATA_CH2        0x04
#define DATA_LSB_CH2    0x05
#define DATA_CH3        0x06
#define DATA_LSB_CH3    0x07
#define RCOUNT_CH0      0x08
#define RCOUNT_CH1      0x09
#define RCOUNT_CH2      0x0A
#define RCOUNT_CH3      0x0B
#define SETTLECOUNT_CH0 0x10
#define SETTLECOUNT_CH1 0x11
#define SETTLECOUNT_CH2 0x12
#define SETTLECOUNT_CH3 0x13
#define CLOCK_DIVIDERS_C_CH0    0x14
#define CLOCK_DIVIDERS_C_CH1    0x15
#define CLOCK_DIVIDERS_C_CH2    0x16
#define CLOCK_DIVIDERS_C_CH3    0x17
#define STATUS          0x18
#define ERROR_CONFIG    0x19
#define CONFIG          0x1A  
#define MUX_CONFIG      0x1B
#define RESET_DEV       0x1C
#define DRIVE_CURRENT_CH0   0x1E
#define DRIVE_CURRENT_CH1   0x1F
#define DRIVE_CURRENT_CH2   0x20
#define DRIVE_CURRENT_CH3   0x21
#define MANUFACTURER_ID     0x7E    // 厂商ID：0x5449（原代码注释错误，已修正）
#define DEVICE_ID           0x7F    // 设备ID：0x3055

// 函数声明
void FDC_IIC_Delay(void);
void FDC_IIC_Init(void);
void FDC_IIC_Start(void);
void FDC_IIC_Stop(void);
u8 FDC_IIC_Wait_Ack(void);
void FDC_IIC_Ack(void);
void FDC_IIC_NAck(void);
void FDC_IIC_Send_Byte(u8 txd);
u8 FDC_IIC_Read_Byte(u8 ack);
u8 Set_FDC2214(u8 reg,u8 MSB,u8 LSB);
u16 FDC_Read(u8 reg);
u32 FCD2214_ReadCH(u8 index);
u8 FDC2214_Init(void);
float Cap_Calculate(u8 chx);
// 新增：带滤波的电容读取函数（提升稳定性）
float Cap_Calculate_Filter(u8 chx, u8 avg_cnt);

#endif