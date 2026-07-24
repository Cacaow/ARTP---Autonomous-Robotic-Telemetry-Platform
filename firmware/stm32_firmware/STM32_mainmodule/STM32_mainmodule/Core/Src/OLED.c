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
#include "util.h"

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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */
int enable_OLED = 1;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int OLED_init(void)
{
  /* USER CODE BEGIN 1 */
	if (enable_OLED == 0) {
		return 0;
	}

	ssd1306_Init();
	//ssd1306_Fill(White);
	ssd1306_SetCursor(0, 0);
	int WriteString = ssd1306_WriteString("ARTP", Font_11x18, White);
	ssd1306_SetCursor(64, 8);
	WriteString = ssd1306_WriteString("INIT", Font_6x8, White);
	if (WriteString != 0) {
		my_printf("ERROR: OLED_init-String could not be written \n");
		//return -1;
	}

	//ssd1306_scroll_blue_right(0x03);

	ssd1306_UpdateScreen();

	return 0;
  /* USER CODE END 1 */
};



int OLED_update(char *txt, int start_y, int *end_y, int clear) {
   /* USER CODE BEGIN 2 */
	if (enable_OLED == 0) {
		return 0;
	}
	if (clear) {
		ssd1306_FillRectangle(0, 0, 128, 64, Black);
	}

	ssd1306_SetCursor(0, 0);
	int WriteString = ssd1306_WriteString("ARTP", Font_11x18, White);
	ssd1306_SetCursor(64, 8);
	WriteString = ssd1306_WriteString("RUNNING", Font_6x8, White);


	//ssd1306_Fill(White);
	char *delimiter = "\n";

	char *token = strtok(txt, delimiter);
	int y_val = start_y;

	while (token != NULL) {
		ssd1306_SetCursor(0, y_val);
		WriteString = ssd1306_WriteString(token, Font_6x8, White);
		//can also be written as if (WriteString == '\0')
		if (WriteString == -1) {
			my_printf("ERROR: OLED invalid string\n");
			//return -1;
		}
		else if (WriteString == -2) {
			my_printf("ERROR: OLED string out of screen\n");
		}
		else if (WriteString != 0) {
			my_printf("ERROR: Unknown error\n");
		}
		y_val += 10;
		token = strtok(NULL, delimiter);
	}

	*end_y = y_val;
	ssd1306_UpdateScreen();

	return 0;

  /* USER CODE END 2 */
}

int OLED_close(void) {
   /* USER CODE BEGIN 3 */
	if (enable_OLED == 0) {
		return 0;
	}
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString("ARTP", Font_11x18, White);
	ssd1306_SetCursor(64, 8);
	ssd1306_WriteString("STOPPED", Font_6x8, White);
	ssd1306_UpdateScreen();
	ssd1306_stop_scroll();

	return 0;
  /* USER CODE END 3 */
}



