#include "bsp_touch.h"
extern I2C_HandleTypeDef hi2c2;

#define TOUCH_RST_SET()        (TOUCH_RST_GPIO_Port->BSRR  = TOUCH_RST_Pin)
#define TOUCH_RST_RESET()      (TOUCH_RST_GPIO_Port->BSRR  = (uint32_t)TOUCH_RST_Pin << 16U)
#define CST816_I2C_TIMEOUT     100

static __IO uint8_t touchChipId = 0;//触摸芯片ID
static __IO uint8_t touchVer = 0;//触摸芯片版本

static HAL_StatusTypeDef touch_read(uint8_t reg, uint8_t *buf, uint16_t len)
{
    return HAL_I2C_Mem_Read(&TOUCH_PORT, CST816_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, buf, len, CST816_I2C_TIMEOUT);
}

static HAL_StatusTypeDef touch_write(uint8_t reg, uint8_t value)
{
    return HAL_I2C_Mem_Write(&TOUCH_PORT, CST816_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, CST816_I2C_TIMEOUT);
}
/**
 * @brief  触摸屏复位
 * @param  无
 */
static void bsp_touchReset(void){
    TOUCH_RST_RESET();
    HAL_Delay(20);
    TOUCH_RST_SET();
    HAL_Delay(50);
}
/**
 * @brief  读取触摸屏ID
 * @param  无
 * @retval 触摸芯片ID
 */
static uint8_t bsp_touchReadId(void){
    uint8_t id=0;
    touch_read(ChipID, &id, 1);
    return id;
}
/**
 * @brief  读取触摸屏版本
 * @param  无
 * @retval 触摸芯片版本
 */
static uint8_t bsp_touchReadVer(void){
    uint8_t ver=0;
    touch_read(VersionID, &ver, 1);
    return ver;
}
/**
 * @brief  触摸屏初始化
 * @param  无
 */
extern void bsp_touch_init(void){
    bsp_touchReset();
    touchChipId=bsp_touchReadId();
    touchVer=bsp_touchReadVer();

    if(touchChipId == 0x00 || touchChipId == 0xFF){
        return;
    }

    /* 基本配置：关闭自动睡眠、配置中断输出等 */
    touch_write(AutoSleepTime, 0x00);   // 禁止自动休眠
    touch_write(DisAutoSleep, 0x01);    // 立即退出休眠
    touch_write(IrqCtl, 0x41);          // INT低有效、推挽输出
    touch_write(IrqPluseWidth, 0x0F);   // 延长脉宽，便于MCU采样
}
/**
 * @brief  触摸屏中断回调函数
 * @param  GPIO_Pin 中断引脚
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == TOUCH_INT_Pin)
    {
        touch_downFlag = TRUE;
    }
    
}

/**
 * @brief  获取触摸坐标
 * @param  point 存储触摸坐标的指针
 */
void bsp_touch_getXY(Point *point){
     uint8_t data[6] = {0};
     if (touch_read(GestureID, data, sizeof(data)) != HAL_OK) {
         return; // I2C 异常，可在此处尝试复位
     }

     uint8_t finger = data[1] & 0x0F;
     if (finger == 0) {
         return; // 没有触摸（本次中断通常是离开事件）
     }

     uint16_t x = ((data[2] & 0x0F) << 8) | data[3];
     uint16_t y = ((data[4] & 0x0F) << 8) | data[5];

     if(x >= LCD_WIDTH)  x = LCD_WIDTH  - 1;
     if(y >= LCD_HEIGHT) y = LCD_HEIGHT - 1;

     point->x = x;
     point->y = y;
}

