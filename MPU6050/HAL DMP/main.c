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
#include "inv_mpu.h"
#include "mpu6050.h"
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
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
 int fputc(int ch, FILE *f)
 {
     /* ???????????????????USARTx */
     HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
     return (ch);
 }
  int fgetc(FILE *f)
  {
  uint8_t ch = 0;
  HAL_UART_Receive(&huart1, &ch, 1, 0xffff);
  return ch;
  }
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
uint8_t rx_data;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    //HAL_UART_Transmit(&huart1, (uint8_t *)"hello windows!\r\n", 18, 0xFFFF);
    HAL_UART_Transmit(&huart1, &rx_data, sizeof(rx_data), 0xFFFF);
    HAL_UART_Receive_IT(&huart1, &rx_data, 1);
    // ????USART1??????????
    // ???磺????????ν?????????????
  }
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//void MPU6050_INIT()
//{
//    int i = 0, j = 0;
//    
//    //???
//    HAL_Delay(100);
//    uint8_t SendAddress = 0x6b;
//    uint8_t SendData  = 0x00; //?????????
//    HAL_I2C_Mem_Write(&hi2c1,0xD1,SendAddress,1,&SendData,1,0xff);
//    
//    SendAddress = 0x19;//??????????
//    SendData = 0x07;
//    HAL_I2C_Mem_Write(&hi2c1,0xD1,SendAddress,1,&SendData,1,0xff);
//    
//    SendAddress = 0x1A;//????????
//    SendData = 0x06;
//    HAL_I2C_Mem_Write(&hi2c1,0xD1,SendAddress,1,&SendData,1,0xff);
//    
//    SendAddress = 0x1B;//??????
//    SendData = 0x08;   //?? 500 ??/s
//    HAL_I2C_Mem_Write(&hi2c1,0xD1,SendAddress,1,&SendData,1,0xff);
//    
//    SendAddress = 0x1C;//??????
//    SendData = 0x00;   //?? 2g
//    HAL_I2C_Mem_Write(&hi2c1,0xD1,SendAddress,1,&SendData,1,0xff);
//    
//}
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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  printf("\r\n=== MPU6050 DMP Initialize Start ===\r\n");
  
  // Check I2C bus status first
  printf("Checking I2C bus status...\r\n");
  HAL_Delay(100);
  
  // Try to read MPU6050 device ID
  uint8_t whoami = 0;
  uint16_t mpu_addr = 0xD0; // MPU6050 I2C address
  
  // Read WHO_AM_I register (0x75) using HAL_I2C_Mem_Read
//  HAL_StatusTypeDef i2c_result = HAL_I2C_Mem_Read(&hi2c1, mpu_addr, 0x75, 1, &whoami, 1, 100);
//  
//  if (i2c_result == HAL_OK) {
//      printf("I2C通信正常，WHO_AM_I = 0x%02X (期望 0x68)\r\n", whoami);
//      if (whoami != 0x68) {
//          printf("警告: WHO_AM_I值不正确，可能是设备地址或硬件问题\r\n");
//      }
//  } else {
//      printf("I2C通信失败，错误码: %d\r\n", i2c_result);
//      printf("Please check:\r\n");
//          printf("1. SCL and SDA pin connections\r\n");
//          printf("2. Pull-up resistors (4.7kΩ)\r\n");
//          printf("3. Power supply (3.3V)\r\n");
//          printf("4. MPU6050 module damage\r\n");
//      }
//      
//      printf("Starting DMP initialization...\r\n");
  HAL_Delay(200);
  
  uint8_t MPU_Data[] = {0X00, 0X00,0X00, 0X00,0X00, 0X00,0X00, 0X00,0X00, 0X00,0X00, 0X00,0X00, 0X00};
  uint8_t preg1_Data  = 0x3B;
  double ACCEL_XOUT = 0.0,ACCEL_YOUT =0.0,ACCEL_ZOUT=0.0;
  double GYRO_XOUT = 0.0,GYRO_YOUT =0.0,GYRO_ZOUT=0.0;
  float pitch=0,roll=0,yaw=0;

  int init_result;
  int retry_count = 0;
  const int max_retries = 5;
  
  do {
      printf("Attempt %d of DMP initialization...\r\n", retry_count + 1);
      init_result = mpu_dmp_init();
      
      if (init_result != 0) {
          printf("DMP initialization failed, error code: %d\r\n", init_result);
          
          // 根据错误码提供具体诊断信息
          switch (init_result) {
              case -1:
                  printf("错误: I2C通信失败\r\n");
                  break;
              case -2:
                  printf("错误: MPU6050设备ID不正确\r\n");
                  break;
              case -3:
                  printf("错误: 传感器自检失败\r\n");
                  break;
              case -4:
                  printf("错误: DMP固件加载失败\r\n");
                  break;
              case -5:
                  printf("错误: DMP配置失败\r\n");
                  break;
              default:
                  printf("错误: 未知错误\r\n");
                  break;
          }
          
          retry_count++;
          if (retry_count < max_retries) {
              printf("等待1秒后重试...\r\n");
              HAL_Delay(1000);
          }
      }
  } while (init_result != 0 && retry_count < max_retries);
  
  if (init_result == 0) {
      printf("DMP初始化成功！\r\n");
  } else {
      printf("DMP初始化最终失败，请检查硬件连接和配置\r\n");
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
        if( mpu_dmp_get_data(&pitch,&roll,&yaw) == 0 )
        { 
            printf("\r\npitch =%.2f\r\n", pitch);
            printf("\r\nroll =%.2f\r\n", roll);
            printf("\r\nyaw =%.2f\r\n", yaw);
        }
        HAL_Delay(200);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//      HAL_I2C_Mem_Read(&hi2c1, 0xD1,preg1_Data,I2C_MEMADD_SIZE_8BIT,MPU_Data,14,50);
//      //ShowHex(MPU_Data,14);
//      float Temp = (MPU_Data[6]<<8)|MPU_Data[7];
//      if(Temp>32768) Temp-=65536;
//      Temp=(36.53+Temp/340);
//      
//      short int ACCEL_XOUT1 = ((MPU_Data[0]<<8)|MPU_Data[1]);ACCEL_XOUT = (double)ACCEL_XOUT1/16384;
//      short int ACCEL_YOUT1 = ((MPU_Data[2]<<8)|MPU_Data[3]);ACCEL_YOUT = (double)ACCEL_YOUT1/16384;
//      short int ACCEL_ZOUT1 = ((MPU_Data[4]<<8)|MPU_Data[5]);ACCEL_ZOUT = (double)ACCEL_ZOUT1/16384;
//      
//      
//      short int GYRO_XOUT1 = ((MPU_Data[8]<<8)|MPU_Data[9]);GYRO_XOUT = (double)GYRO_XOUT1/65.5;
//      short int GYRO_YOUT1 = ((MPU_Data[10]<<8)|MPU_Data[11]);GYRO_YOUT = (double)GYRO_YOUT1/65.5;
//      short int GYRO_ZOUT1 = ((MPU_Data[12]<<8)|MPU_Data[13]);GYRO_ZOUT = (double)GYRO_ZOUT1/65.5;
//      
//      //printf("temp = %0.2f",Temp);
//      printf("x= %0.2f,y= %0.2f,z= %0.2f",GYRO_XOUT,GYRO_YOUT,GYRO_ZOUT);//?????
//      //printf("x= %0.2f,y= %0.2f,z= %0.2f",ACCEL_XOUT,ACCEL_YOUT,ACCEL_ZOUT);
//      printf("\r\n");
//      HAL_Delay(200);
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
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
