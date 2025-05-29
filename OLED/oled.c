#include "oled.h"
#include "oled_font.h"

void OLED_I2C_Init(void) {
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_SET);
}

void OLED_I2C_Start(void) {
    OLED_W_SDA(1);
    OLED_W_SCL(1);
    OLED_W_SDA(0);
    OLED_W_SCL(0);
}

void OLED_I2C_Stop(void) {
    OLED_W_SDA(0);
    OLED_W_SCL(1);
    OLED_W_SDA(1);
}

void OLED_I2C_SendByte(uint8_t Byte) {
    uint8_t i;
    for (i = 0; i < 8; i++) {
        OLED_W_SDA(Byte & (0x80 >> i));
        OLED_W_SCL(1);
        OLED_W_SCL(0);
    }
    OLED_W_SCL(1);
    OLED_W_SCL(0);
}

void OLED_WriteCommand(uint8_t Command) {
    OLED_I2C_Start();
    OLED_I2C_SendByte(0x78);  // I2C�ӻ���ַ��0x3C<<1��
    OLED_I2C_SendByte(0x00);  // д����
    OLED_I2C_SendByte(Command);
    OLED_I2C_Stop();
}

void OLED_WriteData(uint8_t Data) {
    OLED_I2C_Start();
    OLED_I2C_SendByte(0x78);
    OLED_I2C_SendByte(0x40);  // д����
    OLED_I2C_SendByte(Data);
    OLED_I2C_Stop();
}

void OLED_SetCursor(uint8_t Y, uint8_t X) {
    OLED_WriteCommand(0xB0 | Y);
    OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));
    OLED_WriteCommand(0x00 | (X & 0x0F));
}

void OLED_Clear(void) {
    uint8_t i, j;
    for (j = 0; j < 8; j++) {
        OLED_SetCursor(j, 0);
        for (i = 0; i < 128; i++) {
            OLED_WriteData(0x00);
        }
    }
}

void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char) {
    OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);
    for (uint8_t i = 0; i < 8; i++) {
        OLED_WriteData(OLED_F8x16[Char - ' '][i]);
    }
    OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);
    for (uint8_t i = 0; i < 8; i++) {
        OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);
    }
}

void OLED_ShowString(uint8_t Line, uint8_t Column, char *String) {
    uint8_t i;
    for (i = 0; String[i] != '\0'; i++) {
        OLED_ShowChar(Line, Column + i, String[i]);
    }
}

void OLED_Init(void) {
    OLED_I2C_Init();
    // OLED��ʼ���������Ӳ���ֲ�������˴�Ϊͨ�����ã�
    OLED_WriteCommand(0xAE);  // �ر���ʾ
    OLED_WriteCommand(0xD5);  // ʱ�ӷ�Ƶ
    OLED_WriteCommand(0x80);
    OLED_WriteCommand(0xA8);  // ��·����
    OLED_WriteCommand(0x3F);
    OLED_WriteCommand(0xD3);  // ��ʾƫ��
    OLED_WriteCommand(0x00);
    OLED_WriteCommand(0x40);  // ��ʼ��
    OLED_WriteCommand(0xA1);  // ���ҷ���
    OLED_WriteCommand(0xC8);  // ���·���
    OLED_WriteCommand(0xDA);  // COM��������
    OLED_WriteCommand(0x12);
    OLED_WriteCommand(0x81);  // �Աȶ�
    OLED_WriteCommand(0xCF);
    OLED_WriteCommand(0xD9);  // Ԥ�������
    OLED_WriteCommand(0xF1);
    OLED_WriteCommand(0xDB);  // VCOMH
    OLED_WriteCommand(0x30);
    OLED_WriteCommand(0xA4);  // ȫ����ʾ
    OLED_WriteCommand(0xA6);  // ������ʾ
    OLED_WriteCommand(0x8D);  // ����
    OLED_WriteCommand(0x14);
    OLED_WriteCommand(0xAF);  // ������ʾ
    OLED_Clear();  // ����
}