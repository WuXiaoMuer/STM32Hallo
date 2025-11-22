#include "bsp_touch.h"
extern I2C_HandleTypeDef hi2c2;

#define TOUCH_RST_SET() (TOUCH_RST_GPIO_Port->BSRR |= TOUCH_RST_Pin)
#define TOUCH_RST_RESET() (TOUCH_RST_GPIO_Port->BSRR |= (uint32_t)TOUCH_RST_Pin << 16U)


static __IO uint8_t touchChipId = 0;//触摸芯片ID
static __IO uint8_t touchVer = 0;//触摸芯片版本
/**
 * @brief  触摸屏复位
 * @param  无
 */
static void bsp_touchReset(void){
    TOUCH_RST_RESET();
    HAL_Delay(10);
    TOUCH_RST_SET();
    HAL_Delay(10);
}
/**
 * @brief  读取触摸屏ID
 * @param  无
 * @retval 触摸芯片ID
 */
static uint8_t bsp_touchReadId(void){
    uint8_t id=0;
    HAL_I2C_Mem_Read(&TOUCH_PORT, CST816_ADDRESS, ChipID, I2C_MEMADD_SIZE_8BIT, &id, 1, HAL_MAX_DELAY);
    return id;
}
/**
 * @brief  读取触摸屏版本
 * @param  无
 * @retval 触摸芯片版本
 */
static uint8_t bsp_touchReadVer(void){
    uint8_t ver=0;
    HAL_I2C_Mem_Read(&TOUCH_PORT, CST816_ADDRESS, VersionID, I2C_MEMADD_SIZE_8BIT, &ver, 1, HAL_MAX_DELAY);
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
    uint8_t data[4];
    HAL_I2C_Mem_Read(&TOUCH_PORT, CST816_ADDRESS, XposH, I2C_MEMADD_SIZE_8BIT, data, 4, HAL_MAX_DELAY);
    point->x = ((data[0] & 0x0F) << 8) | data[1];
    point->y = ((data[2] & 0x0F) << 8) | data[3];
}

