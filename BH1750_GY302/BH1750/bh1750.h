#ifndef __BH1750_H
#define __BH1750_H

#include "stm32f1xx_hal.h"

#define BH1750_ADDR 0x23
#define BH1750_ADDR_ALT 0x5C

#define BH1750_POWER_DOWN 0x00
#define BH1750_POWER_ON 0x01
#define BH1750_RESET 0x07

#define BH1750_CONT_H_MODE 0x10
#define BH1750_CONT_H_MODE2 0x11
#define BH1750_CONT_L_MODE 0x13

#define BH1750_ONE_TIME_H_MODE 0x20
#define BH1750_ONE_TIME_H_MODE2 0x21
#define BH1750_ONE_TIME_L_MODE 0x23

#define BH1750_MEASURE_TIMEOUT 200

typedef enum {
    BH1750_OK = 0,
    BH1750_ERROR = 1,
    BH1750_TIMEOUT = 2,
    BH1750_NOT_FOUND = 3
} BH1750_Status;

typedef enum {
    BH1750_CONTINUOUS_HIGH_RES = 0,
    BH1750_CONTINUOUS_HIGH_RES2,
    BH1750_CONTINUOUS_LOW_RES,
    BH1750_ONETIME_HIGH_RES,
    BH1750_ONETIME_HIGH_RES2,
    BH1750_ONETIME_LOW_RES
} BH1750_Mode;

typedef enum {
    BH1750_STATE_IDLE = 0,
    BH1750_STATE_SENDING_CMD,
    BH1750_STATE_MEASURING,
    BH1750_STATE_READING,
    BH1750_STATE_COMPLETE
} BH1750_State;

typedef struct {
    BH1750_State state;
    uint32_t startTick;
    uint8_t buffer[2];
    float luxValue;
    uint8_t addr;
} BH1750_HandleTypeDef;

void BH1750_InitHandle(BH1750_HandleTypeDef *hBH1750);
BH1750_Status BH1750_StartMeasure(BH1750_HandleTypeDef *hBH1750);
BH1750_Status BH1750_Process(BH1750_HandleTypeDef *hBH1750);
float BH1750_GetLux(BH1750_HandleTypeDef *hBH1750);

BH1750_Status BH1750_Init(void);
BH1750_Status BH1750_DeInit(void);
BH1750_Status BH1750_Reset(void);
BH1750_Status BH1750_SetMode(BH1750_Mode mode);
BH1750_Status BH1750_Measure(BH1750_Mode mode, float *lux);
float BH1750_ReadLux(void);
BH1750_Status BH1750_CheckDevice(void);

#endif
