/*
 *	at20.h
 *	at20温湿度传感器的.c文件
 *	2026/5/7 v0.1
 *	Ancho.YF
 */
#include "aht20.h"

#define AHT20_ADDRESS 0x70//AHT20的IIC硬件地址

void AHT20_Init()//初始化
{
	uint8_t readBuffer;
	HAL_Delay(40);
	HAL_I2C_Master_Receive(&hi2c1, AHT20_ADDRESS, &readBuffer, 1, HAL_MAX_DELAY);
	if ((readBuffer & 0x08) == 0x00) 
	{
    uint8_t sendBuffer[3] = { 0xBE, 0x08, 0x00 };
    HAL_I2C_Master_Transmit(&hi2c1, AHT20_ADDRESS, sendBuffer, 3, HAL_MAX_DELAY);
	}
}


void AHT20_Read(float *Temperature, float *Humidity)
{
    uint8_t sendBuffer[3] = { 0xAC, 0x33, 0x00 };
    uint8_t readBuffer[6];

    // 发送测量命令
    HAL_I2C_Master_Transmit(&hi2c1, AHT20_ADDRESS, sendBuffer, 3, HAL_MAX_DELAY);
    HAL_Delay(75); // 等待测量完成（AHT20典型转换时间）
    // 读取6字节测量数据
    HAL_I2C_Master_Receive(&hi2c1, AHT20_ADDRESS, readBuffer, 6, HAL_MAX_DELAY);

    // 检查传感器状态：bit7为0表示测量完成
    if((readBuffer[0] & 0x80) == 0x00) 
		{
        uint32_t data = 0;
        // 解析湿度数据（20位）
        data = ((uint32_t)readBuffer[3] >> 4) + ((uint32_t)readBuffer[2] << 4) + ((uint32_t)readBuffer[1] << 12);
        *Humidity = data * 100.0f / (1 << 20); // 转换为0-100%RH

        // 解析温度数据（20位）
        data = (((uint32_t)readBuffer[3] & 0x0F) << 16) + ((uint32_t)readBuffer[4] << 8) + (uint32_t)readBuffer[5];
        *Temperature = data * 200.0f / (1 << 20) - 50; // 转换为-50~+150℃
    }
}
