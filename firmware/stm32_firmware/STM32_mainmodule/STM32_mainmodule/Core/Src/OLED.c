/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : OLED.c
  * @brief          : OLED program body - configures the OLED and displays system status
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
#include "OLED.h"
#include "i2c.h"

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

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static int16_t scroll_x = 128;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int OLED_init(void)
{

  /* USER CODE BEGIN 1 */

	ssd1306_Init();
	//ssd1306_Fill(White);
	ssd1306_SetCursor(0, 0);
	int WriteString = ssd1306_WriteString("Hello OLED", Font_7x10, White);
	if (WriteString != 0) {
		printf("ERROR: OLED_init-String could not be written \n");
		//return -1;
	}
	ssd1306_UpdateScreen();

	return 0;
  /* USER CODE END 1 */
};



int OLED_update(char *txt) {
   /* USER CODE BEGIN 2 */
	//ssd1306_Fill(White);
	ssd1306_SetCursor(0, 20);
	int WriteString = ssd1306_WriteString(txt, Font_6x8, White);
	//can also be written as if (WriteString == '\0')
	if (WriteString != 0) {
			printf("ERROR: OLED_update-String could not be written \n");
			//return -1;
		}
	ssd1306_UpdateScreen();


	return 0;

  /* USER CODE END 2 */
}

int OLED_close(void) {
   /* USER CODE BEGIN 3 */
	ssd1306_SetCursor(10, 10);
	ssd1306_WriteString("", Font_11x18, White);
	ssd1306_UpdateScreen();

	return 0;
  /* USER CODE END 3 */
}

