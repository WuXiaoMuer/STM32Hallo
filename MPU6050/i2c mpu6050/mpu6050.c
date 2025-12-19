#include "mpu6050.h"
#include "math.h"


// 静态变量：存储当前配置的量程（用于物理单位转换）
static MPU6050_Accel_Range current_accel_range = ACCEL_RANGE_2G;
static MPU6050_Gyro_Range current_gyro_range = GYRO_RANGE_2000DPS;

/**
 * @brief  设置加速度计量程
 * @param  range: 加速度计量程枚举值
 * @retval HAL_StatusTypeDef: I2C操作状态（HAL_OK/HAL_ERROR等）
 */
HAL_StatusTypeDef MPU6050_Set_Accel_Range(MPU6050_Accel_Range range) {
    uint8_t reg_data = range;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&MPU6050_I2C, MPU6050_ADDR,
                                                ACCEL_CONFIG, 1, &reg_data, 1, 0xFF);
    if (status == HAL_OK) {
        current_accel_range = range; // 更新当前量程
    }
    return status;
}

/**
 * @brief  设置陀螺仪量程
 * @param  range: 陀螺仪量程枚举值
 * @retval HAL_StatusTypeDef: I2C操作状态（HAL_OK/HAL_ERROR等）
 */
HAL_StatusTypeDef MPU6050_Set_Gyro_Range(MPU6050_Gyro_Range range) {
    uint8_t reg_data = range;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&MPU6050_I2C, MPU6050_ADDR,
                                                GYRO_CONFIG, 1, &reg_data, 1, 0xFF);
    if (status == HAL_OK) {
        current_gyro_range = range; // 更新当前量程
    }
    return status;
}

/**
 * @brief  MPU6050初始化（兼容原有逻辑，新增默认量程配置）
 */
void MPU6050_Init(void){
    uint8_t data = 0x00;
    
    // 解除休眠
    HAL_I2C_Mem_Write(&MPU6050_I2C, MPU6050_ADDR, PWR_MGMT_1, 1, &data, 1, 0xff);
    HAL_Delay(10); // 等待传感器唤醒
    
    // 采样分频器
    data = SMPLRT_DIV_VAL;
    HAL_I2C_Mem_Write(&MPU6050_I2C, MPU6050_ADDR, SMPLRT_DIV, 1, &data, 1, 0xff);
    
    // 低通滤波器
    data = CONFIG_VAL;
    HAL_I2C_Mem_Write(&MPU6050_I2C, MPU6050_ADDR, CONFIG, 1, &data, 1, 0xff);
    
    // 陀螺仪配置（使用默认值，也可通过Set函数修改）
    data = GYRO_CONFIG_VAL;
    HAL_I2C_Mem_Write(&MPU6050_I2C, MPU6050_ADDR, GYRO_CONFIG, 1, &data, 1, 0xff);
    
    // 加速度计配置（使用默认值，也可通过Set函数修改）
    data = ACCEL_CONFIG_VAL;
    HAL_I2C_Mem_Write(&MPU6050_I2C, MPU6050_ADDR, ACCEL_CONFIG, 1, &data, 1, 0xff);
    
    // 更新静态变量为默认量程
    current_accel_range = (MPU6050_Accel_Range)ACCEL_CONFIG_VAL;
    current_gyro_range = (MPU6050_Gyro_Range)GYRO_CONFIG_VAL;
}

/**
 * @brief  读取MPU6050原始数据（原有函数，完全保留）
 * @retval MPU6050_Data: 16位有符号原始值
 */
MPU6050_Data MPU6050_Read(void){
    uint8_t data[14];
    // 读取加速度计和陀螺仪数据
    HAL_I2C_Mem_Read(&MPU6050_I2C, MPU6050_ADDR, ACCEL_XOUT_H, 1, data, 14, 0xff);
    // 解析数据
    MPU6050_Data mpu6050_data;
    mpu6050_data.accel_x = (data[0] << 8) | data[1];
    mpu6050_data.accel_y = (data[2] << 8) | data[3];
    mpu6050_data.accel_z = (data[4] << 8) | data[5];
    mpu6050_data.temp = (data[6] << 8) | data[7];
    mpu6050_data.gyro_x = (data[8] << 8) | data[9];
    mpu6050_data.gyro_y = (data[10] << 8) | data[11];
    mpu6050_data.gyro_z = (data[12] << 8) | data[13];
    return mpu6050_data;
}

/**
 * @brief  读取MPU6050数据并转换为物理单位（新增核心函数）
 * @retval MPU6050_Physic_Data: 直观的物理单位值（m/s²、℃、°/s）
 */
MPU6050_Physic_Data MPU6050_Read_Physic(void) {
    // 先读取原始数据
    MPU6050_Data raw_data = MPU6050_Read();
    MPU6050_Physic_Data physic_data = {0};
    
    // -------------------------- 加速度计转换（m/s²） --------------------------
    float accel_scale;
    switch (current_accel_range) {
        case ACCEL_RANGE_2G:  accel_scale = 16384.0f; break; // 16384 LSB/G
        case ACCEL_RANGE_4G:  accel_scale = 8192.0f;  break;
        case ACCEL_RANGE_8G:  accel_scale = 4096.0f;  break;
        case ACCEL_RANGE_16G: accel_scale = 2048.0f;  break;
        default:              accel_scale = 16384.0f; break;
    }
    // 1G = 9.80665 m/s²
    physic_data.accel_x = (raw_data.accel_x / accel_scale) * 9.80665f;
    physic_data.accel_y = (raw_data.accel_y / accel_scale) * 9.80665f;
    physic_data.accel_z = (raw_data.accel_z / accel_scale) * 9.80665f;
    
    // -------------------------- 温度转换（℃） --------------------------
    // 公式：temp(℃) = (temp_raw / 340) + 36.53
    physic_data.temp = (raw_data.temp / 340.0f) + 36.53f;
    
    // -------------------------- 陀螺仪转换（°/s） --------------------------
    float gyro_scale;
    switch (current_gyro_range) {
        case GYRO_RANGE_250DPS:  gyro_scale = 131.0f;  break; // 131 LSB/(°/s)
        case GYRO_RANGE_500DPS:  gyro_scale = 65.5f;   break;
        case GYRO_RANGE_1000DPS: gyro_scale = 32.8f;   break;
        case GYRO_RANGE_2000DPS: gyro_scale = 16.4f;   break;
        default:                 gyro_scale = 16.4f;   break;
    }
    physic_data.gyro_x = raw_data.gyro_x / gyro_scale;
    physic_data.gyro_y = raw_data.gyro_y / gyro_scale;
    physic_data.gyro_z = raw_data.gyro_z / gyro_scale;
    
    return physic_data;
}