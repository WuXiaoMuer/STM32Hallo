// ========== nRF24L01硬件定义 ==========
#define NRF24_CSN_PIN    GPIO_PIN_3    // CSN引脚PA3
#define NRF24_CSN_PORT   GPIOA
#define NRF24_CE_PIN     GPIO_PIN_4    // CE引脚PA4
#define NRF24_CE_PORT    GPIOA

// 引脚操作宏
#define NRF24_CSN_HIGH()  HAL_GPIO_WritePin(NRF24_CSN_PORT, NRF24_CSN_PIN, GPIO_PIN_SET)
#define NRF24_CSN_LOW()   HAL_GPIO_WritePin(NRF24_CSN_PORT, NRF24_CSN_PIN, GPIO_PIN_RESET)
#define NRF24_CE_LOW()    HAL_GPIO_WritePin(NRF24_CE_PORT, NRF24_CE_PIN, GPIO_PIN_RESET)
#define NRF24_CE_HIGH()   HAL_GPIO_WritePin(NRF24_CE_PORT, NRF24_CE_PIN, GPIO_PIN_SET)

// ========== nRF24L01寄存器/命令定义 ==========
#define NRF24_REG_CONFIG      0x00    // 配置寄存器
#define NRF24_REG_EN_AA       0x01    // 自动应答使能
#define NRF24_REG_EN_RXADDR   0x02    // 接收地址使能
#define NRF24_REG_SETUP_AW    0x03    // 地址宽度设置
#define NRF24_REG_SETUP_RETR  0x04    // 重发设置
#define NRF24_REG_RF_CH       0x05    // 射频通道
#define NRF24_REG_RF_SETUP    0x06    // 射频配置
#define NRF24_REG_STATUS      0x07    // 状态寄存器
#define NRF24_REG_RX_ADDR_P0  0x0A    // 接收地址P0
#define NRF24_REG_TX_ADDR     0x10    // 发送地址
#define NRF24_REG_RX_PW_P0    0x11    // 接收通道P0数据长度
#define NRF24_REG_FIFO_STATUS 0x17    // FIFO状态寄存器

#define NRF24_CMD_W_REGISTER   0x20    // 写寄存器命令
#define NRF24_CMD_R_REGISTER   0x00    // 读寄存器命令
#define NRF24_CMD_W_TX_PAYLOAD 0xA0    // 写发送数据
#define NRF24_CMD_R_RX_PAYLOAD 0x61    // 读接收数据
#define NRF24_CMD_FLUSH_TX     0xE1    // 清空TX FIFO
#define NRF24_CMD_FLUSH_RX     0xE2    // 清空RX FIFO

// 通信参数配置（收发端必须完全一致）
const uint8_t NRF24_ADDR[5] = {0x34, 0x43, 0x10, 0x10, 0x01}; // 5字节通信地址
#define NRF24_PAYLOAD_SIZE    5       // 数据包长度（字节）
#define NRF24_RF_CHANNEL      40      // 射频通道（2.440GHz）



/**
  * @brief  SPI1 读写一个字节（nRF24L01专用）
  * @param  tx: 要发送的字节
  * @retval 接收到的字节
  */
uint8_t NRF24_SPI_RW_Byte(uint8_t tx)
{
  uint8_t rx = 0;
  // SPI收发（超时500ms，降低速率提高兼容性）
  HAL_SPI_TransmitReceive(&hspi1, &tx, &rx, 1, 500);
  return rx;
}

/**
  * @brief  写nRF24L01寄存器
  * @param  reg: 寄存器地址
  * @param  value: 要写入的值
  * @retval 状态寄存器值
  */
uint8_t NRF24_Write_Reg(uint8_t reg, uint8_t value)
{
  uint8_t status;
  NRF24_CSN_LOW();
  status = NRF24_SPI_RW_Byte(NRF24_CMD_W_REGISTER | (reg & 0x1F)); // 写命令+地址
  NRF24_SPI_RW_Byte(value);
  NRF24_CSN_HIGH();
  return status;
}


/**
  * @brief  读nRF24L01寄存器
  * @param  reg: 寄存器地址
  * @retval 寄存器值
  */
uint8_t NRF24_Read_Reg(uint8_t reg)
{
  uint8_t value;
  NRF24_CSN_LOW();
  NRF24_SPI_RW_Byte(NRF24_CMD_R_REGISTER | (reg & 0x1F)); // 读命令+地址
  value = NRF24_SPI_RW_Byte(0xFF); // 0xFF占位读取
  NRF24_CSN_HIGH();
  return value;
}

/**
  * @brief  写nRF24L01多字节寄存器（地址/数据）
  * @param  reg: 寄存器地址
  * @param  buf: 数据缓冲区
  * @param  len: 数据长度
  * @retval 状态寄存器值
  */
uint8_t NRF24_Write_Buf(uint8_t reg, const uint8_t *buf, uint8_t len)
{
  uint8_t status, i;
  NRF24_CSN_LOW();
  status = NRF24_SPI_RW_Byte(NRF24_CMD_W_REGISTER | (reg & 0x1F));
  for(i = 0; i < len; i++)
  {
    NRF24_SPI_RW_Byte(buf[i]);
  }
  NRF24_CSN_HIGH();
  return status;
}

/**
  * @brief  清空nRF24L01 FIFO
  * @param  is_tx: 1=清空TX FIFO，0=清空RX FIFO
  */
void NRF24_Flush_FIFO(uint8_t is_tx)
{
  NRF24_CSN_LOW();
  if(is_tx)
    NRF24_SPI_RW_Byte(NRF24_CMD_FLUSH_TX);
  else
    NRF24_SPI_RW_Byte(NRF24_CMD_FLUSH_RX);
  NRF24_CSN_HIGH();
  HAL_Delay(1); // 确保命令执行
}

/**
  * @brief  清除nRF24L01中断标志
  */
void NRF24_Clear_IRQ_Flags(void)
{
  // 写STATUS寄存器清除RX_DR(bit6)、TX_DS(bit5)、MAX_RT(bit4)
  NRF24_Write_Reg(NRF24_REG_STATUS, 0x70);
}

/**
  * @brief  nRF24L01断电重置（借鉴安信可代码，提升稳定性）
  */
void NRF24_Power_Off(void)
{
  NRF24_CE_LOW();
  NRF24_Write_Reg(NRF24_REG_CONFIG, 0x0D); // 断电配置
  NRF24_CE_HIGH();
  HAL_Delay(1);
  NRF24_CE_LOW();
}



/**
  * @brief  接收数据包
  * @param  rxbuf: 接收数据缓冲区
  * @param  len: 要接收的长度
  */
void NRF24_RxPacket(uint8_t *rxbuf, uint8_t len)
{
  NRF24_CSN_LOW();
  NRF24_SPI_RW_Byte(NRF24_CMD_R_RX_PAYLOAD);
  for(uint8_t i = 0; i < len; i++)
  {
    rxbuf[i] = NRF24_SPI_RW_Byte(0xFF);
  }
  NRF24_CSN_HIGH();

  NRF24_Clear_IRQ_Flags();
  NRF24_Flush_FIFO(0); // 清空RX FIFO
}

/**
  * @brief  发送数据包（修复版：确保数据写入FIFO并触发发送）
  * @param  txbuf: 发送数据缓冲区
  * @param  len: 数据长度（<=32）
  * @retval 0=发送成功，1=重发超限，2=失败
  */
uint8_t NRF24_TxPacket(uint8_t *txbuf, uint8_t len)
{
  uint8_t status;
  if(len > 32) len = 32; // 最大32字节

  NRF24_CE_LOW(); // 发送前拉低CE，确保模块处于待机状态

  // 1. 清空TX FIFO（避免残留数据干扰）
  NRF24_Flush_FIFO(1);

  // 2. 写发送数据到TX FIFO（关键步骤，确保数据被正确写入）
  NRF24_CSN_LOW();
  NRF24_SPI_RW_Byte(NRF24_CMD_W_TX_PAYLOAD); // 写TX payload命令
  for(uint8_t i = 0; i < len; i++)
  {
    NRF24_SPI_RW_Byte(txbuf[i]); // 逐字节写入数据
  }
  NRF24_CSN_HIGH();

  // 3. 触发发送（CE高脉冲>10us，必须满足时序）
  NRF24_CE_HIGH();
  HAL_Delay(1); // 高电平保持1ms，远大于10us要求
  NRF24_CE_LOW();

  // 4. 等待发送结果（最多等待20ms，确保重发完成）
  HAL_Delay(20);
  status = NRF24_Read_Reg(NRF24_REG_STATUS);

  // 5. 处理发送状态
  if(status & 0x20) // TX_DS：发送成功（收到接收端应答）
  {
    NRF24_Clear_IRQ_Flags();
    NRF24_Flush_FIFO(1);
    return 0;
  }
  else if(status & 0x10) // MAX_RT：重发超限（接收端未应答）
  {
    UART_PRINT("发送端：未收到接收端应答\r\n");
    NRF24_Clear_IRQ_Flags();
    NRF24_Flush_FIFO(1);
    return 1;
  }
  else // 发送失败（数据未发出）
  {
    UART_PRINT("发送端：数据未发出，STATUS=0x%02X\r\n", status);
    return 2;
  }
}
/**
  * @brief  读nRF24L01多字节寄存器（地址/数据）
  * @param  reg: 寄存器地址
  * @param  buf: 接收数据缓冲区
  * @param  len: 数据长度
  * @retval 状态寄存器值
  */
uint8_t NRF24_Read_Buf(uint8_t reg, uint8_t *buf, uint8_t len)
{
  uint8_t status, i;
  NRF24_CSN_LOW();
  status = NRF24_SPI_RW_Byte(NRF24_CMD_R_REGISTER | (reg & 0x1F)); // 读命令+地址
  for(i = 0; i < len; i++)
  {
    buf[i] = NRF24_SPI_RW_Byte(0xFF); // 逐字节读取
  }
  NRF24_CSN_HIGH();
  return status;
}
/**
  * @brief  初始化nRF24L01为TX或RX模式（修复版：确保模式正确）
  * @param  mode: 1=RX模式，0=TX模式
  */
void NRF24_Init(uint8_t mode)
{
  NRF24_Power_Off(); // 断电重置
  NRF24_CE_LOW();

  // 修复CONFIG寄存器配置：
  // TX模式：0x0E（PWR_UP=1，CRC=2字节，PRIM_RX=0）
  // RX模式：0x1F（PWR_UP=1，CRC=2字节，PRIM_RX=1，IRQ使能）
  if(mode == 0) // TX模式
    NRF24_Write_Reg(NRF24_REG_CONFIG, 0x0E);
  else // RX模式
    NRF24_Write_Reg(NRF24_REG_CONFIG, 0x1F); // 增加IRQ使能，提升接收灵敏度
  HAL_Delay(2); // 等待上电稳定（必须≥1.5ms）

  NRF24_Write_Reg(NRF24_REG_EN_AA, 0x01);        // 使能通道0自动应答（收发端必须都开启）
  NRF24_Write_Reg(NRF24_REG_EN_RXADDR, 0x01);    // 使能通道0接收
  NRF24_Write_Reg(NRF24_REG_SETUP_AW, 0x03);     // 地址宽度5字节
  NRF24_Write_Reg(NRF24_REG_SETUP_RETR, 0x1A);   // 500us重发间隔，10次重发
  NRF24_Write_Reg(NRF24_REG_RF_CH, NRF24_RF_CHANNEL); // 通道40
  NRF24_Write_Reg(NRF24_REG_RF_SETUP, 0x27);     // 2Mbps，0dBm（稳定优先）

  // 配置收发地址（确保完全一致）
  NRF24_Write_Buf(NRF24_REG_RX_ADDR_P0, NRF24_ADDR, 5);
  NRF24_Write_Buf(NRF24_REG_TX_ADDR, NRF24_ADDR, 5);

  NRF24_Write_Reg(NRF24_REG_RX_PW_P0, NRF24_PAYLOAD_SIZE); // 接收数据长度5字节

  // 清空FIFO（关键：初始化后必须清空）
  NRF24_Flush_FIFO(0);
  NRF24_Flush_FIFO(1);
  NRF24_Clear_IRQ_Flags();

  // 激活模式
  if(mode == 1) // RX模式：CE持续高电平
  {
    NRF24_CE_HIGH();
    HAL_Delay(1); // 等待RX模式稳定
  }
  else // TX模式：CE保持低电平
    NRF24_CE_LOW();
}



/*
  #define TX_MODE 1  // 1=发送端，0=接收端（两个板子必须不同）

  uint8_t tx_data[NRF24_PAYLOAD_SIZE] = {0xA5, 0x5A, 0x12, 0x34, 0xFF};
  uint8_t rx_data[NRF24_PAYLOAD_SIZE] = {0};
  uint8_t tx_status, status, fifo_status;

  // 打印初始化信息
  UART_PRINT("=== nRF24L01+ 稳定通信测试 ===\r\n");
  UART_PRINT("硬件配置：SCK=PA5, MOSI=PA7, MISO=PA6\r\n");
  UART_PRINT("         CSN=PA3, CE=PA4\r\n");
  UART_PRINT("通信参数：地址=%02X%02X%02X%02X%02X, 通道=%d, 长度=%d\r\n",
             NRF24_ADDR[0], NRF24_ADDR[1], NRF24_ADDR[2], NRF24_ADDR[3], NRF24_ADDR[4],
             NRF24_RF_CHANNEL, NRF24_PAYLOAD_SIZE);
  UART_PRINT("==============================\r\n");

  // 初始化nRF24L01
  #if TX_MODE
    UART_PRINT("本机角色：发送端\r\n");
    NRF24_Init(0); // 初始化为TX模式
  #else
    UART_PRINT("本机角色：接收端\r\n");
    NRF24_Init(1); // 初始化为RX模式
  #endif

  HAL_Delay(500); // 等待模块稳定

  // 模块检测（验证SPI通信是否正常）
  UART_PRINT("模块检测：\r\n");
  UART_PRINT("CONFIG=0x%02X (正常应为0x%02X)\r\n", NRF24_Read_Reg(NRF24_REG_CONFIG),
             TX_MODE ? 0x0F : 0x1F);
  UART_PRINT("RF_CH=0x%02X (正常应为0x%02X)\r\n", NRF24_Read_Reg(NRF24_REG_RF_CH), NRF24_RF_CHANNEL);
  UART_PRINT("EN_RXADDR=0x%02X (正常应为0x01)\r\n", NRF24_Read_Reg(NRF24_REG_EN_RXADDR));
  if(NRF24_Read_Reg(NRF24_REG_EN_RXADDR) != 0x01)
  {
    UART_PRINT("!!! SPI通信异常，请检查接线 !!!\r\n");
    while(1);
  }
  UART_PRINT("模块检测通过，开始通信...\r\n");
*/

  #if TX_MODE
  // 发送端主循环
  while (1)
  {
    // 发送数据包
    tx_status = NRF24_TxPacket(tx_data, NRF24_PAYLOAD_SIZE);

    // 打印发送状态
    UART_PRINT("[发送端] ");
    if(tx_status == 0)
      UART_PRINT("发送成功（收到应答）: ");
    else if(tx_status == 1)
      UART_PRINT("发送失败（重发超限）: ");
    else
      UART_PRINT("发送超时: ");

    // 打印发送数据
    for(uint8_t i = 0; i < NRF24_PAYLOAD_SIZE; i++)
    {
      UART_PRINT("%02X ", tx_data[i]);
    }
    UART_PRINT("\r\n");

    // PC13闪烁（指示发送周期）
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    HAL_Delay(1000); // 1秒发送一次
  }
#else
// 接收端主循环（修复版）
uint32_t cnt = 0;
uint8_t rx_addr[5] = {0}; // 用于读取并验证地址
while (1)
{
  status = NRF24_Read_Reg(NRF24_REG_STATUS);
  fifo_status = NRF24_Read_Reg(NRF24_REG_FIFO_STATUS);

  // 每5秒验证一次地址配置（确保地址写入正确）
  if(cnt % 50 == 0)
  {
    NRF24_Read_Buf(NRF24_REG_RX_ADDR_P0, rx_addr, 5);
    UART_PRINT("接收端：配置地址=%02X%02X%02X%02X%02X，实际地址=%02X%02X%02X%02X%02X\r\n",
               NRF24_ADDR[0], NRF24_ADDR[1], NRF24_ADDR[2], NRF24_ADDR[3], NRF24_ADDR[4],
               rx_addr[0], rx_addr[1], rx_addr[2], rx_addr[3], rx_addr[4]);
    if(memcmp(rx_addr, NRF24_ADDR, 5) != 0)
    {
      UART_PRINT("接收端：地址配置错误，重新写入...\r\n");
      NRF24_Write_Buf(NRF24_REG_RX_ADDR_P0, NRF24_ADDR, 5); // 重新写入地址
    }
  }

  // 修复接收逻辑：即使RX_DR未置位，也检测FIFO是否有数据（避免标志位漏判）
  if(((status & 0x40) || (fifo_status & 0x01) == 0))
  {
    NRF24_RxPacket(rx_data, NRF24_PAYLOAD_SIZE);
    UART_PRINT("[接收端] 收到数据: ");
    for(uint8_t i = 0; i < NRF24_PAYLOAD_SIZE; i++)
    {
      UART_PRINT("%02X ", rx_data[i]);
    }
    UART_PRINT("\r\n");
  }

  // 1秒打印一次心跳和状态
  if(++cnt >= 10)
  {
    UART_PRINT("[接收端] 心跳正常 | STATUS=0x%02X | FIFO=0x%02X\r\n", status, fifo_status);
    cnt = 0;
  }

  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
  HAL_Delay(100);
}
#endif