/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : LED.c
  * @brief          : LED program body - configures the LED and changes LED state
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
#include "LED.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
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

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


/* USER CODE END 0 */

int LED_init(void)
{
  /* USER CODE BEGIN 1 */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);

	return 0;
  /* USER CODE END 1 */
};


int LED_blink(void) {
   /* USER CODE BEGIN 2 */
	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

	return 0;
  /* USER CODE END 2 */
}

int LED_on(void) {
   /* USER CODE BEGIN 3 */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);

	return 0;
  /* USER CODE END 3 */
}

int LED_off(void) {
   /* USER CODE BEGIN 4 */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

	return 0;
  /* USER CODE END 4 */
}


int LED_close(void) {
   /* USER CODE BEGIN 5 */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	return 0;
  /* USER CODE END 5 */
}
