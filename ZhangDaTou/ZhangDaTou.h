#ifndef ZHDT_MOTOR_H
#define ZHDT_MOTOR_H

#include "stm32f1xx_hal.h"

// 固定参数
#define MOTOR_DEFAULT_ADDR      0x01
#define MOTOR_CHECK_CODE        0x6B
#define CMD_SUCCESS             0x02
#define CMD_ERR_CONDITION       0xE2
#define CMD_ERR_INVALID         0xEE

// 方向
#define DIR_CW                  0x00  // 正转（顺时针）
#define DIR_CCW                 0x01  // 反转（逆时针）

// 多机同步
#define SYNC_DISABLE            0x00
#define SYNC_ENABLE             0x01

// 位置模式
#define POS_RELATIVE            0x00  // 相对位置
#define POS_ABSOLUTE            0x01  // 绝对位置

// 存储标志
#define SAVE_DISABLE            0x00
#define SAVE_ENABLE             0x01

// 控制模式
#define MODE_OPEN_LOOP          0x01
#define MODE_CLOSED_LOOP        0x02

// 函数声明 —— 触发动作
HAL_StatusTypeDef ZHDT_Motor_CalibrateEncoder(uint8_t addr);
HAL_StatusTypeDef ZHDT_Motor_SetCurrentPosAsZero(uint8_t addr);  // 替代“回零”
HAL_StatusTypeDef ZHDT_Motor_ResetStallProtect(uint8_t addr);
HAL_StatusTypeDef ZHDT_Motor_RestoreFactory(uint8_t addr);
HAL_StatusTypeDef ZHDT_Motor_ReinitComm(uint8_t addr);

// 函数声明 —— 控制动作
HAL_StatusTypeDef ZHDT_Motor_Enable(uint8_t addr, uint8_t enable, uint8_t sync);
HAL_StatusTypeDef ZHDT_Motor_TorqueMode(uint8_t addr, uint8_t dir, uint16_t current_ramp_ma_per_s, uint16_t current_ma, uint8_t sync);
HAL_StatusTypeDef ZHDT_Motor_SpeedMode(uint8_t addr, uint8_t dir, uint16_t acc_rpm_per_s, uint16_t speed_x10, uint8_t sync);
HAL_StatusTypeDef ZHDT_Motor_DirectPosMode(uint8_t addr, uint8_t dir, int32_t pos_x10, uint8_t pos_mode, uint8_t sync);
HAL_StatusTypeDef ZHDT_Motor_LimitedSpeedPosMode(uint8_t addr, uint8_t dir, uint16_t speed_x10, int32_t pos_x10, uint8_t pos_mode, uint8_t sync);
HAL_StatusTypeDef ZHDT_Motor_TrapezoidalPosMode(uint8_t addr, uint8_t dir,
    uint16_t acc_up_rpm_per_s, uint16_t acc_down_rpm_per_s,
    uint16_t max_speed_x10, int32_t pos_x10, uint8_t pos_mode, uint8_t sync);
HAL_StatusTypeDef ZHDT_Motor_StopImmediately(uint8_t addr, uint8_t sync);
HAL_StatusTypeDef ZHDT_Motor_SyncStart(uint8_t addr);

// 函数声明 —— 读取参数
HAL_StatusTypeDef ZHDT_Motor_ReadVersion(uint8_t addr, uint8_t *firmware, uint8_t *hardware);
HAL_StatusTypeDef ZHDT_Motor_ReadR_L(uint8_t addr, uint16_t *res_mohm, uint16_t *ind_uh);
HAL_StatusTypeDef ZHDT_Motor_ReadPosPID(uint8_t addr, uint32_t *kp_trap, uint32_t *kp_direct, uint32_t *kp_speed, uint32_t *ki_speed);
HAL_StatusTypeDef ZHDT_Motor_ReadBusVoltage(uint8_t addr, uint16_t *voltage_mv);
HAL_StatusTypeDef ZHDT_Motor_ReadPhaseCurrent(uint8_t addr, uint16_t *current_ma);
HAL_StatusTypeDef ZHDT_Motor_ReadEncoderRaw(uint8_t addr, uint16_t *val);
HAL_StatusTypeDef ZHDT_Motor_ReadEncoderCalibrated(uint8_t addr, uint16_t *val);
HAL_StatusTypeDef ZHDT_Motor_ReadTargetPos(uint8_t addr, int32_t *pos_x10);
HAL_StatusTypeDef ZHDT_Motor_ReadRealTimePos(uint8_t addr, int32_t *pos_x10);
HAL_StatusTypeDef ZHDT_Motor_ReadSpeed(uint8_t addr, int32_t *speed_x10);
HAL_StatusTypeDef ZHDT_Motor_ReadPosError(uint8_t addr, int32_t *error_x100);
HAL_StatusTypeDef ZHDT_Motor_ReadTemperature(uint8_t addr, int16_t *temp_c);
HAL_StatusTypeDef ZHDT_Motor_ReadMotorStatus(uint8_t addr, uint8_t *status);
HAL_StatusTypeDef ZHDT_Motor_ReadDriverConfig(uint8_t addr, uint8_t *buf, uint16_t len);
HAL_StatusTypeDef ZHDT_Motor_ReadSystemStatus(uint8_t addr, uint8_t *buf, uint16_t len);

// 函数声明 —— 修改参数
HAL_StatusTypeDef ZHDT_Motor_WriteMicrostep(uint8_t addr, uint8_t save, uint8_t microstep); // 256→0x00
HAL_StatusTypeDef ZHDT_Motor_WriteAddr(uint8_t addr, uint8_t save, uint8_t new_addr);
HAL_StatusTypeDef ZHDT_Motor_SwitchMode(uint8_t addr, uint8_t save, uint8_t mode);
HAL_StatusTypeDef ZHDT_Motor_WriteOpenLoopCurrent(uint8_t addr, uint8_t save, uint16_t current_ma);
HAL_StatusTypeDef ZHDT_Motor_WriteDriverConfig(uint8_t addr, uint8_t save, uint8_t *config_buf);
HAL_StatusTypeDef ZHDT_Motor_WritePosPID(uint8_t addr, uint8_t save,
    uint32_t kp_trap, uint32_t kp_direct, uint32_t kp_speed, uint32_t ki_speed);
HAL_StatusTypeDef ZHDT_Motor_SaveSpeedParam(uint8_t addr, uint8_t save,
    uint8_t dir, uint16_t speed_x10, uint16_t acc_rpm_per_s, uint8_t en_ctrl);
HAL_StatusTypeDef ZHDT_Motor_SwitchSpeedScale(uint8_t addr, uint8_t save, uint8_t enable);



HAL_StatusTypeDef ZHDT_Motor_ReadDriverConfig(uint8_t addr, uint8_t *config_buf, uint16_t buf_len);
typedef struct {
    uint8_t  motor_type;            // 0.9°=50, 1.8°=25
    uint8_t  pulse_mode;            // 0=PUL_OFF, 1=PUL_OPEN, 2=PUL_FOC
    uint8_t  comm_mode;             // 0=RxTx_OFF, 1=CLOG_OUT, 2=UART_FUN, 3=CAN1_MAP
    uint8_t  en_level;              // 0=L, 1=H, 2=Hold
    uint8_t  dir_direction;         // 0=CW, 1=CCW
    uint8_t  microstep;             // 1~256, 0 表示 256
    uint8_t  microstep_interp;      // 0=Disable, 1=Enable
    uint8_t  auto_screen_off;       // 0=Disable, 1=Enable
    uint16_t open_loop_current_ma;  // 开环电流
    uint16_t closed_loop_current_ma;// 闭环最大电流
    uint16_t max_speed_rpm;         // 闭环最大转速
    uint16_t cur_bandwidth_rad_s;   // 电流环带宽
    uint32_t current_ramp_ma_per_s; // 电流爬升斜率
    uint8_t  uart_baud_index;       // 0=9600, ..., 8=921600
    uint8_t  can_baud_index;        // 0=10k, ..., 9=1M
    uint8_t  id_addr;               // 1-255
    uint8_t  checksum_type;         // 0=0x6B, 1=XOR, 2=CRC8
    uint8_t  stall_protect;         // 0=Disable, 1=Enable
    uint16_t stall_rpm;             // 堵转检测转速阈值
    uint16_t stall_current_ma;      // 堵转检测电流阈值
    uint16_t stall_time_ms;         // 堵转检测时间阈值
    uint8_t  overheat_temp_c;       // 过热保护温度阈值（℃）
    uint16_t undervolt_mV;          // 欠压阈值
    uint16_t overvolt_mV;           // 过压阈值
    uint16_t overcurrent_ma;        // 过流阈值
    uint16_t pos_arrival_window_x10;// 位置到达窗口 ×10（如 3 → 0.3°）
} ZHDT_DriverConfig_t;

#endif // ZHDT_MOTOR_H