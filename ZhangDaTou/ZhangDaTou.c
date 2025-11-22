#include "ZhangDaTou.h"
#include <string.h>

extern UART_HandleTypeDef huart1;

static HAL_StatusTypeDef ZHDT_SendCmd(uint8_t *cmd, uint16_t cmd_len, uint8_t *resp, uint16_t resp_len, uint32_t timeout)
{
    __HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_RXNE);
    HAL_StatusTypeDef s = HAL_UART_Transmit(&huart1, cmd, cmd_len, timeout);
    if (s != HAL_OK) return s;
    s = HAL_UART_Receive(&huart1, resp, resp_len, timeout);
    if (s != HAL_OK) return s;
    if (resp[0] != cmd[0] || resp[resp_len - 1] != MOTOR_CHECK_CODE) return HAL_ERROR;
    return HAL_OK;
}

// ====================== 触发动作 ======================
HAL_StatusTypeDef ZHDT_Motor_CalibrateEncoder(uint8_t addr)
{
    uint8_t cmd[] = {addr, 0x06, 0x45, MOTOR_CHECK_CODE};
    uint8_t resp[4]; HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 4, resp, 4, 2000);
    return (s == HAL_OK && resp[2] == CMD_SUCCESS) ? HAL_OK : HAL_ERROR;
}

HAL_StatusTypeDef ZHDT_Motor_SetCurrentPosAsZero(uint8_t addr)
{
    uint8_t cmd[] = {addr, 0x0A, 0x6D, MOTOR_CHECK_CODE};
    uint8_t resp[4]; HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 4, resp, 4, 500);
    return (s == HAL_OK && resp[2] == CMD_SUCCESS) ? HAL_OK : HAL_ERROR;
}

HAL_StatusTypeDef ZHDT_Motor_ResetStallProtect(uint8_t addr)
{
    uint8_t cmd[] = {addr, 0x0E, 0x52, MOTOR_CHECK_CODE};
    uint8_t resp[4]; HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 4, resp, 4, 500);
    return (s == HAL_OK && resp[2] == CMD_SUCCESS) ? HAL_OK : HAL_ERROR;
}

HAL_StatusTypeDef ZHDT_Motor_RestoreFactory(uint8_t addr)
{
    uint8_t cmd[] = {addr, 0x0F, 0x5F, MOTOR_CHECK_CODE};
    uint8_t resp[4]; HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 4, resp, 4, 1000);
    return (s == HAL_OK && resp[2] == CMD_SUCCESS) ? HAL_OK : HAL_ERROR;
}

HAL_StatusTypeDef ZHDT_Motor_ReinitComm(uint8_t addr)
{
    uint8_t cmd[] = {addr, 0xDA, 0x37, MOTOR_CHECK_CODE};
    uint8_t resp[4]; HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 4, resp, 4, 500);
    return (s == HAL_OK && resp[2] == CMD_SUCCESS) ? HAL_OK : HAL_ERROR;
}

// ====================== 控制动作 ======================
HAL_StatusTypeDef ZHDT_Motor_Enable(uint8_t addr, uint8_t en, uint8_t sync)
{
    uint8_t cmd[] = {addr, 0xF3, 0xAB, en, sync, MOTOR_CHECK_CODE};
    uint8_t resp[4]; HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 6, resp, 4, 500);
    return (s == HAL_OK && resp[2] == CMD_SUCCESS) ? HAL_OK : HAL_ERROR;
}

HAL_StatusTypeDef ZHDT_Motor_TorqueMode(uint8_t addr, uint8_t dir,
    uint16_t ramp_ma_per_s, uint16_t current_ma, uint8_t sync)
{
    uint8_t cmd[9] = {addr, 0xF5, dir,
        (ramp_ma_per_s >> 8), (ramp_ma_per_s & 0xFF),
        (current_ma >> 8), (current_ma & 0xFF),
        sync, MOTOR_CHECK_CODE};
    uint8_t resp[4]; HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 9, resp, 4, 500);
    return (s == HAL_OK && resp[2] == CMD_SUCCESS) ? HAL_OK : HAL_ERROR;
}

HAL_StatusTypeDef ZHDT_Motor_SpeedMode(uint8_t addr, uint8_t dir,
    uint16_t acc_rpm_per_s, uint16_t speed_x10, uint8_t sync)
{
    uint8_t cmd[9] = {addr, 0xF6, dir,
        (acc_rpm_per_s >> 8), (acc_rpm_per_s & 0xFF),
        (speed_x10 >> 8), (speed_x10 & 0xFF),
        sync, MOTOR_CHECK_CODE};
    uint8_t resp[4]; HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 9, resp, 4, 500);
    return (s == HAL_OK && resp[2] == CMD_SUCCESS) ? HAL_OK : HAL_ERROR;
}

HAL_StatusTypeDef ZHDT_Motor_DirectPosMode(uint8_t addr, uint8_t dir,
    int32_t pos_x10, uint8_t pos_mode, uint8_t sync)
{
    uint8_t cmd[11] = {addr, 0xFA, dir,
        (pos_x10 >> 24) & 0xFF, (pos_x10 >> 16) & 0xFF,
        (pos_x10 >> 8) & 0xFF, pos_x10 & 0xFF,
        pos_mode, sync, MOTOR_CHECK_CODE};
    uint8_t resp[4]; HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 10, resp, 4, 500);
    return (s == HAL_OK && resp[2] == CMD_SUCCESS) ? HAL_OK : HAL_ERROR;
}

HAL_StatusTypeDef ZHDT_Motor_LimitedSpeedPosMode(uint8_t addr, uint8_t dir,
    uint16_t speed_x10, int32_t pos_x10, uint8_t pos_mode, uint8_t sync)
{
    uint8_t cmd[13] = {addr, 0xFB, dir,
        (speed_x10 >> 8), (speed_x10 & 0xFF),
        (pos_x10 >> 24) & 0xFF, (pos_x10 >> 16) & 0xFF,
        (pos_x10 >> 8) & 0xFF, pos_x10 & 0xFF,
        pos_mode, sync, MOTOR_CHECK_CODE};
    uint8_t resp[4]; HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 12, resp, 4, 500);
    return (s == HAL_OK && resp[2] == CMD_SUCCESS) ? HAL_OK : HAL_ERROR;
}

HAL_StatusTypeDef ZHDT_Motor_TrapezoidalPosMode(uint8_t addr, uint8_t dir,
    uint16_t acc_up, uint16_t acc_down, uint16_t max_speed_x10,
    int32_t pos_x10, uint8_t pos_mode, uint8_t sync)
{
    uint8_t cmd[16] = {addr, 0xFD, dir,
        (acc_up >> 8), (acc_up & 0xFF),
        (acc_down >> 8), (acc_down & 0xFF),
        (max_speed_x10 >> 8), (max_speed_x10 & 0xFF),
        (pos_x10 >> 24) & 0xFF, (pos_x10 >> 16) & 0xFF,
        (pos_x10 >> 8) & 0xFF, pos_x10 & 0xFF,
        pos_mode, sync, MOTOR_CHECK_CODE};
    uint8_t resp[4]; HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 16, resp, 4, 1000);
    return (s == HAL_OK && resp[2] == CMD_SUCCESS) ? HAL_OK : HAL_ERROR;
}

HAL_StatusTypeDef ZHDT_Motor_StopImmediately(uint8_t addr, uint8_t sync)
{
    uint8_t cmd[] = {addr, 0xFE, 0x98, sync, MOTOR_CHECK_CODE};
    uint8_t resp[4]; HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 5, resp, 4, 500);
    return (s == HAL_OK && resp[2] == CMD_SUCCESS) ? HAL_OK : HAL_ERROR;
}

HAL_StatusTypeDef ZHDT_Motor_SyncStart(uint8_t addr)
{
    uint8_t cmd[] = {addr, 0xFF, 0x66, MOTOR_CHECK_CODE};
    uint8_t resp[4]; HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 4, resp, 4, 500);
    return (s == HAL_OK && resp[2] == CMD_SUCCESS) ? HAL_OK : HAL_ERROR;
}

// ====================== 读取参数 ======================
HAL_StatusTypeDef ZHDT_Motor_ReadVersion(uint8_t addr, uint8_t *fw, uint8_t *hw)
{
    uint8_t cmd[] = {addr, 0x1F, MOTOR_CHECK_CODE};
    uint8_t resp[6]; HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 3, resp, 6, 500);
    if (s == HAL_OK && resp[2] != CMD_ERR_INVALID) { *fw = resp[3]; *hw = resp[4]; return HAL_OK; }
    return HAL_ERROR;
}

HAL_StatusTypeDef ZHDT_Motor_ReadR_L(uint8_t addr, uint16_t *r, uint16_t *l)
{
    uint8_t cmd[] = {addr, 0x20, MOTOR_CHECK_CODE};
    uint8_t resp[8]; HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 3, resp, 8, 500);
    if (s == HAL_OK && resp[2] != CMD_ERR_INVALID) {
        *r = (resp[3] << 8) | resp[4];
        *l = (resp[5] << 8) | resp[6];
        return HAL_OK;
    }
    return HAL_ERROR;
}

HAL_StatusTypeDef ZHDT_Motor_ReadPosPID(uint8_t addr, uint32_t *kp1, uint32_t *kp2, uint32_t *kps, uint32_t *kis)
{
    uint8_t cmd[] = {addr, 0x21, MOTOR_CHECK_CODE};
    uint8_t resp[20]; HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 3, resp, 20, 1000);
    if (s == HAL_OK && resp[2] != CMD_ERR_INVALID) {
        *kp1 = (resp[3] << 24) | (resp[4] << 16) | (resp[5] << 8) | resp[6];
        *kp2 = (resp[7] << 24) | (resp[8] << 16) | (resp[9] << 8) | resp[10];
        *kps = (resp[11] << 24) | (resp[12] << 16) | (resp[13] << 8) | resp[14];
        *kis = (resp[15] << 24) | (resp[16] << 16) | (resp[17] << 8) | resp[18];
        return HAL_OK;
    }
    return HAL_ERROR;
}

HAL_StatusTypeDef ZHDT_Motor_ReadBusVoltage(uint8_t addr, uint16_t *v)
{
    uint8_t cmd[] = {addr, 0x24, MOTOR_CHECK_CODE};
    uint8_t resp[6]; HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 3, resp, 6, 500);
    if (s == HAL_OK && resp[2] != CMD_ERR_INVALID) { *v = (resp[3] << 8) | resp[4]; return HAL_OK; }
    return HAL_ERROR;
}

HAL_StatusTypeDef ZHDT_Motor_ReadPhaseCurrent(uint8_t addr, uint16_t *i)
{
    uint8_t cmd[] = {addr, 0x27, MOTOR_CHECK_CODE};
    uint8_t resp[6]; HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 3, resp, 6, 500);
    if (s == HAL_OK && resp[2] != CMD_ERR_INVALID) { *i = (resp[3] << 8) | resp[4]; return HAL_OK; }
    return HAL_ERROR;
}

HAL_StatusTypeDef ZHDT_Motor_ReadEncoderRaw(uint8_t addr, uint16_t *val)
{
    uint8_t cmd[] = {addr, 0x30, MOTOR_CHECK_CODE};
    uint8_t resp[6]; HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 3, resp, 6, 500);
    if (s == HAL_OK && resp[2] != CMD_ERR_INVALID) { *val = (resp[3] << 8) | resp[4]; return HAL_OK; }
    return HAL_ERROR;
}

HAL_StatusTypeDef ZHDT_Motor_ReadEncoderCalibrated(uint8_t addr, uint16_t *val)
{
    uint8_t cmd[] = {addr, 0x31, MOTOR_CHECK_CODE};
    uint8_t resp[6]; HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 3, resp, 6, 500);
    if (s == HAL_OK && resp[2] != CMD_ERR_INVALID) { *val = (resp[3] << 8) | resp[4]; return HAL_OK; }
    return HAL_ERROR;
}

HAL_StatusTypeDef ZHDT_Motor_ReadTargetPos(uint8_t addr, int32_t *pos)
{
    uint8_t cmd[] = {addr, 0x33, MOTOR_CHECK_CODE};
    uint8_t resp[9]; HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 3, resp, 9, 500);
    if (s == HAL_OK && resp[2] != CMD_ERR_INVALID) {
        uint32_t val = (resp[4] << 24) | (resp[5] << 16) | (resp[6] << 8) | resp[7];
        *pos = (resp[3] == 0x01) ? -(int32_t)val : (int32_t)val;
        return HAL_OK;
    }
    return HAL_ERROR;
}

HAL_StatusTypeDef ZHDT_Motor_ReadRealTimePos(uint8_t addr, int32_t *pos)
{
    uint8_t cmd[] = {addr, 0x36, MOTOR_CHECK_CODE}; // 注意：是 0x36！
    uint8_t resp[9]; HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 3, resp, 9, 500);
    if (s == HAL_OK && resp[2] != CMD_ERR_INVALID) {
        uint32_t val = (resp[4] << 24) | (resp[5] << 16) | (resp[6] << 8) | resp[7];
        *pos = (resp[3] == 0x01) ? -(int32_t)val : (int32_t)val;
        return HAL_OK;
    }
    return HAL_ERROR;
}

HAL_StatusTypeDef ZHDT_Motor_ReadSpeed(uint8_t addr, int32_t *speed)
{
    uint8_t cmd[] = {addr, 0x35, MOTOR_CHECK_CODE};
    uint8_t resp[6]; HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 3, resp, 6, 500);
    if (s == HAL_OK && resp[2] != CMD_ERR_INVALID) {
        uint16_t val = (resp[4] << 8) | resp[5];
        *speed = (resp[3] == 0x01) ? -(int32_t)val : (int32_t)val;
        return HAL_OK;
    }
    return HAL_ERROR;
}

HAL_StatusTypeDef ZHDT_Motor_ReadPosError(uint8_t addr, int32_t *err)
{
    uint8_t cmd[] = {addr, 0x37, MOTOR_CHECK_CODE};
    uint8_t resp[9]; HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 3, resp, 9, 500);
    if (s == HAL_OK && resp[2] != CMD_ERR_INVALID) {
        uint32_t val = (resp[4] << 24) | (resp[5] << 16) | (resp[6] << 8) | resp[7];
        *err = (resp[3] == 0x01) ? -(int32_t)val : (int32_t)val; // 注意：*err/100.0 才是度
        return HAL_OK;
    }
    return HAL_ERROR;
}

HAL_StatusTypeDef ZHDT_Motor_ReadTemperature(uint8_t addr, int16_t *temp)
{
    uint8_t cmd[] = {addr, 0x39, MOTOR_CHECK_CODE};
    uint8_t resp[6]; HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 3, resp, 6, 500);
    if (s == HAL_OK && resp[2] != CMD_ERR_INVALID) {
        *temp = (resp[3] == 0x01) ? -((int16_t)((resp[4] << 8) | resp[5])) : ((int16_t)((resp[4] << 8) | resp[5]));
        return HAL_OK;
    }
    return HAL_ERROR;
}

HAL_StatusTypeDef ZHDT_Motor_ReadMotorStatus(uint8_t addr, uint8_t *status)
{
    uint8_t cmd[] = {addr, 0x3A, MOTOR_CHECK_CODE};
    uint8_t resp[5]; HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 3, resp, 5, 500);
    if (s == HAL_OK && resp[2] != CMD_ERR_INVALID) { *status = resp[3]; return HAL_OK; }
    return HAL_ERROR;
}



HAL_StatusTypeDef ZHDT_Motor_ReadDriverConfig(uint8_t addr, uint8_t *buf, uint16_t len)
{
    if (!buf || len < 38) return HAL_ERROR; // 至少 38 字节参数（见 P41 表）

    uint8_t cmd[] = {addr, 0x42, 0x6C, MOTOR_CHECK_CODE};
    uint8_t resp[42] = {0}; // 1+1+38+1 = 41

    HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 4, resp, 41, 1500);
    if (s != HAL_OK || resp[2] == CMD_ERR_INVALID) return HAL_ERROR;

    memcpy(buf, &resp[3], 38); // 提取 38 字节配置参数
    return HAL_OK;
}

HAL_StatusTypeDef ZHDT_Motor_WriteDriverConfig(uint8_t addr, uint8_t save, uint8_t *buf)
{
    if (!buf) return HAL_ERROR;

    uint8_t cmd[42] = {0}; // 1(addr)+1(func)+1(sub)+1(save)+38(data)+1(chk) = 43
    uint8_t resp[4] = {0};

    cmd[0] = addr;
    cmd[1] = 0x48;
    cmd[2] = 0xD1;
    cmd[3] = save; // 0=不保存, 1=掉电保存
    memcpy(&cmd[4], buf, 38);
    cmd[42] = MOTOR_CHECK_CODE;

    HAL_StatusTypeDef s = ZHDT_SendCmd(cmd, 43, resp, 4, 1500);
    return (s == HAL_OK && resp[2] == CMD_SUCCESS) ? HAL_OK : HAL_ERROR;
}



