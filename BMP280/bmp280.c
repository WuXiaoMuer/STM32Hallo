

#include "bmp280.h"

#include <stdio.h>

#include "i2c.h"


/**
 * I2C通信地址
 * SDO接地时:I2C地址是:0b1110110,即0x76
 * SDO接VDD时:I2C地址是:0b1110111,即0x77
 * 以接地时为例,完整的地址位为8位:
 * 当主机向从机发送数据的目的是设置(写)从机,最后一位补充0,即0b11101100
 * 当主机向从机发送数据的目的是从从机读取数据,最后一位补充1,即0b11101101
*/
#define BMP280_I2C_ADDRESS 0x76
//id寄存器地址
#define ID_ADDRESS 0xD0
//配置寄存器地址
#define CONFIG_ADDRESS 0xF5
//控制寄存器地址
#define CTRL_ADDRESS 0xF4
//重置寄存器地址
#define RESET_ADDRESS 0xE0
//校准寄存器的起始地址(part_1)
#define CALIBRATION_ADDRESS 0x88
//存储原始读取到温湿度信息的寄存器首地址
#define DATA_ADDRESS 0xF7

//计算补偿使用的变量
static int32_t t_fine;
// 温度和压力补偿值,从补偿寄存器中读取,启动时读取即可
static uint16_t dig_T1;
static int16_t dig_T2;
static int16_t dig_T3;
static uint16_t dig_P1;
static int16_t dig_P2;
static int16_t dig_P3;
static int16_t dig_P4;
static int16_t dig_P5;
static int16_t dig_P6;
static int16_t dig_P7;
static int16_t dig_P8;
static int16_t dig_P9;


//校准温度方法(文档内置)
static int32_t calibration_T(int32_t adc_T);
//校准气压方法(文档内置)
static uint32_t calibration_P(int32_t adc_P);
//获取校准(补偿)值
static HAL_StatusTypeDef readCalibrationData();
//工具方法写入指令
static HAL_StatusTypeDef write_bmp280(uint8_t address, uint8_t cmd);


HAL_StatusTypeDef BMP280_Init()
{
    //先delay一下,确保设备上电了
    HAL_Delay(100);
    //首先检验设备是否正确;
    uint8_t chip_id = 0;
    const HAL_StatusTypeDef r0 = HAL_I2C_Mem_Read(BMP280_I2C, BMP280_I2C_ADDRESS << 1 | 1, ID_ADDRESS,
                                                   I2C_MEMADD_SIZE_8BIT, &chip_id, 1,HAL_MAX_DELAY);
    if (r0 != HAL_OK)
    {
        // 通信失败,返回错误
        return r0;
    }
    if (chip_id != 0x58)
    {
        //设备id错误
        return HAL_ERROR;
    }


    //重置设备设定
    const uint8_t reset_cmd = 0xB6;
    const HAL_StatusTypeDef r1 = write_bmp280(RESET_ADDRESS, reset_cmd);
    if (r1 != HAL_OK)
    {
        return r1;
    }
    // 软复位后需等待 2ms 设备完成初始化,这里等10ms
    HAL_Delay(10);

    // 配置寄存器(0xF5)用于设置设备的速率、滤波器和接口选项, Bits [7:5]：t_sb（休眠时间,BMP只能是110/111）Bits [4:2]：filter（滤波器系数）Bits [0]：spi（兼容SPI模式）
    //休眠200ms,不过滤
    const uint8_t config_cmd = 0xC0;
    const HAL_StatusTypeDef r2 = write_bmp280(CONFIG_ADDRESS, config_cmd);
    if (r2 != HAL_OK)
    {
        return r2;
    }

    //设置ctrl_means寄存器,即控制方法寄存器(0xF4),    Bits [7:5]：osrs_t（温度过采样）Bits [4:2]：osrs_p（气压过采样）Bits [1:0]：mode（工作模式）
    //1倍压力,1倍温度过采样,正常模式;(由于未开启过滤器,此时压力和温度传感器的分辨率是18位)
    const uint8_t ctrl_means_cmd = 0x27;
    const HAL_StatusTypeDef r3 = write_bmp280(CTRL_ADDRESS, ctrl_means_cmd);
    if (r3 != HAL_OK)
    {
        return r3;
    }
    //ctrl_hum 无需设置,因为bmp280不能测定湿度;

    //读取校准值,用于校准读取数据0x88~0xA1
    const HAL_StatusTypeDef r4 = readCalibrationData();
    if (r4 != HAL_OK)
    {
        return r4;
    }

    return HAL_OK;
}


/**
 * 向传感器发送请求,读取温度/气压信息
 * @param temp 温度,除以100为摄氏度
 * @param press 气压,除以100为Pa
 */
HAL_StatusTypeDef BMP280_Read(int32_t* temp, int32_t* press)
{
    uint8_t data[8] = {0};
    const HAL_StatusTypeDef ret = HAL_I2C_Mem_Read(BMP280_I2C, BMP280_I2C_ADDRESS << 1 | 1, DATA_ADDRESS,
                                                   I2C_MEMADD_SIZE_8BIT, data, 8, 1000);

    if (ret != HAL_OK)
    {
        return ret;
    }
    // rx_buffer,读取buffer值看看把,返回数据0-7,共8字节,即0xF7~0xFE;0-2压力,3-5温度,6-7湿度(bmp不存在)
    uint32_t pres_raw = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
    uint32_t temp_raw = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);
    //校准数据(注意实际要除以100使用)
    *temp = calibration_T(temp_raw);
    *press = calibration_P(pres_raw);
    return HAL_OK;
}


/**
 * 向bmp280写入某个寄存器单字节命令,进行配置或指示行为
 * @param address 寄存器地址
 * @param cmd 单字节数据
 */
HAL_StatusTypeDef write_bmp280(const uint8_t address, uint8_t cmd)
{
    const HAL_StatusTypeDef ret = HAL_I2C_Mem_Write(BMP280_I2C, BMP280_I2C_ADDRESS << 1, address,
                                                    I2C_MEMADD_SIZE_8BIT, &cmd, 1, 1000);
    if (ret != HAL_OK)
    {
        return ret;
    }
    return HAL_OK;
}


HAL_StatusTypeDef readCalibrationData()
{
    //补偿(校准)值一共有33个字节存储,需要读取,对于温度和湿度实际读取到0x88~0x9F即可,即24个寄存器
    //第一部分:0x88~0xA1一共26个8位寄存器
    //第二部分:0xE1~0xE7 一共7个8位寄存器,都是和湿度有关的,不读了
    uint8_t data[24] = {0};
    const HAL_StatusTypeDef ret = HAL_I2C_Mem_Read(BMP280_I2C, BMP280_I2C_ADDRESS << 1 | 1, CALIBRATION_ADDRESS,
                                                   I2C_MEMADD_SIZE_8BIT, data, 24, 1000);
    if (ret != HAL_OK)
    {
        return HAL_ERROR;
    }
    dig_T1 = (data[1] << 8) | data[0];
    dig_T2 = (data[3] << 8) | data[2];
    dig_T3 = (data[5] << 8) | data[4];
    dig_P1 = (data[7] << 8) | data[6];
    dig_P2 = (data[9] << 8) | data[8];
    dig_P3 = (data[11] << 8) | data[10];
    dig_P4 = (data[13] << 8) | data[12];
    dig_P5 = (data[15] << 8) | data[14];
    dig_P6 = (data[17] << 8) | data[16];
    dig_P7 = (data[19] << 8) | data[18];
    dig_P8 = (data[21] << 8) | data[20];
    dig_P9 = (data[23] << 8) | data[22];
    return HAL_OK;
}

int32_t calibration_T(int32_t adc_T)
{
    int32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((signed
        long int)dig_T3)) >> 14;

    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

uint32_t calibration_P(int32_t adc_P)
{
    int32_t var1, var2;
    uint32_t P;
    var1 = (t_fine >> 1) - (int32_t)64000;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((int32_t)dig_P6);
    var2 = var2 + ((var1 * ((int32_t)dig_P5)) << 1);
    var2 = (var2 >> 2) + (((int32_t)dig_P4) << 16);
    var1 = (((dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + ((((int32_t)dig_P2) * var1) >> 1)) >> 18;
    var1 = ((((32768 + var1)) * ((int32_t)dig_P1)) >> 15);
    if (var1 == 0)
    {
        return 0;
    }
    P = (((uint32_t)(((int32_t)1048576) - adc_P) - (var2 >> 12))) * 3125;
    if (P < 0x80000000)
    {
        P = (P << 1) / ((uint32_t)var1);
    }
    else
    {
        P = (P / (uint32_t)var1) * 2;
    }
    var1 = (((int32_t)dig_P9) * ((int32_t)(((P >> 3) * (P >> 3)) >> 13))) >> 12;
    var2 = (((int32_t)(P >> 2)) * ((int32_t)dig_P8)) >> 13;
    P = (uint32_t)((int32_t)P + ((var1 + var2 + dig_P7) >> 4));
    return P;
}

