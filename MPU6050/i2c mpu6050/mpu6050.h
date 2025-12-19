#ifndef MPU6050_H
#define MPU6050_H

#include "type.h"

// 原有I2C句柄定义
extern I2C_HandleTypeDef hi2c3;
#define MPU6050_I2C              hi2c3      // 使用的 I2C 句柄

// 原有器件地址定义
#define MPU6050_ADDR             0xD0        // MPU6050 默认 I2C 地址 (AD0 引脚接地) 0x68 << 1 = 0xD0

// 原有寄存器地址定义
#define SMPLRT_DIV               0x19        // 采样率分频器
#define CONFIG                   0x1A        // 配置寄存器
#define GYRO_CONFIG              0x1B        // 陀螺仪配置寄存器
#define ACCEL_CONFIG             0x1C        // 加速度计配置寄存器

#define ACCEL_XOUT_H             0x3B        // 加速度计 X 轴高字节
#define ACCEL_XOUT_L             0x3C        // 加速度计 X 轴低字节
#define ACCEL_YOUT_H             0x3D        // 加速度计 Y 轴高字节
#define ACCEL_YOUT_L             0x3E        // 加速度计 Y 轴低字节
#define ACCEL_ZOUT_H             0x3F        // 加速度计 Z 轴高字节
#define ACCEL_ZOUT_L             0x40        // 加速度计 Z 轴低字节

#define TEMP_OUT_H               0x41        // 温度传感器高字节
#define TEMP_OUT_L               0x42        // 温度传感器低字节

#define GYRO_XOUT_H              0x43        // 陀螺仪 X 轴高字节
#define GYRO_XOUT_L              0x44        // 陀螺仪 X 轴低字节
#define GYRO_YOUT_H              0x45        // 陀螺仪 Y 轴高字节
#define GYRO_YOUT_L              0x46        // 陀螺仪 Y 轴低字节
#define GYRO_ZOUT_H              0x47        // 陀螺仪 Z 轴高字节
#define GYRO_ZOUT_L              0x48        // 陀螺仪 Z 轴低字节

#define PWR_MGMT_1               0x6B        // 电源管理寄存器 1
#define WHO_AM_I                 0x75        // 器件 ID 寄存器

// 原有寄存器配置参数 (默认值)
#define SMPLRT_DIV_VAL           0x07        // 采样率 = 1kHz / (1 + 7) = 125Hz
#define CONFIG_VAL               0x06        // 低通滤波器截止频率 = 5Hz
#define GYRO_CONFIG_VAL          0x18        // 陀螺仪满量程 = ±2000 °/s
#define ACCEL_CONFIG_VAL         0x00        // 加速度计满量程 = ±2g

// -------------------------- 新增：量程配置枚举 --------------------------
/**
 * @brief 加速度计量程枚举
 */
typedef enum {
    ACCEL_RANGE_2G  = 0x00,  // 加速度计量程±2G (对应ACCEL_CONFIG_VAL默认值)
    ACCEL_RANGE_4G  = 0x08,  // 加速度计量程±4G
    ACCEL_RANGE_8G  = 0x10,  // 加速度计量程±8G
    ACCEL_RANGE_16G = 0x18   // 加速度计量程±16G
} MPU6050_Accel_Range;

/**
 * @brief 陀螺仪量程枚举
 */
typedef enum {
    GYRO_RANGE_250DPS  = 0x00,  // 陀螺仪量程±250°/s
    GYRO_RANGE_500DPS  = 0x08,  // 陀螺仪量程±500°/s
    GYRO_RANGE_1000DPS = 0x10,  // 陀螺仪量程±1000°/s
    GYRO_RANGE_2000DPS = 0x18   // 陀螺仪量程±2000°/s (对应GYRO_CONFIG_VAL默认值)
} MPU6050_Gyro_Range;

// -------------------------- 原有数据结构体（保留） --------------------------
/**
 * @brief MPU6050原始数据结构体（16位有符号原始值）
 */
typedef struct{
    int16_t accel_x;//加速度计 X 轴原始值
    int16_t accel_y;//加速度计 Y 轴原始值
    int16_t accel_z;//加速度计 Z 轴原始值
    int16_t temp;   //温度传感器原始值
    int16_t gyro_x; //陀螺仪 X 轴原始值
    int16_t gyro_y; //陀螺仪 Y 轴原始值
    int16_t gyro_z; //陀螺仪 Z 轴原始值
} MPU6050_Data;

// -------------------------- 新增：物理单位数据结构体 --------------------------
/**
 * @brief MPU6050物理单位数据结构体（直观可读）
 */
typedef struct {
    float accel_x;  // 加速度X轴 (m/s²)
    float accel_y;  // 加速度Y轴 (m/s²)
    float accel_z;  // 加速度Z轴 (m/s²)
    float temp;     // 温度 (℃)
    float gyro_x;   // 陀螺仪X轴 (°/s)
    float gyro_y;   // 陀螺仪Y轴 (°/s)
    float gyro_z;   // 陀螺仪Z轴 (°/s)
} MPU6050_Physic_Data;

// -------------------------- 函数声明 --------------------------
// 原有函数（保留）
void MPU6050_Init(void);
MPU6050_Data MPU6050_Read(void);

// 新增：量程配置函数
HAL_StatusTypeDef MPU6050_Set_Accel_Range(MPU6050_Accel_Range range); // 设置加速度计量程
HAL_StatusTypeDef MPU6050_Set_Gyro_Range(MPU6050_Gyro_Range range);   // 设置陀螺仪量程

// 新增：读取并转换为物理单位的函数
MPU6050_Physic_Data MPU6050_Read_Physic(void);

#endif

      //显示加�?�度 accX,accY,accZ 角�?�度 gyroXYZ
//      point.y += 16;
//      snprintf(buf, sizeof(buf), "accX:%04d  ", mpu6050_data.accel_x);
//      lcd_ascii(&point, buf, 16, BLACK);
//      point.y += 16;
//      snprintf(buf, sizeof(buf), "accY:%04d  ", mpu6050_data.accel_y);
//      lcd_ascii(&point, buf, 16, BLACK);
//      point.y += 16;
//      snprintf(buf, sizeof(buf), "accZ:%04d  ", mpu6050_data.accel_z);
//      lcd_ascii(&point, buf, 16, BLACK);
//      point.y += 16;
//      snprintf(buf, sizeof(buf), "gyroX:%04d  ", mpu6050_data.gyro_x);
//      lcd_ascii(&point, buf, 16, BLACK);
//      point.y += 16;
//      snprintf(buf, sizeof(buf), "gyroY:%04d  ", mpu6050_data.gyro_y);
//      lcd_ascii(&point, buf, 16, BLACK);
//      point.y += 16;
//      snprintf(buf, sizeof(buf), "gyroZ:%04d  ", mpu6050_data.gyro_z);
//      lcd_ascii(&point, buf, 16, BLACK);