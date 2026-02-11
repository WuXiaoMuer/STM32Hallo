#include "QD4310.h"
#include "can.h"

QD4310_t Motor_0 = {.id = 0, .hcan = &hcan1}; // 初始化电机结构体，使用CAN1外设，ID为0

void CAN_InterfaceInit(void) {
    CAN_FilterTypeDef sFilterConfig;
    sFilterConfig.FilterBank = 0;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.SlaveStartFilterBank = 14;
    if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK) Error_Handler();
    if (HAL_CAN_Start(&hcan1) != HAL_OK) Error_Handler();
    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) Error_Handler();
}

void main() {
    MX_CAN1_Init();

    CAN_InterfaceInit();             // 初始化CAN接口
    QD4310_Enable(&Motor_0);         // 使能电机
    QD4310_SetSpeed(&Motor_0, 0.0f); // 设置电机转速为0rpm

    for (int i = 0; i <= 100; i++) {
        QD4310_SetSpeed(&Motor_0, (float)i);
        HAL_Delay(50); // 50ms间隔 × 100次 = 5秒
    }

    for (int i = 100; i >= 0; i--) {
        QD4310_SetSpeed(&Motor_0, (float)i);
        HAL_Delay(50);
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    if (hcan == &hcan1) {
        CAN_RxHeaderTypeDef rx_header;
        uint8_t rx_data[8];
        HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data);
        if (rx_header.StdId >= 0x500 && rx_header.StdId <= 0x508) {
            if ((rx_header.StdId & 0xFF) == Motor_0.id) {
                QD4310_Update(&Motor_0, rx_data);
            }
        }
    }
}
