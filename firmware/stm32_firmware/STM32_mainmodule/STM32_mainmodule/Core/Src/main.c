/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "dma.h"
#include "fatfs.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include "stdio.h"
#include "string.h"
#include "LED.h"
#include "UARTCom.h"
#include "OLED.h"
#include "MPU6050.h"
#include "BME280.h"
#include "SDCard.h"
#include "util.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAXLEN 4096
#define MAXTEMPLEN 256
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//SPI_HandleTypeDef hspi2;

uint8_t count = 0;
extern uint16_t timer;
extern int enable_timer;
extern uint16_t timer_ESP32;
extern int enable_timer_ESP32;

BME280_Data_t BME280;
MPU6050_Data_t MPU6050;
int start_y = 20;
int end_y = 0;
static char line_buffer[512];
static uint16_t line_idx = 0;

typedef enum {
    STATE_STOPPED,
    STATE_START_REQUESTED,
    STATE_RUNNING,
    STATE_STOP_REQUESTED
} SystemState;

volatile SystemState state = STATE_STOPPED;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void I2C_ClearBus(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void logging(char *msg) {
  FILE *fptr;
  fptr = fopen("D:\\error_log.txt", "a+");
  if (fptr == NULL) {
    my_printf("ERROR: Could not open file.\n");
    return;
  }

  fprintf(fptr, "ERROR %s\n", msg);
  fclose(fptr);

}

int _write(int file, char *ptr, int len) {
	if (ptr == NULL || len <= 0) {
		return 0;
	}
	if (file != 1 && file != 2)
		return len;

	int i=0;
	for (i = 0; i < len; i++) {
		ITM_SendChar((*ptr++));
	}
	return len;
}

void I2C_Scanner(void) {
    HAL_StatusTypeDef result;
    uint8_t devices_found = 0;
    char temp_buffer[512];

    my_printf("\r\n--- Starting I2C Bus Scan ---\r\n");

    for (uint16_t i = 1; i < 128; i++) {
        /* * HAL_I2C_IsDeviceReady expects the left-shifted 8-bit address.
         * It tries to communicate with the device 3 times with a 5ms timeout.
         */
        result = HAL_I2C_IsDeviceReady(&hi2c1, (i << 1), 3, 5);

        if (result == HAL_OK) {
            sprintf(temp_buffer, "Device found at 7-bit address: 0x%02X (8-bit Addr: 0x%02X)\r\n", i, (i << 1));
            my_printf(temp_buffer);
            devices_found++;
        }
    }

    if (devices_found == 0) {
        my_printf("No I2C devices found. Check wiring, power, and pull-up resistors.\r\n");
    } else {
        sprintf(temp_buffer, "Scan complete. Found %d device(s).\r\n", devices_found);
        my_printf(temp_buffer);
    }
    my_printf("-----------------------------\r\n\r\n");
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_13)
    {
        if (state == STATE_STOPPED)
        {
            state = STATE_START_REQUESTED;
        }
        else if (state == STATE_RUNNING)
        {
            state = STATE_STOP_REQUESTED;
        }
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	char buffer[1000];
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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_SPI2_Init();
  MX_FATFS_Init();
  MX_RTC_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  /*
  SDCard_write();
  SDCard_read();

  int val = SDCard_write_log("test start\n");
  if (val != 0) {
	  my_printf("SDCard_write_log ERROR\n");
  }
  my_printf("\n");
  */
  //RECEIVING VALUES
  //UARTCom_rx();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	switch (state)
	{
	  case STATE_STOPPED:
		  // wait for push button
		  break;

	  case STATE_START_REQUESTED:
		  // Mount SD
		  // Open file
		  //SD_Card_Test();
		  SDCard_init();
		  SDCard_open_log();
		  SDCard_open_csv();

		  I2C_ClearBus();
		  I2C_Scanner();

		  OLED_init();

		  LED_init();
		  UARTCom_init();
		  BME280_init();
		  MPU6050_init();
		  MPU6050_Calibrate(&MPU6050);
		  UARTCom_initESP32();
		  UARTCom_receivefromESP32();
		  state = STATE_RUNNING;
		  break;

	  case STATE_RUNNING:
		//make sure all transmitted values have \r\n endings
		sprintf(buffer, "Hello World count = %d, Timer = %d\r\n", count, timer);
		my_printf(buffer);

		//SENDING VALUES
		//UARTCom_tx((uint8_t*)buffer);

		//Timer for UART receive timeout
		if (timer >= 1000) {
		  //entire data has been received
		  timer = 0;
		  enable_timer = 0;
		}
		if (timer_ESP32 >= 1000) {
		  //entire data has been received
		  timer_ESP32 = 0;
		  enable_timer_ESP32 = 0;
		}

		LED_blink();
		 count++;

		 BME280_calculation(&BME280);
		 sprintf(buffer, "BME280: Temp: %.2f C, Pres: %.2f hPa, Humi: %.2f RH, Alti: %.2f m\n",
						BME280.Temperature, BME280.Pressure, BME280.Humidity, BME280.AltitudeP);
		 my_printf(buffer);
		 start_y = 20;
		 end_y = 0;
		 sprintf(buffer, "Temp: %.2f C\nPres: %.2f hPa\nHumi: %.2f RH\nAlti: %.2f m\n",
						BME280.Temperature, BME280.Pressure, BME280.Humidity, BME280.AltitudeP);
		 OLED_update(buffer, start_y, &end_y, 1);
		 start_y = end_y;

		 HAL_Delay(1000);

		 MPU6050_Read_All(&MPU6050);
		 sprintf(buffer, "MPU6050: Ax: %.2f g, Ay: %.2f g, Az: %.2f g, Gx: %.2f Deg/s, Gy: %.2f Deg/s, Gz: %.2f Deg/s\n",
						MPU6050.Ax , MPU6050.Ay, MPU6050.Az , MPU6050.Gx, MPU6050.Gy , MPU6050.Gz);
		 my_printf(buffer);
		 sprintf(buffer, "MPU6050: Roll: %.2f Deg, Pitch: %.2f Deg, KalmanX: %.2f Deg, KalmanY: %.2f Deg, State:%s\n",
						MPU6050.roll , MPU6050.pitch, MPU6050.KalmanAngleX, MPU6050.KalmanAngleY, MPU6050.state);
		my_printf(buffer);
		start_y = 20;
		end_y = 0;
		 sprintf(buffer, "Roll: %.2f Deg\nPitch: %.2f Deg\nState: %s\n",
						MPU6050.KalmanAngleX, MPU6050.KalmanAngleY, MPU6050.state);
		OLED_update(buffer, start_y, &end_y, 1);
		start_y = end_y;

		sprintf(buffer, "TEL,temp=%.2f,pres=%.2f,hum=%.2f,roll=%.2f,pitch=%.2f,state=%s\n",
				BME280.Temperature, BME280.Pressure, BME280.Humidity, MPU6050.KalmanAngleX, MPU6050.KalmanAngleY, MPU6050.state);
		UARTCom_sendtoESP32(buffer);

		my_printf("\n");

		char CSVbuffer[1024];
		//Temperature, Pressure, Humidity, Alititude, Ax, Ay, Az, Gx, Gy, Gz, roll, pitch, KalmanX, KalmanY, state
		sprintf(CSVbuffer, "%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %s",
				BME280.Temperature, BME280.Pressure, BME280.Humidity, BME280.AltitudeP,
				MPU6050.Ax , MPU6050.Ay, MPU6050.Az , MPU6050.Gx, MPU6050.Gy , MPU6050.Gz,
				MPU6050.roll , MPU6050.pitch, MPU6050.KalmanAngleX, MPU6050.KalmanAngleY, MPU6050.state);

		SDCard_append_csv(CSVbuffer);

		HAL_Delay(1000); //500ms delay minimum for current transmit value
		break;

	  case STATE_STOP_REQUESTED:
		  // Close file
		  // Unmount SD
		  //for testing
		  LED_close();
		  BME280_close();
		  MPU6050_close();
		  UARTCom_close();
		  UARTCom_closeESP32();
		  OLED_close();
		  SDCard_close_log();
		  SDCard_close_csv();
		  state = STATE_STOPPED;
		  break;
	}
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void I2C_ClearBus(void) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);  // SDA
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);  // SCL
    for(uint8_t i = 0; i < 9; i++) {
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8);
        HAL_Delay(1);
    }
    //STOP
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
}


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
#ifdef USE_FULL_ASSERT
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
