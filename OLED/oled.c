#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  

/************************** 全局变量定义 **************************/
// OLED显示缓冲区（128列×8页，对应128×64像素）
// 每个字节代表8个垂直像素，OLED_GRAM[x][y/8]的某一位对应(x,y)像素
u8 OLED_GRAM[128][8];
// OLED设备I2C地址（默认使用OLED_I2C_ADDR，初始化时会检测适配）
static uint16_t oled_dev_addr = OLED_I2C_ADDR;

/************************** 核心功能函数 **************************/

/**
 * @brief  OLED屏幕颜色反转控制
 * @param  i: 0-正常显示（黑底白字） 1-颜色反转（白底黑字）
 * @retval 无
 * @note   底层调用SSD1306的A6/A7指令实现
 */
void OLED_ColorTurn(u8 i)
{
    if(i == 0)
        OLED_WR_Byte(0xA6, OLED_CMD); // 正常显示（默认）
    if(i == 1)
        OLED_WR_Byte(0xA7, OLED_CMD); // 反色显示
}

/**
 * @brief  OLED屏幕180度旋转控制
 * @param  i: 0-正常显示 1-屏幕旋转180度显示
 * @retval 无
 * @note   组合调用SSD1306的C8/A1（正常）或C0/A0（旋转）指令
 */
void OLED_DisplayTurn(u8 i)
{
    if(i == 0)
    {
        OLED_WR_Byte(0xC8, OLED_CMD); // 正常显示（COM扫描方向）
        OLED_WR_Byte(0xA1, OLED_CMD); // 正常显示（段地址映射）
    }
    if(i == 1)
    {
        OLED_WR_Byte(0xC0, OLED_CMD); // 旋转显示（COM扫描方向反转）
        OLED_WR_Byte(0xA0, OLED_CMD); // 旋转显示（段地址映射反转）
    }
}

/**
 * @brief  向OLED写入字节（命令/数据）【硬件I2C2实现】
 * @param  dat: 要写入的命令/数据字节
 * @param  mode: 写入类型（OLED_CMD=0 写命令；OLED_DATA=1 写数据）
 * @retval 无
 * @note   替换原SPI逻辑，适配SSD1306的I2C通信协议：
 *         1. I2C帧格式：起始位 → 设备地址(0x78/0x7A) → 控制字节 → 数据字节 → 停止位
 *         2. 控制字节：bit7=0（固定），bit6=D/C（0=命令，1=数据），bit5-0=0
 *         3. 硬件I2C2对应CubeMX配置：PB10=SCL，PB3=SDA
 */
void OLED_WR_Byte(u8 dat, u8 mode)
{
    u8 tx_buf[2];
    // 构造I2C发送缓冲区：
    // tx_buf[0] = 控制字节（D/C位：0=命令 0x00，1=数据 0x40）
    // tx_buf[1] = 实际要发送的命令/数据
    tx_buf[0] = mode ? 0x40 : 0x00;
    tx_buf[1] = dat;
    
    // 硬件I2C2发送数据（阻塞模式，超时500ms）
    HAL_I2C_Master_Transmit(&hi2c2, oled_dev_addr, tx_buf, 2, 500);
}

/**
 * @brief  开启OLED显示（退出休眠）
 * @param  无
 * @retval 无
 * @note   1. 0x8D：电荷泵使能指令
 *         2. 0x14：开启电荷泵（必须开启才能点亮屏幕）
 *         3. 0xAF：开启显示（默认上电后为关闭状态）
 */
void OLED_DisPlay_On(void)
{
    OLED_WR_Byte(0x8D, OLED_CMD); // 电荷泵使能指令
    OLED_WR_Byte(0x14, OLED_CMD); // 开启电荷泵
    OLED_WR_Byte(0xAF, OLED_CMD); // 开启OLED显示
}

/**
 * @brief  关闭OLED显示（进入休眠，低功耗）
 * @param  无
 * @retval 无
 * @note   1. 0x8D：电荷泵使能指令
 *         2. 0x10：关闭电荷泵
 *         3. 0xAE：关闭显示（屏幕黑屏，缓冲区数据保留）
 */
void OLED_DisPlay_Off(void)
{
    OLED_WR_Byte(0x8D, OLED_CMD); // 电荷泵使能指令
    OLED_WR_Byte(0x10, OLED_CMD); // 关闭电荷泵
    OLED_WR_Byte(0xAE, OLED_CMD); // 关闭OLED显示
}

/**
 * @brief  将显示缓冲区数据刷新到OLED屏幕
 * @param  无
 * @retval 无
 * @note   1. SSD1306按"页"刷新：共8页（0~7），每页对应8行像素（0~7行）
 *         2. 刷新流程：设置页地址 → 设置列地址 → 写入整页数据
 *         3. 使用HAL_I2C_Mem_Write批量写入，效率高于单字节发送
 */
void OLED_Refresh(void)
{
    u8 i, n;
    for(i = 0; i < 8; i++)  // 遍历8个页（每页8行，共64行）
    {
        // 设置页地址（0xB0~0xB7对应页0~页7）
        OLED_WR_Byte(0xb0 + i, OLED_CMD);
        // 设置列起始地址（低4位 + 高4位），兼容列偏移
        OLED_WR_Byte(0x00 + (OLED_COL_OFFSET & 0x0F), OLED_CMD);  // 列地址低4位
        OLED_WR_Byte(0x10 + ((OLED_COL_OFFSET >> 4) & 0x0F), OLED_CMD); // 列地址高4位
        
        // 构造当前页的128列数据缓冲区
        u8 tx_buf[128];
        for(n = 0; n < 128; n++)
            tx_buf[n] = OLED_GRAM[n][i];
        
        // 批量写入当前页数据：
        // 参数说明：I2C句柄 → 设备地址 → 内存地址(0x40=写数据) → 地址长度(8位) → 数据缓冲区 → 长度 → 超时
        HAL_I2C_Mem_Write(&hi2c2, oled_dev_addr, 0x40, I2C_MEMADD_SIZE_8BIT, tx_buf, 128, 500);
    }
}

/**
 * @brief  清空OLED显示缓冲区并刷新屏幕
 * @param  无
 * @retval 无
 * @note   1. 将缓冲区所有字节置0（对应所有像素熄灭）
 *         2. 调用Refresh刷新到屏幕，完成清屏
 */
void OLED_Clear(void)
{
    u8 i, n;
    // 遍历缓冲区，全部置0
    for(i = 0; i < 8; i++)
        for(n = 0; n < 128; n++)
            OLED_GRAM[n][i] = 0;
    OLED_Refresh(); // 刷新屏幕，显示清空后的状态
}

/**
 * @brief  在指定坐标画点
 * @param  x: 横坐标（0~127）
 * @param  y: 纵坐标（0~63）
 * @param  t: 点状态（0-熄灭 1-点亮）
 * @retval 无
 * @note   1. 先判断坐标是否越界，越界则直接返回
 *         2. 页计算：y/8 → 确定该点属于第几页
 *         3. 位计算：y%8 → 确定该点在页内的第几位
 */
void OLED_DrawPoint(u8 x, u8 y, u8 t)
{
    u8 i, m, n;
    if(x > 127 || y > 63) return; // 坐标越界检查
    
    i = y / 8;    // 计算点所在的页（0~7）
    m = y % 8;    // 计算点在页内的位（0~7）
    n = 1 << m;   // 构造位掩码
    
    if(t) 
        OLED_GRAM[x][i] |= n;  // 点亮：对应位置1
    else 
        OLED_GRAM[x][i] &= ~n; // 熄灭：对应位置0
}

/**
 * @brief  绘制直线（Bresenham算法）
 * @param  x1: 起点横坐标（0~127）
 * @param  y1: 起点纵坐标（0~63）
 * @param  x2: 终点横坐标（0~127）
 * @param  y2: 终点纵坐标（0~63）
 * @param  mode: 点状态（0-熄灭 1-点亮）
 * @retval 无
 * @note   1. 适配任意斜率的直线（水平、垂直、斜线）
 *         2. 通过误差累积法减少浮点运算，提高效率
 */
void OLED_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2, u8 mode)
{
    u16 t; 
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    
    delta_x = x2 - x1; // 计算x方向差值
    delta_y = y2 - y1; // 计算y方向差值
    uRow = x1;         // 当前点x坐标
    uCol = y1;         // 当前点y坐标
    
    // 确定x方向步进方向
    if(delta_x > 0) incx = 1;
    else if(delta_x == 0) incx = 0; // 垂直线
    else {incx = -1; delta_x = -delta_x;}
    
    // 确定y方向步进方向
    if(delta_y > 0) incy = 1;
    else if(delta_y == 0) incy = 0; // 水平线
    else {incy = -1; delta_y = -delta_y;}
    
    // 确定最大步进距离（x/y方向最大值）
    if(delta_x > delta_y) distance = delta_x;
    else distance = delta_y;
    
    // 逐点绘制直线
    for(t = 0; t < distance + 1; t++)
    {
        OLED_DrawPoint(uRow, uCol, mode); // 绘制当前点
        xerr += delta_x;
        yerr += delta_y;
        
        // x方向误差累积超过阈值，步进x
        if(xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        // y方向误差累积超过阈值，步进y
        if(yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

/**
 * @brief  绘制圆形（中点画圆算法）
 * @param  x: 圆心横坐标（0~127）
 * @param  y: 圆心纵坐标（0~63）
 * @param  r: 圆半径
 * @retval 无
 * @note   1. 利用圆的8对称性减少计算量
 *         2. 通过误差判断调整绘制点，保证圆形平滑
 */
void OLED_DrawCircle(u8 x, u8 y, u8 r)
{
    int a, b, num;
    a = 0;
    b = r;
    
    // 中点画圆算法核心：判断点与圆的位置关系
    while(2 * b * b >= r * r)      
    {
        // 利用8对称性绘制8个点
        OLED_DrawPoint(x + a, y - b, 1);
        OLED_DrawPoint(x - a, y - b, 1);
        OLED_DrawPoint(x - a, y + b, 1);
        OLED_DrawPoint(x + a, y + b, 1);
        OLED_DrawPoint(x + b, y + a, 1);
        OLED_DrawPoint(x + b, y - a, 1);
        OLED_DrawPoint(x - b, y - a, 1);
        OLED_DrawPoint(x - b, y + a, 1);
        
        a++;
        // 计算当前点到圆心的距离与半径的差值
        num = (a*a + b*b) - r*r;
        // 若点在圆外，调整b值
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}

/**
 * @brief  显示单个字符
 * @param  x: 字符起始横坐标（0~127）
 * @param  y: 字符起始纵坐标（0~63）
 * @param  chr: 要显示的字符（ASCII码，仅支持空格~~）
 * @param  size1: 字符大小（8/12/16/24，需匹配字库）
 * @param  mode: 显示模式（0-覆盖显示 1-反色显示）
 * @retval 无
 * @note   1. 字库对应关系：0806(8x6)、1206(12x6)、1608(16x8)、2412(24x12)
 *         2. 字符偏移：chr-' ' → 转换为字库索引（空格为第一个字符）
 */
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size1, u8 mode)
{
    u8 i, m, temp, size2, chr1;
    u8 x0 = x, y0 = y;
    
    // 计算字符占用的字节数
    if(size1 == 8) size2 = 6;
    else size2 = (size1/8 + ((size1%8) ? 1 : 0)) * (size1/2);
    
    chr1 = chr - ' ';  // 转换为字库索引（空格对应索引0）
    
    // 遍历字符的每个字节
    for(i = 0; i < size2; i++)
    {
        // 根据字符大小选择对应字库
        if(size1 == 8) temp = asc2_0806[chr1][i];
        else if(size1 == 12) temp = asc2_1206[chr1][i];
        else if(size1 == 16) temp = asc2_1608[chr1][i];
        else if(size1 == 24) temp = asc2_2412[chr1][i];
        else return; // 不支持的字符大小，直接返回
        
        // 遍历字节的每一位，绘制像素
        for(m = 0; m < 8; m++)
        {
            if(temp & 0x01) OLED_DrawPoint(x, y, mode);
            else OLED_DrawPoint(x, y, !mode);
            temp >>= 1; // 右移，处理下一位
            y++;        // y坐标递增
        }
        x++; // x坐标递增
        
        // 字符换行处理（非8号字体，每绘制size1/2列换一行）
        if((size1 != 8) && ((x - x0) == size1/2))
        {
            x = x0;
            y0 = y0 + 8;
        }
        y = y0; // 复位y坐标
    }
}

/**
 * @brief  显示字符串
 * @param  x: 字符串起始横坐标（0~127）
 * @param  y: 字符串起始纵坐标（0~63）
 * @param  chr: 字符串指针（ASCII码，仅支持空格~~）
 * @param  size1: 字符大小（8/12/16/24，需匹配字库）
 * @param  mode: 显示模式（0-覆盖显示 1-反色显示）
 * @retval 无
 * @note   1. 逐个字符调用ShowChar显示
 *         2. 字符间距：8号字体间距6列，其他字体间距size1/2列
 */
void OLED_ShowString(u8 x, u8 y, u8 *chr, u8 size1, u8 mode)
{
    // 遍历字符串，直到结束符或非打印字符
    while((*chr >= ' ') && (*chr <= '~'))
    {
        OLED_ShowChar(x, y, *chr, size1, mode);
        // 根据字体大小调整x坐标（字符宽度）
        if(size1 == 8) x += 6;
        else x += size1/2;
        chr++; // 指向下一个字符
    }
}

/**
 * @brief  幂运算函数（用于数字显示的位数计算）
 * @param  m: 底数
 * @param  n: 指数
 * @retval 计算结果（m^n）
 * @note   仅支持正整数幂运算，n=0时返回1
 */
u32 OLED_Pow(u8 m, u8 n)
{
    u32 result = 1;
    while(n--) result *= m;
    return result;
}

/**
 * @brief  显示数字（十进制，支持补0）
 * @param  x: 数字起始横坐标（0~127）
 * @param  y: 数字起始纵坐标（0~63）
 * @param  num: 要显示的数字（0~4294967295，u32范围）
 * @param  len: 显示位数（不足补0，超过则显示完整数字）
 * @param  size1: 数字大小（8/12/16/24，需匹配字库）
 * @param  mode: 显示模式（0-覆盖显示 1-反色显示）
 * @retval 无
 * @note   1. 逐位提取数字，转换为字符后显示
 *         2. 8号字体额外调整间距（+2）
 */
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size1, u8 mode)
{
    u8 t, temp, m = 0;
    if(size1 == 8) m = 2; // 8号字体间距补偿
    
    // 逐位处理数字
    for(t = 0; t < len; t++)
    {
        // 提取当前位数字（高位到低位）
        temp = (num / OLED_Pow(10, len - t - 1)) % 10;
        // 显示当前位（0也显示，实现补0效果）
        if(temp == 0)
            OLED_ShowChar(x + (size1/2 + m)*t, y, '0', size1, mode);
        else 
            OLED_ShowChar(x + (size1/2 + m)*t, y, temp + '0', size1, mode);
    }
}

/**
 * @brief  显示中文汉字
 * @param  x: 汉字起始横坐标（0~127）
 * @param  y: 汉字起始纵坐标（0~63）
 * @param  num: 汉字在字库中的索引
 * @param  size1: 汉字大小（16/24/32/64，需匹配字库）
 * @param  mode: 显示模式（0-覆盖显示 1-反色显示）
 * @retval 无
 * @note   1. 字库对应关系：Hzk1(16x16)、Hzk2(24x24)、Hzk3(32x32)、Hzk4(64x64)
 *         2. 汉字为点阵结构，逐字节逐位绘制
 */
void OLED_ShowChinese(u8 x, u8 y, u8 num, u8 size1, u8 mode)
{
    u8 m, temp;
    u8 x0 = x, y0 = y;
    // 计算汉字占用的字节数
    u16 i, size3 = (size1/8 + ((size1%8) ? 1 : 0)) * size1;
    
    // 遍历汉字的每个字节
    for(i = 0; i < size3; i++)
    {
        // 根据汉字大小选择对应字库
        if(size1 == 16) temp = Hzk1[num][i];
        else if(size1 == 24) temp = Hzk2[num][i];
        else if(size1 == 32) temp = Hzk3[num][i];
        else if(size1 == 64) temp = Hzk4[num][i];
        else return; // 不支持的汉字大小，直接返回
        
        // 遍历字节的每一位，绘制像素
        for(m = 0; m < 8; m++)
        {
            if(temp & 0x01) OLED_DrawPoint(x, y, mode);
            else OLED_DrawPoint(x, y, !mode);
            temp >>= 1; // 右移，处理下一位
            y++;        // y坐标递增
        }
        x++; // x坐标递增
        
        // 汉字换行处理（每绘制size1列换一行）
        if((x - x0) == size1)
        {
            x = x0;
            y0 = y0 + 8;
        }
        y = y0; // 复位y坐标
    }
}

/**
 * @brief  OLED初始化函数
 * @param  无
 * @retval 无
 * @note   1. 替换原SPI/GPIO初始化，适配I2C通信
 *         2. 自动检测OLED地址（0x78/0x7A），提高兼容性
 *         3. 初始化流程严格遵循SSD1306 datasheet推荐配置
 */
void OLED_Init(void)
{
    // 检测OLED设备是否在线（先试0x78，失败则试0x7A）
    if (HAL_I2C_IsDeviceReady(&hi2c2, oled_dev_addr, 5, 100) != HAL_OK)
    {
        oled_dev_addr = 0x7A;
        // 若0x7A也检测失败，直接返回（OLED未连接）
        if (HAL_I2C_IsDeviceReady(&hi2c2, oled_dev_addr, 5, 100) != HAL_OK)
        {
            return;
        }
    }

    // ------------------- SSD1306 初始化指令序列 -------------------
    OLED_WR_Byte(0xAE, OLED_CMD); // 关闭显示
    OLED_WR_Byte(0x00, OLED_CMD); // 设置列地址低4位
    OLED_WR_Byte(0x10, OLED_CMD); // 设置列地址高4位
    OLED_WR_Byte(0x40, OLED_CMD); // 设置显示起始行
    OLED_WR_Byte(0xB0, OLED_CMD); // 设置页地址
    OLED_WR_Byte(0x81, OLED_CMD); // 设置对比度
    OLED_WR_Byte(0xFF, OLED_CMD); // 对比度最大值（0~255）
    OLED_WR_Byte(0xA1, OLED_CMD); // 设置段地址映射（正常显示）
    OLED_WR_Byte(0xA6, OLED_CMD); // 正常显示（非反色）
    OLED_WR_Byte(0xA8, OLED_CMD); // 设置多路复用率
    OLED_WR_Byte(0x3F, OLED_CMD); // 多路复用率=63（64行）
    OLED_WR_Byte(0xC8, OLED_CMD); // COM扫描方向（正常）
    OLED_WR_Byte(0xD3, OLED_CMD); // 设置显示偏移
    OLED_WR_Byte(0x00, OLED_CMD); // 显示偏移=0
    OLED_WR_Byte(0xD5, OLED_CMD); // 设置时钟分频因子
    OLED_WR_Byte(0x80, OLED_CMD); // 分频因子=1，震荡频率=默认
    OLED_WR_Byte(0xD9, OLED_CMD); // 设置预充电周期
    OLED_WR_Byte(0xF1, OLED_CMD); // 预充电周期=15 DCLK
    OLED_WR_Byte(0xDA, OLED_CMD); // 设置COM引脚配置
    OLED_WR_Byte(0x12, OLED_CMD); // COM引脚硬件配置
    OLED_WR_Byte(0xDB, OLED_CMD); // 设置VCOMH电压
    OLED_WR_Byte(0x40, OLED_CMD); // VCOMH=0.77*VCC
    OLED_WR_Byte(0x8D, OLED_CMD); // 电荷泵使能
    OLED_WR_Byte(0x14, OLED_CMD); // 开启电荷泵

    OLED_Clear();                 // 清空缓冲区
    OLED_WR_Byte(0xAF, OLED_CMD); // 开启显示
}