#include "fdc2214.h"

// I2C延时函数（优化通信可靠性，适配50MHz主频）
void FDC_IIC_Delay(void)
{
    delay_us(8);
}

// I2C初始化
void FDC_IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7); // SCL/SDA默认拉高
}

// I2C起始信号
void FDC_IIC_Start(void)
{
    FDC_SDA_OUT();
    FDC_IIC_SDA = 1;
    FDC_IIC_SCL = 1;
    FDC_IIC_Delay();
    FDC_IIC_SDA = 0;  // 先SDA拉低，再SCL拉低
    FDC_IIC_Delay();
    FDC_IIC_SCL = 0;
}      

// I2C停止信号
void FDC_IIC_Stop(void)
{
    FDC_SDA_OUT();
    FDC_IIC_SCL = 0;
    FDC_IIC_SDA = 0;
    FDC_IIC_Delay();
    FDC_IIC_SCL = 1;
    FDC_IIC_Delay();
    FDC_IIC_SDA = 1;  // 先SCL拉高，再SDA拉高
    FDC_IIC_Delay();
}

// I2C等待应答
u8 FDC_IIC_Wait_Ack(void)
{
    u16 ucErrTime = 0;
    FDC_SDA_IN();
    FDC_IIC_SDA = 1;
    FDC_IIC_Delay();
    FDC_IIC_SCL = 1;
    FDC_IIC_Delay();

    while(FDC_READ_SDA)  // 等待SDA拉低
    {
        ucErrTime++;
        if(ucErrTime > 1000)  // 增加超时阈值，提升稳定性
        {
            FDC_IIC_Stop();
            return 1; // 应答失败
        }
    }

    FDC_IIC_SCL = 0;
    return 0;  // 应答成功
}

// I2C发送应答
void FDC_IIC_Ack(void)
{
    FDC_IIC_SCL = 0;
    FDC_SDA_OUT();
    FDC_IIC_SDA = 0;
    FDC_IIC_Delay();
    FDC_IIC_SCL = 1;
    FDC_IIC_Delay();
    FDC_IIC_SCL = 0;
    FDC_IIC_Delay();
    FDC_IIC_SDA = 1; // 释放SDA
}
    
// I2C发送非应答
void FDC_IIC_NAck(void)
{
    FDC_IIC_SCL = 0;
    FDC_SDA_OUT();
    FDC_IIC_SDA = 1;
    FDC_IIC_Delay();
    FDC_IIC_SCL = 1;
    FDC_IIC_Delay();
    FDC_IIC_SCL = 0;
}

// I2C发送1字节
void FDC_IIC_Send_Byte(u8 txd)
{
    u8 t;
    
    FDC_SDA_OUT();
    FDC_IIC_SCL = 0;
    
    for(t = 0; t < 8; t++)
    {
        FDC_IIC_SDA = (txd & 0x80) >> 7; // 高位先行
        txd <<= 1;
        FDC_IIC_SCL = 1;
        FDC_IIC_Delay();
        FDC_IIC_SCL = 0;
        FDC_IIC_Delay();
    }
}

// I2C读取1字节
u8 FDC_IIC_Read_Byte(u8 ack)
{
    u8 i, receive = 0;
    FDC_SDA_IN();

    for(i = 0; i < 8; i++)
    {
        FDC_IIC_SCL = 0;
        FDC_IIC_Delay();
        FDC_IIC_SCL = 1;
        receive <<= 1;

        if(FDC_READ_SDA)
        {
            receive++;
        }

        FDC_IIC_Delay();
    }

    if (!ack)
    {
        FDC_IIC_NAck();
    }
    else
    {
        FDC_IIC_Ack();
    }

    return receive;
}

// 配置FDC2214寄存器（16位）
u8 Set_FDC2214(u8 reg,u8 MSB,u8 LSB)
{
    FDC_IIC_Start();
    FDC_IIC_Send_Byte((FDC2214_ADDR<<1)|0); // 写操作

    if(FDC_IIC_Wait_Ack())
    {
        FDC_IIC_Stop();
        return 1; // 通信失败
    }

    FDC_IIC_Send_Byte(reg);
    FDC_IIC_Wait_Ack();
    FDC_IIC_Send_Byte(MSB);
    FDC_IIC_Wait_Ack(); // 补充应答等待，提升可靠性
    FDC_IIC_Send_Byte(LSB);
    FDC_IIC_Wait_Ack(); // 补充应答等待，提升可靠性

    FDC_IIC_Stop();
    return 0; // 配置成功
}

// 读取FDC2214 16位寄存器
u16 FDC_Read(u8 reg)
{
    u16 res;

    FDC_IIC_Start();
    FDC_IIC_Send_Byte((FDC2214_ADDR<<1)|0);
    FDC_IIC_Wait_Ack();
    FDC_IIC_Send_Byte(reg);
    FDC_IIC_Wait_Ack();

    FDC_IIC_Start(); // 重复起始信号
    FDC_IIC_Send_Byte((FDC2214_ADDR<<1)|1); // 读操作
    FDC_IIC_Wait_Ack();
    res = FDC_IIC_Read_Byte(1) << 8; // 读取MSB，发送应答
    res |= FDC_IIC_Read_Byte(0);    // 读取LSB，发送非应答
    FDC_IIC_Stop();

    return res;        
}

// 读取指定通道的28位数据
u32 FCD2214_ReadCH(u8 index) 
{
    u32 result = 0;

    switch(index)
    {
        case 0:
            result = (u32)(FDC_Read(DATA_CH0) & 0x0FFF) << 16; // 高12位
            result |= FDC_Read(DATA_LSB_CH0);                 // 低16位
            break;
        case 1:
            result = (u32)(FDC_Read(DATA_CH1) & 0x0FFF) << 16;
            result |= FDC_Read(DATA_LSB_CH1);
            break;
        case 2:
            result = (u32)(FDC_Read(DATA_CH2) & 0x0FFF) << 16;
            result |= FDC_Read(DATA_LSB_CH2);
            break;
        case 3:
            result = (u32)(FDC_Read(DATA_CH3) & 0x0FFF) << 16;
            result |= FDC_Read(DATA_LSB_CH3);
            break;
        default:
            printf("通道索引错误！\r\n");
            break;
    }

    result &= 0x0FFFFFFF; // 保留28位有效数据
    return result;
}

// FDC2214初始化（核心优化：增大量程+提升稳定性）
u8 FDC2214_Init(void)
{
    u16 manu_id, dev_id;

    FDC_IIC_Init();
    delay_ms(10); // 增加上电延时，确保芯片稳定

    // 读取厂商ID和设备ID，验证通信
    manu_id = FDC_Read(MANUFACTURER_ID);
    dev_id = FDC_Read(DEVICE_ID);
    printf("厂商ID: 0x%x, 设备ID: 0x%x \r\n", manu_id, dev_id);
    
    // 验证ID正确性（原代码厂商ID判断错误，已修正）
    if(manu_id == 0x5449 && dev_id == 0x3055)
    {

        /******** 核心配置1：降低RCOUNT，增大量程（关键优化） ********/
        // RCOUNT值越小，可检测电容上限越大；0x14FB相比0x24FB进一步降低，量程更大
        Set_FDC2214(RCOUNT_CH0, 0x14, 0xFB);
        Set_FDC2214(RCOUNT_CH1, 0x14, 0xFB);
        Set_FDC2214(RCOUNT_CH2, 0x14, 0xFB);
        Set_FDC2214(RCOUNT_CH3, 0x14, 0xFB);

        /******** 核心配置2：增大SETTLECOUNT，提升测量稳定性 ********/
        // 稳定计数越大，测量越稳定（牺牲少量速度），0x003F相比0x001B稳定性更强
        Set_FDC2214(SETTLECOUNT_CH0, 0x00, 0x3F);
        Set_FDC2214(SETTLECOUNT_CH1, 0x00, 0x3F);
        Set_FDC2214(SETTLECOUNT_CH2, 0x00, 0x3F);
        Set_FDC2214(SETTLECOUNT_CH3, 0x00, 0x3F);

        /******** 核心配置3：提高时钟分频系数，最大化量程 ********/
        // 0x8002相比0x4002进一步提高分频，电容检测上限更大
        Set_FDC2214(CLOCK_DIVIDERS_C_CH0, 0x80, 0x02);
        Set_FDC2214(CLOCK_DIVIDERS_C_CH1, 0x80, 0x02);
        Set_FDC2214(CLOCK_DIVIDERS_C_CH2, 0x80, 0x02);
        Set_FDC2214(CLOCK_DIVIDERS_C_CH3, 0x80, 0x02);

        /******** 核心配置4：优化驱动电流，平衡量程与稳定性 ********/
        // 0x6000相比0x7800，驱动电流适中，减少噪声干扰
        Set_FDC2214(DRIVE_CURRENT_CH0, 0x60, 0x00);
        Set_FDC2214(DRIVE_CURRENT_CH1, 0x60, 0x00);
        Set_FDC2214(DRIVE_CURRENT_CH2, 0x60, 0x00);
        Set_FDC2214(DRIVE_CURRENT_CH3, 0x60, 0x00);

        /******** 核心配置5：配置错误处理，提升鲁棒性 ********/
        Set_FDC2214(ERROR_CONFIG, 0x01, 0x01); // 使能错误中断，自动清除错误

        /******** 核心配置6：固定通道扫描，提升测量一致性 ********/
        Set_FDC2214(MUX_CONFIG, 0xC0, 0x0D); // 禁用自动切换，固定通道顺序

        /******** 核心配置7：优化全局配置，稳定采样 ********/
        Set_FDC2214(CONFIG, 0x14, 0x03); // 提高采样精度，禁用低功耗模式，提升稳定性

        printf("FDC2214初始化成功！\r\n");
        return 0;
    }
    else
    {
        printf("FDC2214初始化失败：ID不匹配！\r\n");
        return 1;
    }
}

// 电容计算（原始值）
float Cap_Calculate(u8 chx)
{
    u32 Data_FDC;
    float Cap;

    if(chx > 3) // 通道合法性判断，提升稳定性
    {
        return 0.0f;
    }

    Data_FDC = FCD2214_ReadCH(chx);
    if(Data_FDC == 0) // 避免除零错误
    {
        return 0.0f;
    }
    Cap = 232021045.248 / (float)Data_FDC;

    return (Cap * Cap);
}

// 新增：带滑动平均滤波的电容计算（大幅提升稳定性，可配置平均次数）
float Cap_Calculate_Filter(u8 chx, u8 avg_cnt)
{
    u8 i;
    float sum_cap = 0.0f;
    float single_cap;

    if(chx > 3 || avg_cnt < 1 || avg_cnt > 32) // 参数合法性校验
    {
        return 0.0f;
    }

    // 多次采样求平均
    for(i = 0; i < avg_cnt; i++)
    {
        single_cap = Cap_Calculate(chx);
        sum_cap += single_cap;
        delay_ms(5); // 采样间隔，避免数据相关性
    }

    return sum_cap / (float)avg_cnt;
}