/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <math.h>
#define M_PI 3.14159265358979323846
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
 int fputc(int ch, FILE *f)
 {
     /* 发送一个字节数据到串口USARTx */
     HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1,100);
     return (ch);
 }
  int fgetc(FILE *f)
  {
  uint8_t ch = 0;
  HAL_UART_Receive(&huart1, &ch, 1,100);
  return ch;
  }

#define MPU9250_ADDR  0xD0   // 7bit addr=0x68, write=0xD0, read=0xD1
#define AK8963_ADDR   0x18   // 磁力计地址 7bit=0x0C, write=0x18, read=0x19
// --- 姿态角 ---
float Pitch = 0, Roll = 0, Yaw = 0;
float dt = 0.02f;   // 采样周期 20ms

// --- 互补滤波参数 ---
float alpha = 0.96f;

// --- 校准比例 ---
#define ACCEL_SCALE 16384.0f   // ±2g
#define GYRO_SCALE  131.0f     // ±250°/s
#define MAG_SCALE   0.6f       // 粗略比例（μT）

// --- 校准偏置 ---
float gyro_bias[3] = {0, 0, 0};
// 新增：磁力计校准参数
float mag_offset[3] = {0, 0, 0};  // 磁力计偏移量
float mag_scale[3] = {1.0f, 1.0f, 1.0f};  // 磁力计缩放因子
uint8_t mag_calibrated = 0;  // 校准完成标志（0=未校准，1=已校准）
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
uint8_t rx_data;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    //HAL_UART_Transmit(&huart1, (uint8_t *)"hello windows!\r\n", 18, 0xFFFF);
    HAL_UART_Transmit_IT(&huart1, &rx_data, sizeof(rx_data));
    HAL_UART_Receive_IT(&huart1, &rx_data, 1);
    // 处理USART1接收完成事件
    // 例如：启动下一次接收、解析数据等
  }
}

/* --- MPU9250 初始化 --- */
void MPU9250_Init(void)
{
    uint8_t data;
    HAL_Delay(100);

    // 唤醒MPU9250
    data = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, MPU9250_ADDR, 0x6B, 1, &data, 1, 100);

    // 设置陀螺仪 ±250°/s
    data = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, MPU9250_ADDR, 0x1B, 1, &data, 1, 100);

    // 设置加速度计 ±2g
    data = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, MPU9250_ADDR, 0x1C, 1, &data, 1, 100);

    // 使能旁路访问磁力计
    data = 0x02;
    HAL_I2C_Mem_Write(&hi2c1, MPU9250_ADDR, 0x37, 1, &data, 1, 100);

    // 初始化 AK8963
    data = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, AK8963_ADDR, 0x0A, 1, &data, 1, 100);
    HAL_Delay(10);
    data = 0x16;  // 连续测量模式2，16bit输出
    HAL_I2C_Mem_Write(&hi2c1, AK8963_ADDR, 0x0A, 1, &data, 1, 100);
}

/* --- 读取原始数据 --- */
void MPU9250_Read(float *ax, float *ay, float *az,
                  float *gx, float *gy, float *gz,
                  float *mx, float *my, float *mz)
{
    uint8_t buf[14];
    HAL_I2C_Mem_Read(&hi2c1, MPU9250_ADDR, 0x3B, 1, buf, 14, 100);
    int16_t ax_raw = (buf[0] << 8) | buf[1];
    int16_t ay_raw = (buf[2] << 8) | buf[3];
    int16_t az_raw = (buf[4] << 8) | buf[5];
    int16_t gx_raw = (buf[8] << 8) | buf[9];
    int16_t gy_raw = (buf[10] << 8) | buf[11];
    int16_t gz_raw = (buf[12] << 8) | buf[13];

    *ax = (float)ax_raw / ACCEL_SCALE;
    *ay = (float)ay_raw / ACCEL_SCALE;
    *az = (float)az_raw / ACCEL_SCALE;
    *gx = ((float)gx_raw / GYRO_SCALE) - gyro_bias[0];
    *gy = ((float)gy_raw / GYRO_SCALE) - gyro_bias[1];
    *gz = ((float)gz_raw / GYRO_SCALE) - gyro_bias[2];

    // --- 读取磁力计（修改：添加校准修正） ---
    uint8_t mag_buf[7];
    HAL_I2C_Mem_Read(&hi2c1, AK8963_ADDR, 0x03, 1, mag_buf, 7, 100);
    int16_t mx_raw = (mag_buf[1] << 8) | mag_buf[0];
    int16_t my_raw = (mag_buf[3] << 8) | mag_buf[2];
    int16_t mz_raw = (mag_buf[5] << 8) | mag_buf[4];

    if (mag_calibrated)
    {
        // 应用校准：消除偏移 + 统一轴缩放
        *mx = ((float)mx_raw * MAG_SCALE - mag_offset[0]) * mag_scale[0];
        *my = ((float)my_raw * MAG_SCALE - mag_offset[1]) * mag_scale[1];
        *mz = ((float)mz_raw * MAG_SCALE - mag_offset[2]) * mag_scale[2];
    }
    else
    {
        // 未校准：使用原始转换值
        *mx = (float)mx_raw * MAG_SCALE;
        *my = (float)my_raw * MAG_SCALE;
        *mz = (float)mz_raw * MAG_SCALE;
    }

}
void Calibrate_Gyro(void)
{
    float gx, gy, gz, ax, ay, az, mx, my, mz;
    gyro_bias[0] = gyro_bias[1] = gyro_bias[2] = 0;

    printf("Calibrating gyro... Keep sensor still!\r\n");
    // 临时变量
    float gxi, gyi, gzi;
    gyro_bias[0] = gyro_bias[1] = gyro_bias[2] = 0;
    for (int i = 0; i < 500; i++)
    {
        MPU9250_Read(&ax, &ay, &az, &gxi, &gyi, &gzi, &mx, &my, &mz);
        gyro_bias[0] += gxi;
        gyro_bias[1] += gyi;
        gyro_bias[2] += gzi;
        HAL_Delay(2);
    }
    gyro_bias[0] /= 500.0f;
    gyro_bias[1] /= 500.0f;
    gyro_bias[2] /= 500.0f;

    printf("Gyro bias: %.3f, %.3f, %.3f\r\n", gyro_bias[0], gyro_bias[1], gyro_bias[2]);
}
/* --- 互补滤波姿态解算 --- */
void Update_Attitude(float ax, float ay, float az,
                     float gx, float gy, float gz,
                     float mx, float my, float mz)
{
    // 保证正确dt
    static uint32_t last_tick = 0;
    uint32_t now_tick = HAL_GetTick();
    static int inited = 0;
    float real_dt;
    if (!inited) {
        inited = 1;
        last_tick = now_tick;
        real_dt = 0.02f;
    } else {
        real_dt = (now_tick - last_tick) / 1000.0f;
        last_tick = now_tick;
        if(real_dt < 0.001f) real_dt = 0.02f; // 防异常
        if(real_dt > 0.1f) real_dt = 0.02f;   // 防堵塞
    }
    
    // 标准方向定义（航空电子惯例 pitch: nose up >0, roll: right wing down >0）
    float pitch_acc = atan2f(-ax, sqrtf(ay*ay + az*az)) * 180.f / M_PI;
    float roll_acc  = atan2f(ay, az) * 180.f / M_PI;

    // 磁力计校正航向
    float pitch_rad = pitch_acc * M_PI / 180.0f;
    float roll_rad  = roll_acc  * M_PI / 180.0f;
    float mag_x     = mx * cosf(pitch_rad) + mz * sinf(pitch_rad);
    float mag_y     = mx * sinf(roll_rad)*sinf(pitch_rad)
                    + my * cosf(roll_rad)
                    - mz * sinf(roll_rad)*cosf(pitch_rad);
    float yaw_mag   = atan2f(-mag_y, mag_x) * 180.f / M_PI;
    if (yaw_mag < 0) yaw_mag += 360;

    // 互补滤波（核心部分，alpha减小，加速度计参与校正）
    Pitch = alpha * (Pitch + gy * real_dt) + (1.0f - alpha) * pitch_acc;
    Roll  = alpha * (Roll  + gx * real_dt) + (1.0f - alpha) * roll_acc;
    Yaw   = alpha * (Yaw   + gz * real_dt) + (1.0f - alpha) * yaw_mag;

    // 角度范围限制
    if (Pitch > 180.f)  Pitch -= 360.f;
    if (Pitch < -180.f) Pitch += 360.f;
    if (Roll  > 180.f)  Roll  -= 360.f;
    if (Roll  < -180.f) Roll  += 360.f;
    if (Yaw   > 360.f)  Yaw   -= 360.f;
    if (Yaw   < 0.f)    Yaw   += 360.f;
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
    printf("MPU9250 Init...\r\n");
    MPU9250_Init();
    Calibrate_Gyro();   // 上电自动校准
    float ax, ay, az, gx, gy, gz, mx, my, mz;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
        MPU9250_Read(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz);
        Update_Attitude(ax, ay, az, gx, gy, gz, mx, my, mz);

        printf("Pitch: %.2f°, Roll: %.2f°, Yaw: %.2f°\r\n", Pitch, Roll, Yaw);

        HAL_Delay(20); // 50Hz 采样
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
