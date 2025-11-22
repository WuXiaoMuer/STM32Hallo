#include "bsp_lcd.h"
#include "font.h"

__attribute__((section(".RAM_D2"))) uint16_t lcd_buf[LCD_WIDTH * LCD_HEIGHT];


static void bsp_lcd_cs_valid(void); // CS引脚有效
static void bsp_lcd_cs_invalid(void); // CS引脚无效
static void bsp_lcd_rst_valid(void); // RST引脚有效 复位
static void bsp_lcd_rst_invalid(void); // RST引脚无效 释放
static void bsp_lcd_dc_data(void); // DC引脚数据
static void bsp_lcd_dc_comm(void); // DC引脚命令

extern void bsp_lcd_bl_on(void); // 背光使能
extern void bsp_lcd_bl_off(void); // 背光失能

static void lcd_reset(void); // 液晶屏复位
static void lcd_sendCommand(uint8_t command); // 发送命令
static void lcd_sendData(uint8_t data); // 发送数据
static void lcd_init_reg(void); // 液晶屏初始化寄存器
static void lcd_module_init(void); // 液晶屏模块初始化
static void lcd_setWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2); // 设置窗口
static void lcd_resolu_scan(void); // 设置液晶屏分辨率扫描
void lcd_buf_writePoint(Point* point, rgb565 color); // 写入显存点

extern void lcd_init(void); // 初始化液晶屏
extern void lcd_writePixel(rgb565 color); // 写入像素点
extern void lcd_line(Point* point1, Point* point2, rgb565 color); // 绘制水平线
extern void lcd_circle(Point* point, uint16_t radius, rgb565 color); // 绘制圆
extern void lcd_ascii(Point* point, char* c, uint16_t font_size, rgb565 color); // 绘制字符
extern void lcd_uint(Point* point, uint32_t uint, uint16_t font_size, rgb565 color); // 绘制无符号整数
extern void lcd_hex(Point* point, uint16_t uint, uint16_t font_size, rgb565 color); // 绘制16进制整数
extern void lcd_float(Point* point, float flt, uint16_t font_size, rgb565 color); // 绘制浮点数
float abs(float f) {return f<0?-f:f;}

/**
 * @brief  绘制浮点数 用于绘制浮点数flt,浮点数位置为point,浮点数颜色为color
 * @param  point 浮点数的位置坐标
 * @param  flt 要绘制的浮点数
 */
extern void lcd_float(Point* point, float flt, uint16_t font_size, rgb565 color){
    char str[8]={'?','?','?','.','?','?','?','\0'};
    str[0]  = (flt < 0) ? '-' : ' ';
    //数值过大
    if(abs(flt) >= 100.0f){
        lcd_ascii(point, str, font_size, color);
        return;
    }
    //整数
    int16_t int_part = (int16_t)abs(flt);
    int16_t float_part = (int16_t)(abs(flt - int_part) * 1000 + 0.5f);
    str[2] = (int_part % 10) + '0';
    int_part /= 10;
    str[1] = (int_part == 0) ? ' ' : (int_part % 10) + '0';
    //小数
    for(int i=0; i < 3; i++){
        str[6 - i] = (float_part % 10) + '0';
        float_part /= 10;
    }
    lcd_ascii(point, str, font_size, color);
    return;
}

/**
 * @brief  绘制16进制整数 用于绘制16进制整数uint,整数位置为point,整数颜色为color
 * @param  point 整数的位置坐标
 * @param  uint 要绘制的16进制整数
 * @param  font_size 整数的字体大小
 * @param  color 整数的颜色
 * @retval 无
 */
extern void lcd_hex(Point* point,uint16_t uint, uint16_t font_size, rgb565 color){
    char str[7]={'0','x',};
    char hex_tbl[] = "0123456789ABCDEF";
    for(uint8_t i = 0; i < 4; i++){
        str[5 - i] = hex_tbl[uint & 0x0F];
        uint >>= 4;
    }
    str[6] = '\0';
    lcd_ascii(point, str, font_size, color);
}
/**
 * @brief  查找中文字符在千字文中的位置
 * @param  ph 指向这个汉字的指针
 * @retval 该汉字在千字文中的位置,若未找到，返回65535
 */
static uint16_t hz_index(uint8_t *ph){
    uint8_t *p = (uint8_t*)qzw;
    for(uint16_t i = 0; i < sizeof(qzw); i += 2)
    {
    if(p[i] == ph[0] && p[i+1] == ph[1])
        return (i>>1);
    }
    return 65535;
}
/**
 * @brief  OLED屏显示中文字符串
 * @param  point 字符串的起始点坐标
 * @param  str 要显示的中文字符串（以'\0'为结束）
 * @param  font_size 中文字符的字体大小
 * @param  color 中文字符的颜色
 * @retval 无
 */
extern void lcd_chinese(Point* p, char* pc, uint16_t font_size, rgb565 color){
    uint8_t k;
    uint16_t index;
    Point xy = *p;
    for(uint16_t i = 0; pc[i<<1] != '\0'; i++)
    {
    index = hz_index((uint8_t*)&pc[i<<1]);
    if(index == 65535){
        xy.x=p->x+16*i;
        xy.y=p->y;
        lcd_ascii(&xy, "??\0", font_size, color);
    }
    else
    {
        for(uint16_t m = 0; m < 16; m++)
        {
            for(uint16_t n = 0; n < 2; n++)
            {
            k = t_characters[index<<1][m*2+n];
            for(uint16_t j = 0; j < 8; j++, k >>= 1)
            {
            if(k & 0x01){
                xy.x = p->x + 16*i + 8*n + j;
                xy.y = p->y + m;
                lcd_buf_writePoint(&xy, color);
            }
            }
            
            }
        }
    }
    }
}


/**
 * @brief  绘制无符号整数 用于绘制无符号整数uint,整数位置为point,整数颜色为color
 * @param  point 整数的位置坐标
 * @param  uint 要绘制的无符号整数
 * @param  font_size 整数的字体大小
 * @param  color 整数的颜色
 * @retval 无
 */
extern void lcd_uint(Point* point, uint32_t uint, uint16_t font_size, rgb565 color) {
    char c[12];
    uint8_t i = 0;
    Point p = *point;  // 使用临时坐标，保护原坐标
    
    if (uint == 0) {
        lcd_ascii(&p, "0", font_size, color);
        return;
    }
    
    while (uint != 0) {
        c[i++] = uint % 10 + '0';
        uint /= 10;
    }
    c[i] = '\0';
    
    // 从最高位到最低位绘制
    for(int8_t j = i - 1; j >= 0; j--) {
        char digit[2] = {c[j], '\0'};  // 构造字符串
        lcd_ascii(&p, digit, font_size, color);
        p.x += font_size;  // 固定偏移：每个字符宽度=font_size
    }
}
/**
 * @brief  绘制字符 用于绘制字符c,字符位置为point,字符颜色为color
 * @param  point 字符的位置坐标
 * @param  c 要绘制的字符
 * @param  font_size 字符的字体大小
 * @param  color 字符的颜色
 * @retval 无
 */
extern void lcd_ascii(Point* point,char* c, uint16_t font_size, rgb565 color){
    uint8_t k;
    Point p;
    // 字符宽度固定为8像素（ASCII字体宽度）
    uint16_t char_width = 8;
    
    for(uint16_t i = 0; c[i] != '\0'; i++)
    {
        for(uint16_t m = 0; m < 16; m++)  // 字符高度固定为16像素
        {
            k = ascii[c[i] - ' '][m];
            for(uint16_t j = 0; j < 8; j++, k >>= 1)
            {
                if(k & 0x01) {
                    // 计算像素坐标：字符起始位置 + 字符间距 + 字符内偏移
                    p.x = point->x + char_width * i + j;
                    p.y = point->y + m;
                    lcd_buf_writePoint(&p, color);
                }
            }
        }
    }
}
/**
 * @brief  写入显存点 用于将颜色color写入显存点(point->x,point->y)
 * @param  point 显存点坐标
 * @param  color 颜色
 * @retval 无
 */
void lcd_buf_writePoint(Point* point, rgb565 color){
    // 边界检查（必须）
    if(point->x >= LCD_WIDTH || point->y >= LCD_HEIGHT) {
        return;
    }
    
    // 使用数组索引方式，更安全可靠
    uint16_t *p = (uint16_t *)LCD_BUF_ADD_BAS;
    uint32_t idx = point->y * LCD_WIDTH + point->x;
    p[idx] = color;
}
/**
 * @brief  绘制圆 用于绘制圆心为point,半径为radius的圆
 * @param  point 圆的圆心坐标
 * @param  radius 圆的半径
 * @param  color 圆的颜色
 * @retval 无
 */
extern void lcd_circle(Point* point, uint16_t radius, rgb565 color){
    uint16_t x = 0, y = radius;
    uint32_t r2 = radius * radius, x2 = 0, y2 = r2;
    Point xy;
    while(x <= y)
    {
        xy.x = point->x + x; xy.y = point->y + y;
        lcd_buf_writePoint(&xy, color);
        xy.x = point->x - x; xy.y = point->y + y;
        lcd_buf_writePoint(&xy, color);
        xy.x = point->x + x; xy.y = point->y - y;
        lcd_buf_writePoint(&xy, color);
        xy.x = point->x - x; xy.y = point->y - y;
        lcd_buf_writePoint(&xy, color);
        xy.x = point->x + y; xy.y = point->y + x;
        lcd_buf_writePoint(&xy, color);
        xy.x = point->x - y; xy.y = point->y + x;
        lcd_buf_writePoint(&xy, color);
        xy.x = point->x + y; xy.y = point->y - x;
        lcd_buf_writePoint(&xy, color);
        xy.x = point->x - y; xy.y = point->y - x;
        lcd_buf_writePoint(&xy, color);
        x++;
        x2 = x2 + (x << 1) - 1;
        if(x2 + y2 > r2)
        {
            y--;
            y2 = y2 - (y << 1) - 1;
        }
    }
}
/**
 * @brief  计算绝对值
 * @param  x 输入值
 * @retval 绝对值
 */
static uint16_t abs16(int16_t x){
    return x > 0 ? x : -x;
}

/**
 * @brief  绘制水平线 用于绘制从点point1到点point2的水平线
 * @param  point1 水平线起点坐标
 * @param  point2 水平线终点坐标
 * @param  color 水平线颜色
 * @retval 无
 */
extern void lcd_line(Point* point1, Point* point2, rgb565 color){
    uint16_t x, y, *px, *py, zend, k0, k1;
    int16_t t, z = 1;
	//uint32_t add;
    if(abs16(point2->x - point1->x) > abs16(point2->y - point1->y))
    {//如果x方向变化大
    if(point2->x > point1->x)
    {
        x = point1->x;
        y = point1->y;
        zend = point2->x;
        if(point2->y < point1->y)//如果y2小于y1，则y向减小的方向变化
        z = -1;
    }
    else
    {
        x = point2->x;
        y = point2->y;
        zend = point1->x;
        if(point1->y < point2->y)//如果y1小于y2，则y向增大的方向变化
        z = -1;
    }
    px = &x;
    py = &y;
    k0 = abs16(point2->y - point1->y);
    k1 = abs16(point2->x - point1->x);
    }
    else
    {//如果y方向变化大
    if(point2->y > point1->y)//如果y1大于y0，则(x0,y0)为起点；(x1,y1)为终点
    {
        x = point1->x;
        y = point1->y;
        zend = point2->y;
        if(point2->x < point1->x)//若x1<x0，则x向减小的方向变化
        z = -1;
    }
    else
    {//如果y1大于y2，则(x2,y2)为起点；(x1,y1)为终点
        x = point2->x;
        y = point2->y;
        zend = point1->y;
        if(point1->x < point2->x)//若x0<x1，则x向减小的方向变化
        z = -1;
    }
    px = &y;
    py = &x;
    k0 = abs16(point2->x - point1->x);
    k1 = abs16(point2->y - point1->y);
    }
    t = -k1 / 2;
    while(*px <= zend)
    {
        Point p = {*px, *py};
        lcd_buf_writePoint(&p, color);
        (*px)++;
        t += k0;
        if(t >= 0)
        {
            (*py) += z;
            t -= k1;
        }
    }
}
/** 
 * @brief  刷新液晶屏显示
 * @param  无
 * @retval 无
 */
extern void bsp_lcd_refresh(void)
{
    // 刷新液晶屏显示，双层循环，先Y后X，顺序与官方一致
    uint16_t *p = (uint16_t *)LCD_BUF_ADD_BAS;
    lcd_setWindow(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    bsp_lcd_dc_data();
    bsp_lcd_cs_valid();
    for(uint16_t y = 0; y < LCD_HEIGHT; y++) {
        for(uint16_t x = 0; x < LCD_WIDTH; x++) {
            uint32_t idx = y * LCD_WIDTH + x;
            uint8_t buf[2];
            buf[0] = (uint8_t)(p[idx] >> 8);      // 高字节
            buf[1] = (uint8_t)(p[idx] & 0xFF);    // 低字节
            HAL_SPI_Transmit(&LCD_PORT, buf, 2, LCD_TIMEOUT);
        }
    }
    bsp_lcd_cs_invalid();
}

/**
 * @brief  写入像素点
 * @param  color 像素点颜色
 * @retval 无
 */
extern void lcd_writePixel(rgb565 color)
{
    // 写入像素点
    // HAL_SPI_Transmit(&LCD_PORT, (uint8_t *)&color, 2, LCD_TIMEOUT); // 原发送：低字节在前（小端）
    // 修复：高字节在前的顺序发送
    uint8_t buf[2];
    buf[0] = (uint8_t)(color >> 8);
    buf[1] = (uint8_t)(color & 0xFF);
    HAL_SPI_Transmit(&LCD_PORT, buf, 2, LCD_TIMEOUT);
}

/**
 * @brief  设置液晶屏分辨率
 * @param  无
 * @retval 无
 */
static void lcd_resolu_scan(void)
{
    // 扫描液晶屏分辨率
     //lcd_sendCommand(0x36);  // 原设置：仅设置内存访问控制
     //lcd_sendData(0x00);     // 原设置：正常扫描方向
    
//    // 修复：添加内存访问控制和像素格式设置
//    lcd_sendCommand(0x36);      // 内存访问控制
//    lcd_sendData(0x00);         // 正常扫描方向
//    HAL_Delay(10);              // 添加延时确保设置生效
//    
//    lcd_sendCommand(0x3A);      // 像素格式设置
//    lcd_sendData(0x55);         // 16位色深 (RGB565)
//    HAL_Delay(10);              // 添加延时确保设置生效
}
/**
 * @brief  初始化液晶屏
 * @param  无
 * @retval 无
 */
extern void lcd_init(void)
{
    // 初始化液晶屏
    lcd_module_init();
    lcd_reset();
    lcd_resolu_scan();
    lcd_init_reg();
    lcd_setWindow(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    
    // 在LCD完全初始化后再打开背光
    bsp_lcd_bl_on();
}

/**
 * @brief  清除液晶屏显示
 * @param  无
 * @retval 无
 */
extern void lcd_clear(void){
    // 清除液晶屏显示
    uint16_t *p;
    uint32_t i;
    p = (uint16_t *)LCD_BUF_ADD_BAS;
    for(i=0;i<LCD_WIDTH*LCD_HEIGHT;i++){
        p[i]=BACKGROUND;
    }
    bsp_lcd_refresh();
}
/**
 * @brief  设置窗口 用于设置液晶屏显示的窗口区域
 * @param  x1 窗口左上角x坐标
 * @param  y1 窗口左上角y坐标
 * @param  x2 窗口右下角x坐标
 * @param  y2 窗口右下角y坐标
 * @retval 无
 */
static void lcd_setWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    // 窗口设置
    // 如果x=0左边还有一列像素，说明LCD物理显示区域有X偏移，需要添加x_offset
    // 常见情况：ST7789某些模块的物理显示区域从x=1开始（而不是x=0）
    uint16_t x_offset = -1;  // X偏移1像素（修复：x=0左边还有一列像素的问题）
    uint16_t y_offset = 0;  // Y偏移（暂时设为0）

    bsp_lcd_cs_valid();

    // 列地址设置 0x2A
    bsp_lcd_dc_comm();
    uint8_t cmd = 0x2A;
    HAL_SPI_Transmit(&LCD_PORT, &cmd, 1, LCD_TIMEOUT);
    bsp_lcd_dc_data();
    // 列地址设置：CASET命令需要起始和结束地址（包含结束地址）
    // direct_spi_test: {0x00, 0x00, 0x00, 0xEF} = X: 0-239
    uint8_t ca[4] = {
        (uint8_t)((x1 + x_offset) >> 8), (uint8_t)((x1 + x_offset) & 0xFF),
        (uint8_t)((x2 + x_offset) >> 8), (uint8_t)((x2 + x_offset) & 0xFF)  // 修复：应该是x2，不是x2-1
    };
    HAL_SPI_Transmit(&LCD_PORT, ca, 4, LCD_TIMEOUT);

    // 行地址设置：RASET命令需要起始和结束地址（包含结束地址）
    // direct_spi_test: {0x00, 0x00, 0x01, 0x3F} = Y: 0-319
    bsp_lcd_dc_comm();
    cmd = 0x2B;
    HAL_SPI_Transmit(&LCD_PORT, &cmd, 1, LCD_TIMEOUT);
    bsp_lcd_dc_data();
    uint8_t ra[4] = {
        (uint8_t)((y1 + y_offset) >> 8), (uint8_t)((y1 + y_offset) & 0xFF),
        (uint8_t)((y2 + y_offset) >> 8), (uint8_t)((y2 + y_offset) & 0xFF)  // 修复：应该是y2，不是y2-1
    };
    HAL_SPI_Transmit(&LCD_PORT, ra, 4, LCD_TIMEOUT);

    // 存储器写入 0x2C
    bsp_lcd_dc_comm();
    cmd = 0x2C;
    HAL_SPI_Transmit(&LCD_PORT, &cmd, 1, LCD_TIMEOUT);

    bsp_lcd_cs_invalid();
}

/**
 * @brief  初始化液晶屏模块
 * @param  无
 * @retval 无
 */
static void lcd_module_init(void)
{
    // 初始化液晶屏模块
    bsp_lcd_dc_data();
    bsp_lcd_cs_valid();
    bsp_lcd_rst_invalid();

}

/**
 * @brief  初始化液晶屏寄存器
 * @param  无
 * @retval 无
 */
static void lcd_init_reg(void)
{
    // 先配置其他寄存器
    lcd_sendCommand(0xB2);
    lcd_sendData(0x0B);
    lcd_sendData(0x0B);
    lcd_sendData(0x00);
    lcd_sendData(0x33);
    lcd_sendData(0x35);

    lcd_sendCommand(0xB7);
    lcd_sendData(0x11);

    lcd_sendCommand(0xBB);
    lcd_sendData(0x35);

    lcd_sendCommand(0xC0);
    lcd_sendData(0x2C);

    lcd_sendCommand(0xC2);
    lcd_sendData(0x01);

    lcd_sendCommand(0xC3);
    lcd_sendData(0x0D);

    lcd_sendCommand(0xC4);
    lcd_sendData(0x20);

    lcd_sendCommand(0xC6);
    lcd_sendData(0x13);

    lcd_sendCommand(0xD0);
    lcd_sendData(0xA4);
    lcd_sendData(0xA1);

    lcd_sendCommand(0xD6);
    lcd_sendData(0xA1);

    lcd_sendCommand(0xE0);
    lcd_sendData(0xF0);
    lcd_sendData(0x06);
    lcd_sendData(0x0B);
    lcd_sendData(0x0A);
    lcd_sendData(0x09);
    lcd_sendData(0x26);
    lcd_sendData(0x29);
    lcd_sendData(0x33);
    lcd_sendData(0x41);
    lcd_sendData(0x18);
    lcd_sendData(0x16);
    lcd_sendData(0x15);
    lcd_sendData(0x29);
    lcd_sendData(0x2D);

    lcd_sendCommand(0xE1);
    lcd_sendData(0xF0);
    lcd_sendData(0x04);
    lcd_sendData(0x08);
    lcd_sendData(0x08);
    lcd_sendData(0x07);
    lcd_sendData(0x03);
    lcd_sendData(0x28);
    lcd_sendData(0x32);
    lcd_sendData(0x40);
    lcd_sendData(0x3B);
    lcd_sendData(0x19);
    lcd_sendData(0x18);
    lcd_sendData(0x2A);
    lcd_sendData(0x2E);

    lcd_sendCommand(0xE4);
    lcd_sendData(0x25);
    lcd_sendData(0x00);
    lcd_sendData(0x00);

    // 退出睡眠模式
    lcd_sendCommand(0x11);
    HAL_Delay(120);
    
    // 在SLPOUT之后设置内存访问控制和像素格式（关键：确保正确的扫描方向）
    lcd_sendCommand(0x36);  // MADCTL - 内存访问控制
    // MADCTL位定义：Bit7(MY)行地址顺序, Bit6(MX)列地址顺序, Bit5(MV)行列交换, Bit4(ML)垂直刷新, Bit3(RGB)BGR顺序
    // 0x00 = 正常扫描方向（与direct_spi_test一致）
    // 如果显示倾斜，可尝试：0x08(垂直镜像), 0x40(水平镜像), 0xC0(180度旋转)
    lcd_sendData(0x00);     // 正常扫描方向
    
    lcd_sendCommand(0x3A);  // COLMOD - 像素格式
    lcd_sendData(0x55);     // 16位色深 (RGB565)
    
    // 开启反色显示（与direct_spi_test一致）
    lcd_sendCommand(0x21);  // INVON - 开启反色
    
    // 开启显示
    lcd_sendCommand(0x29);  // DISPON
}
/**
 * @brief  发送数据
 * @param  data 数据
 * @retval 无
 */
static void lcd_sendData(uint8_t data)
{
    // 发送数据
    bsp_lcd_dc_data();
    bsp_lcd_cs_valid();
    if(HAL_SPI_Transmit(&LCD_PORT, &data, 1, LCD_TIMEOUT) != HAL_OK)
    {
        // 发送失败
        Error_Handler();
    }
    bsp_lcd_cs_invalid();
}
/**
 * @brief  发送命令
 * @param  command 命令
 * @retval 无
 */
static void lcd_sendCommand(uint8_t command)
{
    // 发送命令
    bsp_lcd_dc_comm();
    bsp_lcd_cs_valid();
    if(HAL_SPI_Transmit(&LCD_PORT, &command, 1, LCD_TIMEOUT) != HAL_OK)
    {
        // 发送失败
        Error_Handler();
    }
    bsp_lcd_cs_invalid();
    
}
/**
 * @brief  液晶屏复位
 * @param  无
 * @retval 无
 */
static void lcd_reset(void)
{
    // 复位液晶屏
    bsp_lcd_rst_valid();
    HAL_Delay(100);
    bsp_lcd_rst_invalid();
    HAL_Delay(200);

}


/**
 * @brief  背光使能
 * @param  无
 * @retval 无
 */
extern void bsp_lcd_bl_on(void)
{
    // 使能背光
    HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, LCD_BL_ON);
}
/**
 * @brief  背光熄灭
 * @param  无
 * @retval 无
 */
extern void bsp_lcd_bl_off(void)
{
    // 失能背光
    HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, LCD_BL_OFF);
}
/**
 * @brief  CS引脚有效
 * @param  无
 * @retval 无
 */
static void bsp_lcd_cs_valid(void)
{
    // 使能CS引脚
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, LCD_CS_ON);
}
/**
 * @brief  CS引脚无效
 * @param  无
 * @retval 无
 */
static void bsp_lcd_cs_invalid(void)
{
    // 失能CS引脚
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, LCD_CS_OFF);
}
/**
 * @brief  RST引脚有效 复位
 * @param  无
 * @retval 无
 */
static void bsp_lcd_rst_valid(void)
{
    // 使能RST引脚
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, LCD_RST_ON);
}
/**
 * @brief  RST引脚无效 释放
 * @param  无
 * @retval 无
 */
static void bsp_lcd_rst_invalid(void)
{
    // 失能RST引脚
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, LCD_RST_OFF);
}
/**
 * @brief  DC引脚数据
 * @param  无
 * @retval 无
 */
static void bsp_lcd_dc_data(void)
{
    // 使能DC引脚数据
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, LCD_DC_DATA);
}
/**
 * @brief  DC引脚命令
 * @param  无
 * @retval 无
 */
static void bsp_lcd_dc_comm(void)
{
    // 失能DC引脚命令
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, LCD_DC_COMM);
}

