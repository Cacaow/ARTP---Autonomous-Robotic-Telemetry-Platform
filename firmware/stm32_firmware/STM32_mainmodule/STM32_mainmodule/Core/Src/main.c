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

BME280_Data_t BME280;
MPU6050_Data_t MPU6050;
int start_y = 20;
int end_y = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void I2C_ClearBus(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void SD_Card_Test(void);
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


void I2C_Scanner(void) {
    HAL_StatusTypeDef result;
    uint8_t devices_found = 0;

    printf("\r\n--- Starting I2C Bus Scan ---\r\n");

    for (uint16_t i = 1; i < 128; i++) {
        /* * HAL_I2C_IsDeviceReady expects the left-shifted 8-bit address.
         * It tries to communicate with the device 3 times with a 5ms timeout.
         */
        result = HAL_I2C_IsDeviceReady(&hi2c1, (i << 1), 3, 5);

        if (result == HAL_OK) {
            printf("Device found at 7-bit address: 0x%02X (8-bit Addr: 0x%02X)\r\n", i, (i << 1));
            devices_found++;
        }
    }

    if (devices_found == 0) {
        printf("No I2C devices found. Check wiring, power, and pull-up resistors.\r\n");
    } else {
        printf("Scan complete. Found %d device(s).\r\n", devices_found);
    }
    printf("-----------------------------\r\n\r\n");
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
  /* USER CODE BEGIN 2 */
  I2C_ClearBus();
  I2C_Scanner();
	
  SD_Card_Test();
  LED_init();
  UARTCom_init();
  BME280_init();
  MPU6050_init();
  MPU6050_Calibrate(&MPU6050);
  OLED_init();
  
  printf("\n");


  //RECEIVING VALUES
  //UARTCom_rx();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  //make sure all transmitted values have \r\n endings
	  sprintf(buffer, "Hello World count = %d, Timer = %d\r\n", count, timer);
	  printf(buffer);

	  //SENDING VALUES
	  //UARTCom_tx((uint8_t*)buffer);

	  //Timer for UART receive timeout
	  if (timer >= 1000) {
		  //entire data has been received
		  timer = 0;
		  enable_timer = 0;
	  }

	LED_blink();
    count++;

    BME280_calculation(&BME280);
    sprintf(buffer, "BME280: Temp: %.2f C, Pres: %.2f hPa, Humi: %.2f RH, Alti: %.2f m\n",
    				BME280.Temperature, BME280.Pressure, BME280.Humidity, BME280.AltitudeP);
    printf(buffer);
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
    printf(buffer);
    sprintf(buffer, "MPU6050: Roll: %.2f Deg, Pitch: %.2f Deg, KalmanX: %.2f Deg, KalmanY: %.2f Deg, State:%s\n",
    				MPU6050.roll , MPU6050.pitch, MPU6050.KalmanAngleX, MPU6050.KalmanAngleY, MPU6050.state);
	printf(buffer);
	start_y = 20;
	end_y = 0;
    sprintf(buffer, "Roll: %.2f Deg\nPitch: %.2f Deg\nState: %s\n",
    				MPU6050.KalmanAngleX, MPU6050.KalmanAngleY, MPU6050.state);
	OLED_update(buffer, start_y, &end_y, 1);
	start_y = end_y;

	printf("\n");
    HAL_Delay(1000); //500ms delay minimum for current transmit value

  }
  LED_close();
  BME280_close();
  MPU6050_close();
  OLED_close();
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
void SD_Card_Test(void)
{
	printf("Start FATFS System\n");
    FATFS FatFs;        // FAT file system object (required by FatFs)
    FIL Fil;            // File object (used to open/read/write files)
    FRESULT FR_Status;  // Stores return status of FatFs functions
    FATFS *FS_Ptr;      // Pointer to file system information
    UINT RWC, WWC;      // RWC = Read byte count, WWC = Written byte count
    DWORD FreeClusters; // Stores number of free clusters in SD card
    uint32_t TotalSize, FreeSpace; // SD card size information
    char RW_Buffer[200]; // Buffer used for both read and write operations

    //------------------[ STEP 1: Mount the SD Card ]--------------------
    // This step makes the SD card ready to use with FatFs
    FR_Status = f_mount(&FatFs, "", 1);
    if (FR_Status != FR_OK)
    {
        // If mounting fails, print error and exit function
        printf("[ERROR] SD Card mount failed! Code = %d\r\n", FR_Status);
        return;
    }

    // SD card mounted successfully
    printf("[INFO] SD Card mounted successfully\r\n\n");

    //------------------[ STEP 2: Get SD Card Size & Free Space ]--------------------
    // Get free cluster count and file system information
    f_getfree("", &FreeClusters, &FS_Ptr);

    // Calculate total size and free space in Bytes
    // csize = cluster size, each sector = 512 bytes
    TotalSize = (FS_Ptr->n_fatent - 2) * FS_Ptr->csize * 512;
    FreeSpace = FreeClusters * FS_Ptr->csize * 512;

    // Print SD card memory details on serial terminal
    printf("[INFO] SD Card Total Size : %lu Bytes\r\n", TotalSize);
    printf("[INFO] SD Card Free Space : %lu Bytes\r\n\n", FreeSpace);

    //------------------[ STEP 3: Create File and Write Data ]--------------------
    // Create a new file or overwrite if it already exists
    FR_Status = f_open(&Fil,
                       "STM_FILE.txt",
                       FA_WRITE | FA_READ | FA_CREATE_ALWAYS);

    if (FR_Status != FR_OK)
    {
        // File creation/open failed
        printf("[ERROR] File create/open failed! Code = %d\r\n", FR_Status);
        return;
    }

    // File created successfully
    printf("[SD] File created: STM_FILE.txt\r\n");

    // Prepare data to be written into the SD card file
    strcpy(RW_Buffer, "[SD] Hello from STM32! Data written using f_write()\r\n");

    // Write data to the file
    FR_Status = f_write(&Fil, RW_Buffer, strlen(RW_Buffer), &WWC);
    if (FR_Status != FR_OK || WWC == 0)
    {
        // Writing failed
        printf("[ERROR] SD file write failed\r\n");
        f_close(&Fil);
        return;
    }

    // Print number of bytes written
    printf("[INFO] Written string to SD card: %s\r\n", RW_Buffer);
    printf("[INFO] Written %d bytes to SD card\r\n", WWC);

    // Close the file after writing
    f_close(&Fil);

    //------------------[ STEP 4: Read File Content ]--------------------
    // Open the same file in read mode
    FR_Status = f_open(&Fil, "STM_FILE.txt", FA_READ);
    if (FR_Status != FR_OK)
    {
        printf("[ERROR] File open for read failed\r\n");
        return;
    }

    // Clear buffer before reading data
    memset(RW_Buffer, 0, sizeof(RW_Buffer));

    // Move file pointer to the beginning of the file
    f_lseek(&Fil, 0);

    // Read data from SD card file into buffer
    FR_Status = f_read(&Fil, RW_Buffer, sizeof(RW_Buffer) - 1, &RWC);
    if (FR_Status == FR_OK)
    {
        // Add null character to make it a valid string
        RW_Buffer[RWC] = '\0';

        // Print file content on serial terminal
        printf("[SD] File content read from SD card:\r\n%s\r\n", RW_Buffer);
    }

    // Close file after reading
    f_close(&Fil);

    //------------------[ STEP 5: Append New Data to File ]--------------------
    // Open existing file in write mode (for updating)
    FR_Status = f_open(&Fil,
                       "STM_FILE.txt",
                       FA_OPEN_EXISTING | FA_WRITE);
    if (FR_Status != FR_OK)
    {
        printf("[ERROR] File open for update failed\r\n");
        return;
    }

    // Move file pointer to the end of the file
    // This ensures new data is appended, not overwritten
    f_lseek(&Fil, f_size(&Fil));

    // New data to be appended at the end of file
    strcpy(RW_Buffer,
           "[SD] This line was appended later\r\n");

    // Write appended data
    f_write(&Fil, RW_Buffer, strlen(RW_Buffer), &WWC);
    printf("[SD] Appended new data to file\r\n");

    // Close file after updating
    f_close(&Fil);

    //------------------[ STEP 6: Read File After Update ]--------------------
    // Open file again to verify updated content
    FR_Status = f_open(&Fil, "STM_FILE.txt", FA_READ);
    if (FR_Status == FR_OK)
    {
        memset(RW_Buffer, 0, sizeof(RW_Buffer));

        // Read entire file content
        f_read(&Fil, RW_Buffer, sizeof(RW_Buffer) - 1, &RWC);
        RW_Buffer[RWC] = '\0';

        // Print updated file content
        printf("[SD] File content after update:\r\n%s\r\n", RW_Buffer);

        f_close(&Fil);
    }

    //------------------[ STEP 7: Unmount the SD Card ]--------------------
    // Properly unmount the SD card to avoid data corruption
    FR_Status = f_mount(NULL, "", 0);
    if (FR_Status != FR_OK)
        printf("[ERROR] SD Card unmount failed! Code = %d\r\n", FR_Status);
    else
        printf("[INFO] SD Card unmounted safely\r\n");
}

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
