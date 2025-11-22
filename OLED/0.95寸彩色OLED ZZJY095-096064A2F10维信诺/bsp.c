#include "bsp.h"

#include "font.h"
#include "gvar.h"

uint16_t led_pin[LED_NUMBER] = {LED_R_Pin,LED_G_Pin,LED_Y_Pin};
GPIO_TypeDef* led_port[LED_NUMBER] = {LED_R_GPIO_Port,\
	LED_G_GPIO_Port,LED_Y_GPIO_Port};

/*
函数功能：点亮LED
函数参数：LED编号x
函数返回：无
*/
extern void bspLedOn(uint16_t x){
	HAL_GPIO_WritePin(led_port[x],led_pin[x], LED_ON_LEVEL);
	
}

/*
函数功能：熄灭LED
函数参数：LED编号x
函数返回：无
*/
extern void bspLedOff(uint16_t x){
	HAL_GPIO_WritePin(led_port[x], led_pin[x], LED_OFF_LEVEL);

}

/*
函数功能：翻转LED
函数参数：LED编号x
函数返回：无
*/
extern void bspLedToggle(uint16_t x){
	HAL_GPIO_TogglePin(led_port[x], led_pin[x]);
	
}
/*
函数功能：检测按键
函数参数：无
函数返回：低八位是按键按下情况,高八位是按键松开情况
*/
uint16_t bspKeyCheck(void){
	static uint8_t key_o = 0x00;//前次状态
	uint8_t key_n = 0x00;//本次
	uint16_t i=0, key_ud;//key_ud：返回值（高8位松开，低8位按下）
	
	GPIO_TypeDef* key_port[KEY_READ_N] = {KEYS4_GPIO_Port, KEYS3_GPIO_Port, \
		KEYS2_GPIO_Port, KEYS1_GPIO_Port};
	uint16_t key_pin[KEY_READ_N] = {KEYS4_Pin, KEYS3_Pin, \
		KEYS2_Pin, KEYS1_Pin};
	for(i = 0; i < KEY_READ_N; i++){
		key_n<<=1;
		if(HAL_GPIO_ReadPin(key_port[i], key_pin[i]) == KEY_DN_L){
			key_n |= 0x01;
		}
	}
	key_ud = ((key_n ^ key_o) & key_o)<<8;
	key_ud |= (~key_o & key_n);
	key_o = key_n;
	return key_ud;
}

/*
OLED
*/

#define OLED_RES_Clr()  HAL_GPIO_WritePin(OLED_RES__GPIO_Port, OLED_RES__Pin, GPIO_PIN_RESET)
#define OLED_RES_Set()  HAL_GPIO_WritePin(OLED_RES__GPIO_Port, OLED_RES__Pin, GPIO_PIN_SET)
#define OLED_DC_Clr()   HAL_GPIO_WritePin(OLED_D_C__GPIO_Port, OLED_D_C__Pin, GPIO_PIN_RESET)
#define OLED_DC_Set()   HAL_GPIO_WritePin(OLED_D_C__GPIO_Port, OLED_D_C__Pin, GPIO_PIN_SET)
#define OLED_CS_Clr()   HAL_GPIO_WritePin(OLED_CS__GPIO_Port, OLED_CS__Pin, GPIO_PIN_RESET)
#define OLED_CS_Set()   HAL_GPIO_WritePin(OLED_CS__GPIO_Port, OLED_CS__Pin, GPIO_PIN_SET)

extern SPI_HandleTypeDef hspi2;
static sbuf_def oled_buf[OLED_HEIGHT][OLED_WIDTH];//OLED屏数据缓冲区

/*
函数功能：向OLED屏写入一个字节
函数参数：dat是写入的内容 
函数返回：无
*/
static void OLED_Writ_Bus(uint8_t dat) 
{	
  OLED_CS_Clr();
  HAL_SPI_Transmit(&hspi2, &dat, 1, 100);
  OLED_CS_Set();	
}
/*
函数功能：向OLED屏写入命令
函数参数：dat是写入的命令 
函数返回：无
*/
static void OLED_WR_REG(uint8_t dat)
{
  OLED_DC_Clr();//写命令
  OLED_Writ_Bus(dat);
  OLED_DC_Set();//写数据
}
/*
函数功能：OLED屏初始化
函数参数：无 
函数返回：无
*/
void bsp_oled_init(void)
{
//A0 60 旋转180°
//  const uint8_t oled_init_c[] = {0xAE, 0xA0, 0x72, 0xA1, 0x00, 0xA2, 0x00, 0xA4, 0xA8, \
//  0x3F, 0xAD, 0x8E, 0xB0, 0x0B, 0xB1, 0x31, 0xB3, 0xF0, 0x8A, 0x64, 0x8B, 0x78, 0x8C, \
//  0x64, 0xBB, 0x3A, 0xBE, 0x3E, 0x87, 0x06, 0x81, 0x91, 0x82, 0x50, 0x83, 0x7D, 0xAF};
  const uint8_t oled_init_c[] = {0xAE, 0xA0, 0x72, 0xA1, 0x00, 0xA2, 0x00, 0xA4, 0xA8, \
  0x3F, 0xAD, 0x8E, 0xB0, 0x0B, 0xB1, 0x31, 0xB3, 0xF0, 0x8A, 0x64, 0x8B, 0x78, 0x8C, \
  0x64, 0xBB, 0x3A, 0xBE, 0x28, 0x87, 0x0F, 0x81, 0xE0, 0x82, 0x50, 0x83, 0x7D, 0xAF};
  OLED_RES_Clr();//复位
  HAL_Delay(100);
  OLED_RES_Set();
  HAL_Delay(100);
  for(uint16_t i = 0; i < sizeof(oled_init_c); i++)
    OLED_WR_REG(oled_init_c[i]);
}
/*
函数功能：设置写入OLED屏的地址范围
函数参数：x1,x2 设置列的起始和结束地址
           y1,y2 设置行的起始和结束地址
函数返回：无
*/
static void oled_address_set(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2)
{
  OLED_WR_REG(0x15);//列地址设置
  OLED_WR_REG(x1);
  OLED_WR_REG(x2);
  OLED_WR_REG(0x75);//行地址设置
  OLED_WR_REG(y1);
  OLED_WR_REG(y2);
}
/*
函数功能：OLED屏显示
函数参数：无
函数返回：无
*/
void bsp_oled_show(void)
{
  oled_address_set(0, 0, OLED_WIDTH-1,OLED_HEIGHT-1);
  OLED_CS_Clr();
  //HAL_SPI_Transmit_DMA(&hspi2, oled_buf[0][0].sbuf1, (OLED_HEIGHT*OLED_WIDTH<<1));
  HAL_SPI_Transmit_DMA(&hspi2, (uint8_t*)oled_buf, OLED_HEIGHT * OLED_WIDTH * 2);
  //OLED_CS_Set();
}
/*
函数功能：SPI传输完成回调函数
函数参数：hspi是指向SPI接口的指针
函数返回：无
*/
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  if(hspi == &hspi2){
    OLED_CS_Set();
  }
}
/*
函数功能：OLED屏清屏
函数参数：无
函数返回：无
*/
void bsp_oled_clear(void)
{
//  for(uint16_t i = 0; i < OLED_HEIGHT; i++)
//    for(uint16_t j = 0; j < OLED_WIDTH; j++)
//      oled_buf[i][j].sbuf0 = BACKGROUND_COLOR;

  for(uint16_t i = 0; i < OLED_HEIGHT; i++)
  {
    for(uint16_t j = 0; j < OLED_WIDTH; j++)
    {
      oled_buf[i][j].sbuf0 = BACKGROUND_COLOR;  // 16位背景色
    }
  }
}
/*
函数功能：OLED屏画点
函数参数：x,y是坐标，color是点的颜色
函数返回：无
*/
extern void bsp_oled_point(uint16_t x, uint16_t y, my_color565 color)
{
  if(x < OLED_WIDTH && y < OLED_HEIGHT){
    oled_buf[y][x].sbuf0 = color;

  }
}
/*
函数功能：计算绝对值的函数
函数参数：x是要计算绝对值的数据
函数返回：计算的结果
*/
static uint16_t abs16(int16_t x)
{
  if(x < 0)
    x = -x;
  return x;
}
/*
函数功能：OLED屏画直线
函数参数：x0,y0是起点坐标，x1,y1终点坐标，color是直线的颜色
函数返回：无
*/
extern void bsp_oled_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, my_color565 color)
{
  uint16_t x, y, *px, *py, zend, k0, k1;
  int16_t t, z = 1;
	//uint32_t add;
  if(abs16(x1 - x0) > abs16(y1 - y0))
  {//如果x方向变化大
    if(x1 > x0)
    {
      x = x0;
      y = y0;
      zend = x1;
      if(y1 < y0)
        z = -1;
    }
    else
    {
      x = x1;
      y = y1;
      zend = x0;
      if(y0 < y1)//
        z = -1;
    }
    px = &x;
    py = &y;
    k0 = abs16(y1 - y0);
    k1 = abs16(x1 - x0);
  }
  else
  {//如果y方向变化大
    if(y1 > y0)//如果y1大于y0，则(x0,y0)为起点；(x1,y1)为终点
    {
      x = x0;
      y = y0;
      zend = y1;
      if(x1 < x0)//若x1<x0，则x向减小的方向变化
        z = -1;
    }
    else
    {//如果y0大于y1，则(x1,y1)为起点；(x0,y0)为终点
      x = x1;
      y = y1;
      zend = y0;
      if(x0 < x1)//若x0<x1，则x向减小的方向变化
        z = -1;
    }
    px = &y;
    py = &x;
    k0 = abs16(x1 - x0);
    k1 = abs16(y1 - y0);
  }
  t = -k1 / 2;
  while(*px <= zend)
  {
    bsp_oled_point(x, y, color);
    (*px)++;
    t += k0;
    if(t >= 0)
    {
      (*py) += z;
      t -= k1;
    }
  }
}
/*
函数功能：OLED屏画一个圆
函数参数：x0、y0是圆心点坐标
          r是半径
函数返回：无
*/
extern void bsp_oled_circle(uint16_t x0, uint16_t y0, uint16_t r, my_color565 c)
{
  uint16_t x = 0, y = r;
  uint32_t r2 = r * r, x2 = 0, y2 = r2;
  while(x <= y)
  {
    bsp_oled_point(x0 + x, y0 + y, c);
    bsp_oled_point(x0 - x, y0 + y, c);
    bsp_oled_point(x0 - x, y0 - y, c);
    bsp_oled_point(x0 + x, y0 - y, c);
    bsp_oled_point(x0 + y, y0 + x, c);
    bsp_oled_point(x0 - y, y0 + x, c);
    bsp_oled_point(x0 - y, y0 - x, c);
    bsp_oled_point(x0 + y, y0 - x, c);
    x++;
    x2 = x2 + (x << 1) - 1;
    if(x2 + y2 > r2)
    {
      y--;
      y2 = y2 - (y << 1) - 1;
    }
  }
}
/*
函数功能：OLED屏显示西文字符串
函数参数：x0、y0是起始点坐标，str是字符串（以'\0'为结束，c是字符的颜色）
函数返回：无
*/
extern void bsp_oled_ascii(uint16_t x0, uint16_t y0, uint8_t *str, my_color565 c)
{
  uint8_t k;
  for(uint16_t i = 0; str[i] != '\0'; i++)
  {
    for(uint16_t m = 0; m < 16; m++)
    {
        k = ascii[str[i] - ' '][m];
        for(uint16_t j = 0; j < 8; j++, k >>= 1)
        {
          if(k & 0x01)
            bsp_oled_point(x0 + 8 * i + j, y0 + m, c);
          else
            bsp_oled_point(x0 + 8 * i + j, y0 + m, BACKGROUND_COLOR);
        }
    }
  }
}
/*
函数功能：
函数参数：ph是指向这个汉字的指针
函数返回：该汉字在千字文中的位置,若未找到，返回65535
*/
static uint16_t hz_index(uint8_t *ph)
{
  uint8_t *p = (uint8_t*)qzw;
  for(uint16_t i = 0; i < sizeof(qzw); i += 2)
  {
    if(p[i] == ph[0] && p[i+1] == ph[1])
      return (i>>1);
  }
  return 65535;
}
/*
函数功能：OLED屏显示中文字符串
函数参数：x0、y0是起始点坐标，str是字符串（以'\0'为结束，c是字符的颜色）
函数返回：无
*/
extern void bsp_oled_hz(uint16_t x0, uint16_t y0, uint8_t *str, my_color565 c)
{
  uint8_t k;
  uint16_t index;
  for(uint16_t i = 0; str[i<<1] != '\0'; i++)
  {
    index = hz_index(&str[i<<1]);
    if(index == 65535)
      bsp_oled_ascii(x0 + 16 * i, y0, "??\0", c);
    else
    {
      for(uint16_t m = 0; m < 16; m++)
      {
        for(uint16_t n = 0; n < 2; n++)
        {
          k = t_characters[index<<1][m*2+n];
          for(uint16_t j = 0; j < 8; j++, k >>= 1)
          {
            if(k & 0x01)
              bsp_oled_point(x0 + 16 * i + 8 * n + j, y0 + m, c);
            else
              bsp_oled_point(x0 + 16 * i + 8 * n + j, y0 + m, BACKGROUND_COLOR);
          }
        }
      }
    }
  }
}
/**
 * @brief  SSD1331硬件滚动配置与启动
 * @param  mode: 滚动模式（SCROLL_HORIZONTAL/SCROLL_VERTICAL/SCROLL_DIAGONAL）
 * @param  h_offset: 水平滚动偏移（列数，0~95，仅水平/对角线模式生效）
 * @param  v_start: 垂直滚动起始行（0~63，仅垂直/对角线模式生效）
 * @param  v_rows: 垂直滚动行数（0~63，需满足v_start + v_rows ≤64）
 * @param  v_offset: 垂直滚动偏移（行数，0~63，仅垂直/对角线模式生效）
 * @param  speed: 滚动速度（SSD1331_ScrollSpeed）
 * @retval 无
 */
void ssd1331_hw_scroll_start(SSD1331_ScrollMode mode, uint8_t h_offset,
                             uint8_t v_start, uint8_t v_rows, uint8_t v_offset,
                             SSD1331_ScrollSpeed speed)
{
    OLED_WR_REG(0x2E);          
    HAL_Delay(10);

    /* 水平滚动必须全高 */
    if (mode == SCROLL_HORIZONTAL) {
        v_start = 0;
        v_rows = 64;            // 关键：不能为 0
    }

    OLED_WR_REG(0x27);
    OLED_WR_REG(h_offset);      // 水平偏移
    OLED_WR_REG(v_start);       // 起始行
    OLED_WR_REG(v_rows);        // 滚动行数
    OLED_WR_REG(v_offset);      // 垂直偏移
    OLED_WR_REG(speed);         // 速度
    OLED_WR_REG(0x00);          // 保留位

    OLED_WR_REG(0x2F);          // 启动
}
/**
 * @brief  停止硬件滚动
 * @param  无
 * @retval 无
 * 说明：停止后需重新写入显示数据，否则画面会保持滚动停止时的状态
 */
void ssd1331_hw_scroll_stop(void) {
    OLED_WR_REG(0x2E);  // 停止滚动指令
    // 重新写入RAM数据（示例：清屏后重绘，可替换为你的显示内容）
    bsp_oled_clear();
    bsp_oled_show();
}

/**
 * @brief  读取RTC时间
 * @param  time: 指向TIME结构体的指针，用于存储读取到的时间
 * @retval 无
 * 说明：读取RTC当前时间并存储到time结构体中
 */
void bspReadTime(TIME* time){
//RTC_HandleTypeDef *hrtc;
RTC_TimeTypeDef GetTime;
HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
time->h = GetTime.Hours;
time->m = GetTime.Minutes;
time->s = GetTime.Seconds;

printf("%02d:%02d:%02d\r\n",GetTime.Hours, GetTime.Minutes, GetTime.Seconds);

RTC_DateTypeDef GetData;
HAL_RTC_GetDate(&hrtc, &GetData, RTC_FORMAT_BIN);
printf("%02d/%02d/%02d\r\n",2000 + GetData.Year, GetData.Month, GetData.Date);

}
/**
 * @brief  写入RTC时间
 * @param  time: 指向TIME结构体的指针，包含要写入的时间
 * @retval 无
 * 说明：将time结构体中的时间写入RTC
 */
void bspWriteTime(TIME* time){
  RTC_TimeTypeDef SetTime;
  SetTime.Hours = time->h;
  SetTime.Minutes = time->m;
  SetTime.Seconds = time->s;
  HAL_RTC_SetTime(&hrtc, &SetTime, RTC_FORMAT_BIN);
}
/* app.h ->
函数功能：按键处理
函数参数：低八位是按键按下情况,高八位是按键松开情况
函数返回：无

void keyDisp(uint16_t key_updn){


}
*/

