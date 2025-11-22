/*
电机使能 + 以 1000 RPM 旋转

void Example_SpeedMode(void)
{
    ZHDT_Motor_Enable(1, 1, SYNC_DISABLE);
    HAL_Delay(100);
    ZHDT_Motor_SpeedMode(1, DIR_CW, 2000, 10000, SYNC_DISABLE); // 2000 RPM/s 加速，1000.0 RPM
}

梯形曲线旋转 360°

void Example_TrapezoidalRotation(void)
{
    ZHDT_Motor_Enable(1, 1, SYNC_DISABLE);
    HAL_Delay(100);
    // 加速 1000 RPM/s，减速 1000 RPM/s，最大速度 2000.0 RPM，转 360.0°（=3600×10）
    ZHDT_Motor_TrapezoidalPosMode(1, DIR_CW, 1000, 1000, 20000, 3600, POS_RELATIVE, SYNC_DISABLE);
}

同步控制两台电机

void Example_SyncControl(void)
{
    // 电机1：转 -360.0°（相对）
    ZHDT_Motor_DirectPosMode(1, DIR_CCW, -3600, POS_RELATIVE, SYNC_ENABLE);
    // 电机2：转 +720.0°（相对）
    ZHDT_Motor_LimitedSpeedPosMode(2, DIR_CW, 15000, 7200, POS_RELATIVE, SYNC_ENABLE);
    
    // 同时启动
    ZHDT_Motor_SyncStart(0); // 广播地址
}

清零位置 + 读取实时位置

void Example_SetZeroAndRead(void)
{
    ZHDT_Motor_SetCurrentPosAsZero(1);
    HAL_Delay(100);

    int32_t pos;
    ZHDT_Motor_ReadRealTimePos(1, &pos);
    float deg = pos / 10.0f; // 单位：度
    printf("Current position: %.1f deg\n", deg);
}

读取温度和电流

void Example_ReadStatus(void)
{
    int16_t temp;
    uint16_t current;
    ZHDT_Motor_ReadTemperature(1, &temp);
    ZHDT_Motor_ReadPhaseCurrent(1, &current);
    printf("Temp: %d°C, Current: %d mA\n", temp, current);
}

绝对角度定位（单圈）

void Demo_AbsolutePos_0_to_90_deg(void)
{
    ZHDT_Motor_Enable(1, 1, SYNC_DISABLE);
    HAL_Delay(50);

    // 设置当前位置为 0°
    ZHDT_Motor_SetCurrentPosAsZero(1);

    // 绝对运动到 90.0°
    ZHDT_Motor_DirectPosMode(1, DIR_CW, 900, POS_ABSOLUTE, SYNC_DISABLE);

    // 等待到位（轮询）
    uint8_t status;
    uint32_t timeout = 0;
    do {
        HAL_Delay(10);
        ZHDT_Motor_ReadMotorStatus(1, &status);
        timeout++;
    } while ((status & 0x02) == 0 && timeout < 500); // bit1 = 到位标志

    if (timeout < 500) {
        printf("Reached 90.0°!\n");
    } else {
        printf("Timeout or not arrived!\n");
        ZHDT_Motor_StopImmediately(1, SYNC_DISABLE);
    }
}

多圈相对旋转（如 3 圈 → 1080°）

void Demo_Relative_3_Turns(void)
{
    ZHDT_Motor_Enable(1, 1, SYNC_DISABLE);
    HAL_Delay(50);

    // 相对当前位置，顺时针旋转 3 圈（1080.0°）
    ZHDT_Motor_TrapezoidalPosMode(
        1,
        DIR_CW,
        2000,   // 加速 2000 RPM/s
        2000,   // 减速 2000 RPM/s
        30000,  // 最大速度 3000.0 RPM
        10800,  // 1080.0° ×10
        POS_RELATIVE,
        SYNC_DISABLE
    );

    // 延迟等待（或轮询到位）
    HAL_Delay(3000); // 足够时间完成

    // 立即刹车（确保停稳）
    ZHDT_Motor_StopImmediately(1, SYNC_DISABLE);
}

往复运动（0° ? 180°）5 次
void Demo_Oscillate_0_to_180_5_Times(void)
{
    ZHDT_Motor_Enable(1, 1, SYNC_DISABLE);
    HAL_Delay(100);
    ZHDT_Motor_SetCurrentPosAsZero(1);

    for (int i = 0; i < 5; i++) {
        // 0 → 180.0°
        ZHDT_Motor_LimitedSpeedPosMode(1, DIR_CW, 15000, 1800, POS_ABSOLUTE, SYNC_DISABLE);
        HAL_Delay(1500);

        // 180 → 0°
        ZHDT_Motor_LimitedSpeedPosMode(1, DIR_CCW, 15000, 0, POS_ABSOLUTE, SYNC_DISABLE);
        HAL_Delay(1500);
    }

    ZHDT_Motor_StopImmediately(1, SYNC_DISABLE);
}

高精度角度校准测试（验证闭环精度）

void Demo_Accuracy_Test(void)
{
    ZHDT_Motor_Enable(1, 1, SYNC_DISABLE);
    HAL_Delay(100);
    ZHDT_Motor_SetCurrentPosAsZero(1);

    // 目标：0.1° 步进，共 3600 步
    for (int i = 0; i <= 3600; i++) {
        int32_t target = i; // i * 0.1°
        ZHDT_Motor_DirectPosMode(1, DIR_CW, target, POS_ABSOLUTE, SYNC_DISABLE);
        HAL_Delay(50); // 等待稳定

        // 读取实际位置
        int32_t real_pos;
        ZHDT_Motor_ReadRealTimePos(1, &real_pos);
        float real_deg = real_pos / 10.0f;

        // 读取误差（×100，所以除以 100）
        int32_t err_x100;
        ZHDT_Motor_ReadPosError(1, &err_x100);
        float err_deg = err_x100 / 100.0f;

        printf("Target: %.1f°, Real: %.2f°, Error: %.3f°\n", target/10.0f, real_deg, err_deg);
    }

    ZHDT_Motor_StopImmediately(1, SYNC_DISABLE);
}

多电机同步角度运动（2 轴联动）

void Demo_Sync_Two_Motors(void)
{
    // 电机1：从当前位置 +720°（2圈）
    ZHDT_Motor_TrapezoidalPosMode(1, DIR_CW, 1000, 1000, 20000, 7200, POS_RELATIVE, SYNC_ENABLE);

    // 电机2：从当前位置 -360°（1圈）
    ZHDT_Motor_TrapezoidalPosMode(2, DIR_CCW, 1000, 1000, 20000, -3600, POS_RELATIVE, SYNC_ENABLE);

    // 同时启动！
    ZHDT_Motor_SyncStart(0); // 广播地址 0

    HAL_Delay(3000); // 运动时间

    // 同步停止
    ZHDT_Motor_StopImmediately(0, SYNC_DISABLE); // 广播停止
}

安全角度限制（防超程）

void Demo_Safe_Angle_Limit(int32_t target_deg_x10)
{
    // 假设机械限位：-900 ~ +900（-90° ~ +90°）
    if (target_deg_x10 < -900 || target_deg_x10 > 900) {
        printf("Error: Target %.1f° exceeds mechanical limit!\n", target_deg_x10 / 10.0f);
        return;
    }

    ZHDT_Motor_Enable(1, 1, SYNC_DISABLE);
    HAL_Delay(50);

    ZHDT_Motor_DirectPosMode(1, DIR_CW, target_deg_x10, POS_ABSOLUTE, SYNC_DISABLE);
    HAL_Delay(1000);
    ZHDT_Motor_StopImmediately(1, SYNC_DISABLE);
}


堵转恢复流程（带保护）

void Demo_Stall_Recovery(void)
{
    ZHDT_Motor_Enable(1, 1, SYNC_DISABLE);
    HAL_Delay(100);

    // 故意设置一个会被堵转的指令（如强行转到障碍物）
    ZHDT_Motor_TorqueMode(1, DIR_CW, 10000, 3500, SYNC_DISABLE); // 高电流，可能堵转

    HAL_Delay(2000);

    // 检查状态
    uint8_t status;
    ZHDT_Motor_ReadMotorStatus(1, &status);
    if (status & 0x08) { // bit3 = 堵转保护触发
        printf("Stall protection triggered!\n");
        ZHDT_Motor_ResetStallProtect(1); // 解除保护
        ZHDT_Motor_StopImmediately(1, SYNC_DISABLE);
        ZHDT_Motor_Enable(1, 0, SYNC_DISABLE); // 失能
    }
}


*/