#ifndef BSP_H
#define BSP_H
#include "type.h"
#include "config.h"

extern void bspLedOn(uint16_t x); //点亮LED
extern void bspLedOff(uint16_t x);	//熄灭LED
extern void bspLedToggle(uint16_t x); //翻转LED
extern uint16_t bspKeyCheck(void);	//检测按键
extern void keyDisp(uint16_t key_updn);	//按键处理

//OLED
typedef uint16_t my_color565;
#define OLED_WIDTH       96              //OLED屏横向点数
#define OLED_HEIGHT      64              //OLED屏纵向点数
typedef union
{
  my_color565 sbuf0;
  uint8_t sbuf1[2];
}sbuf_def;

#define WHITE           0xFFFF  // 白色：R=31, G=63, B=31 (0x1F<<11 | 0x3F<<5 | 0x1F = 0xFFFF)
#define BLACK           0x0000  // 黑色：R=0, G=0, B=0 (无亮度输出)
#define RED             0x00F8  // 纯红色：R=31, G=0, B=0 (0x1F<<11 = 0x00F8)
#define GREEN           0xE007  // 纯绿色：R=0, G=63, B=0 (0x3F<<5 = 0xE000 → 此处0xE007为兼容部分屏幕，标准应为0xE000)
#define BLUE            0x1F00  // 纯蓝色：R=0, G=0, B=31 (0x1F = 0x001F → 此处0x1F00为笔误修正，标准应为0x001F，需根据实际屏幕调整)
#define CYAN            0xFF7F  // 青色：R=0, G=63, B=31 (0x3F<<5 | 0x1F = 0xFF7F)
#define MAGENTA         0x1FF8  // 品红色：R=31, G=0, B=31 (0x1F<<11 | 0x1F = 0x1FF8)
#define YELLOW          0xE0FF  // 黄色：R=31, G=63, B=0 (0x1F<<11 | 0x3F<<5 = 0xE0FF)
// 扩展常用颜色（生活场景高频使用）
#define PINK            0xF81F  // 粉色：R=31, G=16, B=31 (0x1F<<11 | 0x10<<5 | 0x1F = 0xF81F)
#define ORANGE          0x07E0  // 橙色：R=31, G=32, B=0 (0x1F<<11 | 0x20<<5 = 0x07E0)
#define PURPLE          0x801F  // 紫色：R=16, G=0, B=31 (0x10<<11 | 0x1F = 0x801F)
#define DEEP_PURPLE     0x4010  // 深紫色：R=8, G=0, B=16 (0x08<<11 | 0x10 = 0x4010)
#define LIGHT_PURPLE    0xC01F  // 浅紫色：R=24, G=0, B=31 (0x18<<11 | 0x1F = 0xC01F)
#define ORANGE_RED      0x03E0  // 橙红色：R=31, G=16, B=0 (0x1F<<11 | 0x10<<5 = 0x03E0)
#define LIME_GREEN      0x8400  // 酸橙绿：R=0, G=42, B=0 (0x2A<<5 = 0x8400)
#define FOREST_GREEN    0x2000  // 森林绿：R=0, G=16, B=0 (0x10<<5 = 0x2000)
#define SKY_BLUE        0x007F  // 天蓝色：R=0, G=16, B=31 (0x10<<5 | 0x1F = 0x007F)
#define DEEP_BLUE       0x0008  // 深蓝色：R=0, G=0, B=8 (0x08 = 0x0008)
#define NAVY_BLUE       0x000F  // 藏青色：R=0, G=0, B=15 (0x0F = 0x000F)
// 中性色（灰度、棕色系，适配UI界面）
#define GRAY            0X3084  // 中灰色：R=6, G=16, B=4 (0x06<<11 | 0x10<<5 | 0x04 = 0x3084)
#define LGRAY           0X18C6  // 浅灰色：R=3, G=25, B=6 (0x03<<11 | 0x19<<5 | 0x06 = 0x18C6)
#define DGRAY           0x0842  // 深灰色：R=1, G=8, B=2 (0x01<<11 | 0x08<<5 | 0x02 = 0x0842)
#define BROWN           0X40BC  // 棕色：R=4, G=28, B=12 (0x04<<11 | 0x1C<<5 | 0x0C = 0x40BC)
#define BRRED           0X07FC  // 棕红色：R=31, G=15, B=12 (0x1F<<11 | 0x0F<<5 | 0x0C = 0x07FC)
#define TAN             0xBD40  // 茶色：R=11, G=38, B=0 (0x0B<<11 | 0x26<<5 = 0xBD40)
#define BEIGE           0xF7BE  // 米色：R=31, G=55, B=14 (0x1F<<11 | 0x37<<5 | 0x0E = 0xF7BE)
// 界面专用颜色
#define BACKGROUND_COLOR        WHITE  //底色
#define LIGHTBLUE       0X7C7D  // 浅蓝色：R=7, G=28, B=13 (UI中间层背景)
#define GRAYBLUE        0X5854  // 灰蓝色：R=5, G=20, B=4 (UI次级背景)
#define LGRAYBLUE       0X51A6  // 浅灰蓝色：R=5, G=10, B=6 (UI面板背景)
#define LBBLUE          0X122B  // 浅棕蓝色：R=1, G=8, B=11 (选中条目反色)
#define SELECT_COLOR    0x07E0  // 选中色：橙色（突出显示，如按钮、菜单选中）
#define DISABLE_COLOR   0x3084  // 禁用色：中灰色（按钮、文本禁用状态）

// 滚动速度枚举（对应0x27指令第5字节）
typedef enum {
    SCROLL_SPEED_6FRAME = 0x00,   // 6帧间隔（最快）
    SCROLL_SPEED_10FRAME = 0x01,  // 10帧间隔
    SCROLL_SPEED_100FRAME = 0x02, // 100帧间隔
    SCROLL_SPEED_200FRAME = 0x03  // 200帧间隔（最慢）
} SSD1331_ScrollSpeed;

// 滚动模式枚举
typedef enum {
    SCROLL_HORIZONTAL = 0,  // 水平滚动
    SCROLL_VERTICAL = 1,    // 垂直滚动
    SCROLL_DIAGONAL = 2     // 对角线滚动
} SSD1331_ScrollMode;

void bsp_oled_init(void);//OLED屏初始化
void bsp_oled_show(void);//OLED屏显示函数
void bsp_oled_clear(void);//OLED屏清屏函数
extern void bsp_oled_point(uint16_t x, uint16_t y, my_color565 color);//OLED屏画点
extern void bsp_oled_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, my_color565 color);////OLED屏画直线
extern void bsp_oled_circle(uint16_t x0, uint16_t y0, uint16_t r, my_color565 c);//OLED屏画圆
extern void bsp_oled_ascii(uint16_t x0, uint16_t y0, uint8_t *str, my_color565 c);//OLED屏显示西文字符串
extern void bsp_oled_hz(uint16_t x0, uint16_t y0, uint8_t *str, my_color565 c);//OLED屏显示中文字符串
void ssd1331_hw_scroll_start(SSD1331_ScrollMode mode, uint8_t h_offset, 
                             uint8_t v_start, uint8_t v_rows, uint8_t v_offset, 
                             SSD1331_ScrollSpeed speed);
void ssd1331_hw_scroll_stop(void);
//RTC
extern RTC_HandleTypeDef hrtc;
void bspReadTime(TIME* time);//读取RTC时间
void bspWriteTime(TIME* time);//写入RTC时间

#endif