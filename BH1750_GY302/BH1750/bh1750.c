#include "bh1750.h"

extern I2C_HandleTypeDef hi2c1;

static BH1750_Mode currentMode = BH1750_CONTINUOUS_HIGH_RES;
static uint8_t bh1750Addr = BH1750_ADDR;

static BH1750_Status BH1750_SendCmd(uint8_t cmd)
{
    if (HAL_I2C_Master_Transmit(&hi2c1, bh1750Addr << 1, &cmd, 1, 100) != HAL_OK) {
        return BH1750_ERROR;
    }
    return BH1750_OK;
}

static BH1750_Status BH1750_ReadData(uint8_t *buf, uint16_t size)
{
    if (HAL_I2C_Master_Receive(&hi2c1, bh1750Addr << 1, buf, size, 100) != HAL_OK) {
        return BH1750_ERROR;
    }
    return BH1750_OK;
}

BH1750_Status BH1750_Init(void)
{
    if (BH1750_CheckDevice() != BH1750_OK) {
        bh1750Addr = BH1750_ADDR_ALT;
        if (BH1750_CheckDevice() != BH1750_OK) {
            return BH1750_NOT_FOUND;
        }
    }

    if (BH1750_Reset() != BH1750_OK) {
        return BH1750_ERROR;
    }

    currentMode = BH1750_CONTINUOUS_HIGH_RES;
    if (BH1750_SetMode(currentMode) != BH1750_OK) {
        return BH1750_ERROR;
    }

    HAL_Delay(180);

    return BH1750_OK;
}

BH1750_Status BH1750_DeInit(void)
{
    if (BH1750_SendCmd(BH1750_POWER_DOWN) != BH1750_OK) {
        return BH1750_ERROR;
    }
    return BH1750_OK;
}

BH1750_Status BH1750_Reset(void)
{
    if (BH1750_SendCmd(BH1750_POWER_ON) != BH1750_OK) {
        return BH1750_ERROR;
    }

    HAL_Delay(10);

    if (BH1750_SendCmd(BH1750_RESET) != BH1750_OK) {
        return BH1750_ERROR;
    }

    HAL_Delay(10);

    return BH1750_OK;
}

BH1750_Status BH1750_SetMode(BH1750_Mode mode)
{
    uint8_t cmd;
    currentMode = mode;

    switch (mode) {
        case BH1750_CONTINUOUS_HIGH_RES:
            cmd = BH1750_CONT_H_MODE;
            break;
        case BH1750_CONTINUOUS_HIGH_RES2:
            cmd = BH1750_CONT_H_MODE2;
            break;
        case BH1750_CONTINUOUS_LOW_RES:
            cmd = BH1750_CONT_L_MODE;
            break;
        case BH1750_ONETIME_HIGH_RES:
            cmd = BH1750_ONE_TIME_H_MODE;
            break;
        case BH1750_ONETIME_HIGH_RES2:
            cmd = BH1750_ONE_TIME_H_MODE2;
            break;
        case BH1750_ONETIME_LOW_RES:
            cmd = BH1750_ONE_TIME_L_MODE;
            break;
        default:
            return BH1750_ERROR;
    }

    return BH1750_SendCmd(cmd);
}

BH1750_Status BH1750_Measure(BH1750_Mode mode, float *lux)
{
    uint8_t buf[2];
    uint16_t data;

    if (BH1750_SetMode(mode) != BH1750_OK) {
        return BH1750_ERROR;
    }

    switch (mode) {
        case BH1750_CONTINUOUS_HIGH_RES:
        case BH1750_ONETIME_HIGH_RES:
            HAL_Delay(180);
            break;
        case BH1750_CONTINUOUS_HIGH_RES2:
        case BH1750_ONETIME_HIGH_RES2:
            HAL_Delay(180);
            break;
        case BH1750_CONTINUOUS_LOW_RES:
        case BH1750_ONETIME_LOW_RES:
            HAL_Delay(24);
            break;
        default:
            return BH1750_ERROR;
    }

    if (BH1750_ReadData(buf, 2) != BH1750_OK) {
        return BH1750_ERROR;
    }

    data = (buf[0] << 8) | buf[1];

    if (mode == BH1750_CONTINUOUS_LOW_RES || mode == BH1750_ONETIME_LOW_RES) {
        *lux = data / 1.2f;
    } else {
        *lux = data / 1.2f;
    }

    return BH1750_OK;
}

float BH1750_ReadLux(void)
{
    uint8_t cmd = BH1750_ONE_TIME_H_MODE;
    uint8_t buf[2];
    uint16_t data;
    float lux;
    uint8_t addr = bh1750Addr;
    
    if (HAL_I2C_IsDeviceReady(&hi2c1, addr << 1, 1, 10) != HAL_OK) {
        addr = BH1750_ADDR_ALT;
        if (HAL_I2C_IsDeviceReady(&hi2c1, addr << 1, 1, 10) != HAL_OK) {
            BH1750_Init();
            addr = bh1750Addr;
            if (HAL_I2C_IsDeviceReady(&hi2c1, addr << 1, 1, 10) != HAL_OK) {
                addr = BH1750_ADDR_ALT;
            }
        }
    }
    
    if (HAL_I2C_Master_Transmit(&hi2c1, addr << 1, &cmd, 1, 100) != HAL_OK) {
        return -1.0f;
    }
    
    HAL_Delay(120);
    
    if (HAL_I2C_Master_Receive(&hi2c1, addr << 1, buf, 2, 100) != HAL_OK) {
        return -1.0f;
    }

    data = (buf[0] << 8) | buf[1];
    lux = data / 1.2f;

    return lux;
}

BH1750_Status BH1750_CheckDevice(void)
{
    if (HAL_I2C_IsDeviceReady(&hi2c1, bh1750Addr << 1, 3, 100) == HAL_OK) {
        return BH1750_OK;
    }
    return BH1750_NOT_FOUND;
}

void BH1750_InitHandle(BH1750_HandleTypeDef *hBH1750)
{
    hBH1750->state = BH1750_STATE_IDLE;
    hBH1750->startTick = 0;
    hBH1750->luxValue = -1.0f;
    hBH1750->addr = bh1750Addr;
}

BH1750_Status BH1750_StartMeasure(BH1750_HandleTypeDef *hBH1750)
{
    if (hBH1750->state != BH1750_STATE_IDLE) {
        return BH1750_ERROR;
    }
    
    uint8_t cmd = BH1750_ONE_TIME_H_MODE;
    
    if (HAL_I2C_Master_Transmit(&hi2c1, hBH1750->addr << 1, &cmd, 1, 50) != HAL_OK) {
        hBH1750->state = BH1750_STATE_IDLE;
        return BH1750_ERROR;
    }
    
    hBH1750->state = BH1750_STATE_MEASURING;
    hBH1750->startTick = HAL_GetTick();
    
    return BH1750_OK;
}

BH1750_Status BH1750_Process(BH1750_HandleTypeDef *hBH1750)
{
    uint32_t currentTick = HAL_GetTick();
    
    switch (hBH1750->state) {
        case BH1750_STATE_IDLE:
            return BH1750_OK;
            
        case BH1750_STATE_MEASURING:
            if ((currentTick - hBH1750->startTick) >= 120) {
                if (HAL_I2C_Master_Receive(&hi2c1, hBH1750->addr << 1, hBH1750->buffer, 2, 50) != HAL_OK) {
                    hBH1750->state = BH1750_STATE_IDLE;
                    return BH1750_ERROR;
                }
                uint16_t data = (hBH1750->buffer[0] << 8) | hBH1750->buffer[1];
                hBH1750->luxValue = data / 1.2f;
                hBH1750->state = BH1750_STATE_COMPLETE;
            }
            break;
            
        case BH1750_STATE_COMPLETE:
            return BH1750_OK;
            
        default:
            break;
    }
    
    if ((currentTick - hBH1750->startTick) > BH1750_MEASURE_TIMEOUT) {
        hBH1750->state = BH1750_STATE_IDLE;
        return BH1750_TIMEOUT;
    }
    
    return BH1750_OK;
}

float BH1750_GetLux(BH1750_HandleTypeDef *hBH1750)
{
    float lux = hBH1750->luxValue;
    hBH1750->state = BH1750_STATE_IDLE;
    hBH1750->luxValue = -1.0f;
    return lux;
}
