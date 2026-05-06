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
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"

#include "ssd1306.h"
#include "ssd1306_fonts.h"
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void logging(char *msg) {
  FILE *fptr;
  fptr = fopen("D:\\error_log.txt", "a+");
  if (fptr == NULL) {
    printf("ERROR: Could not open file.\n");
    return;
  }

  fprintf(fptr, "ERROR %s\n", msg);

  fclose(fptr);

}



int _write(int file, char *ptr, int len) {
	int i=0;
	for (i = 0; i < len; i++) {
		ITM_SendChar((*ptr++));
	}
	return len;
}

uint8_t count = 0;

//RECIEVING CODE

uint8_t isReceive = 1;
uint8_t receive_count = 0;

//RECEIVING VALUES
static uint8_t rx_val[MAXLEN];
uint8_t rx_temp[MAXTEMPLEN];


//can be used for DMA and IT
//Currently DMA Mode
uint16_t temp_index = 0;
uint16_t rx_index = 0;

int enable_timer = 0;
uint16_t timer = 0;
/*
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
	printf("Received buffer value = %s, Size of message=%d, Receive_count index=%d, Temporary Count Index=%d\n", rx_temp, Size, receive_count, temp_index);

	memcpy(rx_val + receive_count, rx_temp + temp_index, Size - temp_index);

	if (Size == MAXTEMPLEN) {
		//DMA wrapped, reset rx_temp
		rx_index++;
		receive_count = MAXTEMPLEN * rx_index;
		temp_index = 0;
	} else {
		//rx_temp not full
		receive_count += (Size - temp_index);
		temp_index = Size;
	}
	printf("Received value = %s, Total message size = %d, Temporary Count Index=%d\n", rx_val, receive_count, temp_index);
	enable_timer = 1;
	timer = 0;
}
*/

//DMA Idle Receive Normal Mode
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
	HAL_StatusTypeDef status = HAL_OK;
	printf("Received buffer value = %s, Size of message=%d, Receive_count index=%d, Temporary Count Index=%d\n", rx_temp, Size, receive_count, temp_index);
	if (rx_temp[0] != 0) {
		memcpy(rx_val + receive_count, rx_temp, Size);

		receive_count += Size;
		printf("Received value = %s, Total message size = %d, Temporary Count Index=%d\n", rx_val, receive_count, temp_index);
	}
	else {
		printf("ERROR - rx_temp[0] != 0\n");
	}
	enable_timer = 1;
	timer = 0;
	rx_temp[0] = 0;
	//call the function again
	status = HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_temp, MAXTEMPLEN);
	  if (status != HAL_OK) {
			printf("HAL_UARTEx_ReceiveToIdle_DMA Error - %d \n", status);
			char temp_buf[256];
	  }
	  //Important to prevent splicing in the middle of a message (can lead to busy error code)
	  __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);

}

//SENDING CODE

uint8_t isSent = 1;
uint8_t sent_count = 0;

//can be used for DMA and IT
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
  isSent = 1;
  sent_count++;
  printf("sent %d\n", sent_count);
  if (sent_count > 10) {
	  HAL_UART_DMAStop(&huart2);
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
  /* USER CODE BEGIN 2 */
  ssd1306_Init();
  //ssd1306_Fill(White);
  ssd1306_SetCursor(10, 10);
  ssd1306_WriteString("Hello OLED", Font_11x18, White);
  ssd1306_UpdateScreen();


  rx_val[0] = 0;


  //SENDING VALUES
  uint8_t val[MAXLEN];

  for (uint32_t i = 0; i < MAXLEN; i++) {
    val[i] = 48;
  }
  
  HAL_StatusTypeDef status = HAL_OK;
  	//DMA in Circular mode
  {
	status = HAL_UART_Transmit_DMA(&huart2, val, MAXLEN);
	if (status != HAL_OK)
	{
		printf("HAL_Tranmit_Error - %d", status);
		char temp_buf[256];
		sprintf(temp_buf, "HAL_Tranmit_Error - %d", status);
		logging(temp_buf);
	}

  }

  //RECEIVING VALUES
  status = HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_temp, MAXTEMPLEN);
  if (status != HAL_OK) {
		printf("HAL_UARTEx_ReceiveToIdle_DMA Error - %d", status);
		char temp_buf[256];
  }

  __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  if (timer >= 1000) {
		  //entire data has been received
		  timer = 0;
		  enable_timer = 0;
	  }

	//RECEIVING VALUES



/*
	if (isReceive == 1) {
		rx_temp[0] = 0;
		//HAL_UART_Receive_IT(&huart2, temp, 1);
		status = HAL_UART_Receive_DMA(&huart2, rx_temp, 1);
		if (status != HAL_OK) {
			printf("HAL_UART_RECEIVE_ERROR %d\n", status );
			char temp_buf[256];
			sprintf(temp_buf, "HAL_Recieve_Error - %d", status);
			logging(temp_buf);
		}
		else if (rx_temp[0] != 0) {
			isReceive = 0;
		}
	}
*/
	//SENDING VALUES
    //HAL_UART_Transmit(&huart2, val, MAXLEN, HAL_MAX_DELAY);

	/*
	//DMA in Normal Mode

    if (isSent == 1) {
      //HAL_UART_Transmit_IT(&huart2, val, maxlen);
    	HAL_UART_Transmit_DMA(&huart2, val, maxlen);
    	isSent = 0;
    }
    */

    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    count++;
    printf("Hello World count = %d\n", count);
    HAL_Delay(500); //500mx delay

    

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
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
