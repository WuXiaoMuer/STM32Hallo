#include "ds18b20.h"
/* 72 MHz 下的微秒忙等待 */
static void DWT_Delay_us(uint32_t us)
{
    uint32_t clk = SystemCoreClock / 1000000UL;          /* 72 */
    uint32_t cnt = us * clk;
    uint32_t start = DWT->CYCCNT;
    while ((DWT->CYCCNT - start) < cnt) {};
}

/* 把引脚设为输出开漏 */
static void DQ_OUT(void)
{
    GPIO_InitTypeDef g = {0};
    g.Pin = DS18B20_PIN;
    g.Mode = GPIO_MODE_OUTPUT_OD;
    g.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DS18B20_PORT, &g);
}

/* 把引脚设为输入，上拉 */
static void DQ_IN(void)
{
    GPIO_InitTypeDef g = {0};
    g.Pin = DS18B20_PIN;
    g.Mode = GPIO_MODE_INPUT;
    g.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DS18B20_PORT, &g);
}

/* 产生复位脉冲，返回 0=设备存在 */
static uint8_t DS18B20_Reset(void)
{
    uint8_t pulse = 0;
    DQ_OUT();
    HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
    DWT_Delay_us(480);          /* 拉低 ≥480 µs */
    HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_SET);
    DQ_IN();
    DWT_Delay_us(70);           /* 等待 15-60 µs 存在脉冲 */
    if (HAL_GPIO_ReadPin(DS18B20_PORT, DS18B20_PIN) == GPIO_PIN_RESET)
        pulse = 1;
    DWT_Delay_us(410);
    return pulse;
}

/* 写 1 字节 */
static void DS18B20_WriteByte(uint8_t dat)
{
    DQ_OUT();
    for (uint8_t i = 0; i < 8; i++)
    {
        HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
        DWT_Delay_us(2);
        if (dat & 0x01)
            HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_SET);
        DWT_Delay_us(60);
        HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_SET);
        DWT_Delay_us(2);
        dat >>= 1;
    }
}

/* 读 1 字节 */
static uint8_t DS18B20_ReadByte(void)
{
    uint8_t dat = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        DQ_OUT();
        HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET);
        DWT_Delay_us(2);
        HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_SET);
        DQ_IN();
        DWT_Delay_us(12);
        if (HAL_GPIO_ReadPin(DS18B20_PORT, DS18B20_PIN) == GPIO_PIN_SET)
            dat |= (1 << i);
        DWT_Delay_us(50);
    }
    return dat;
}

/* 初始化 GPIO 并启用 DWT */
void DS18B20_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /* 启用 DWT 计数器（用于微秒延时） */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

/* 完整读取温度，返回 °C，失败返回 -999 */
float DS18B20_ReadTemp(void)
{
    int16_t temp;
    if (!DS18B20_Reset()) return -999.0f;

    DS18B20_WriteByte(0xCC);   /* Skip ROM */
    DS18B20_WriteByte(0x44);   /* Convert T */
    HAL_Delay(750);            /* 12-bit 最长 750 ms */

    if (!DS18B20_Reset()) return -999.0f;
    DS18B20_WriteByte(0xCC);
    DS18B20_WriteByte(0xBE);   /* Read scratchpad */

    uint8_t lo = DS18B20_ReadByte();
    uint8_t hi = DS18B20_ReadByte();
    temp = (int16_t)((hi << 8) | lo);
    return temp * 0.0625f;
}

/*-------------  1-Wire CRC8  -------------*/
static uint8_t _crc8(uint8_t crc, uint8_t data)
{
    /* 多项式 0x31 (x^8 + x^5 + x^4 + 1) */
    static const uint8_t table[16] = {
        0x00,0x5E,0xBC,0xE2,0x61,0x3F,0xDD,0x83,
        0xC2,0x9C,0x7E,0x20,0xA3,0xFD,0x1F,0x41
    };
    crc ^= data;
    for (int i = 0; i < 8; i++)
        crc = (crc & 0x80) ? (crc << 1) ^ 0x31 : crc << 1;
    return crc;
}
int8_t DS18B20_Diag(void)          /* 返回 0=OK，-1=无存在脉冲，-2=CRC 错 */
{
    if (!DS18B20_Reset()) return -1;
    DS18B20_WriteByte(0xCC);
    DS18B20_WriteByte(0x44);       /* 启动转换 */
    HAL_Delay(750);
    if (!DS18B20_Reset()) return -1;
    DS18B20_WriteByte(0xCC);
    DS18B20_WriteByte(0xBE);       /* 读暂存器 */
    uint8_t buf[9];
    for (int i=0;i<9;i++) buf[i]=DS18B20_ReadByte();
    uint8_t crc = 0;
    for (int i = 0; i < 8; i++) crc = _crc8(crc, buf[i]);
    if(crc!=buf[8]) return -2;
    return 0;
}
