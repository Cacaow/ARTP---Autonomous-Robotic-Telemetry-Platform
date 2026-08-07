/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : UARTCom.c
  * @brief          : UART DMA Normal Transmit and receive.
  * 				Code can be modified for IT and circular.
  * 				Receive uses DMA + Idle + Normal
  * 				Transmit uses DMA + Normal
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
#include "UARTCom.h"
#include "dma.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "util.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PD */
#define MAXLEN 4096
#define MAXTEMPLEN 256
#define MAXVALUES 40
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int enable_timer;
uint16_t timer;
int enable_timer_ESP32;
uint16_t timer_ESP32;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//TRANSMITTING CODE
uint8_t isSent = 1;
uint8_t sent_count = 0;
uint8_t tx_values[MAXVALUES][2048];

uint8_t isSent_ESP32 = 1;

//RECIEVING CODE
uint8_t isReceive = 1;
uint8_t receive_count = 0;
uint8_t rx_val[MAXLEN];
uint8_t rx_temp[MAXTEMPLEN];

uint8_t rx_val_ESP32[MAXLEN];
uint8_t rx_temp_ESP32[MAXTEMPLEN];

//can be used for DMA and IT
//Currently DMA Mode
uint16_t temp_index = 0;
uint16_t rx_index = 0;

uint16_t rx_index_ESP32 = 0;


//DMA Idle Receive Normal Mode
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
	HAL_StatusTypeDef status = HAL_OK;
	char temp_buffer[512];
	if (huart->Instance == huart2.Instance) {
		sprintf(temp_buffer, "Received buffer value = %s, Size of message=%d, Receive_count index=%d, Temporary Count Index=%d\n", rx_temp, Size, receive_count, temp_index);
		my_printf(temp_buffer);
		if (rx_temp[0] != 0) {

			for (int i = 0; i < Size; i++) {
				if (rx_temp[i] == '\n' || rx_temp[i] == '\r') {
					rx_val[rx_index] = 0;
					if (rx_val[0] != 0) {
						sprintf(temp_buffer, "Buffer reset, Received Value: %s\n", rx_val);
						my_printf(temp_buffer);
						/*add processed data*/
						rx_index = 0;
						rx_val[0] = 0;
					}
				}
				else {
					rx_val[rx_index] = rx_temp[i];
					rx_index++;
				}
			}
			//memcpy(rx_val + receive_count, rx_temp, Size);
			//receive_count += Size;
			//printf("Received value = %s, Total message size = %d, Temporary Count Index=%d\n", rx_val, receive_count, temp_index);
		}
		else {
			my_printf("ERROR - rx_temp[0] != 0\n");
		}
		enable_timer = 1;
		timer = 0;
		rx_temp[0] = 0;
		//call the function again
		status = HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_temp, MAXTEMPLEN);
		  if (status != HAL_OK) {
				sprintf(temp_buffer, "HAL_UARTEx_ReceiveToIdle_DMA Error - %d \n", status);
				my_printf(temp_buffer);
		  }
		  //Important to prevent splicing in the middle of a message (can lead to busy error code)
		  __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
	} else if (huart->Instance == huart1.Instance) {
		sprintf(temp_buffer, "Received buffer value = %s, Size of message=%d\n", rx_temp_ESP32, Size);
		my_printf(temp_buffer);
		if (rx_temp_ESP32[0] != 0) {

			for (int i = 0; i < Size; i++) {
				if (rx_temp_ESP32[i] == '\n' || rx_temp_ESP32[i] == '\r') {
					rx_val_ESP32[rx_index_ESP32] = 0;
					if (rx_val_ESP32[0] != 0) {
						sprintf(temp_buffer, "Buffer reset, Received Value: %s\n", rx_val_ESP32);
						my_printf(temp_buffer);
						/*add processed data*/
						rx_index_ESP32 = 0;
						rx_val_ESP32[0] = 0;
					}
				}
				else {
					rx_val_ESP32[rx_index_ESP32] = rx_temp_ESP32[i];
					rx_index_ESP32++;
				}
			}
		}
		else {
			my_printf("ERROR - rx_temp[0] != 0\n");
		}
		enable_timer = 1;
		timer = 0;
		rx_temp_ESP32[0] = 0;
		//call the function again
		status = HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_temp_ESP32, MAXTEMPLEN);
		  if (status != HAL_OK) {
				sprintf(temp_buffer, "HAL_UARTEx_ReceiveToIdle_DMA Error - %d \n", status);
				my_printf(temp_buffer);
		  }
		  //Important to prevent splicing in the middle of a message (can lead to busy error code)
		  __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
	}
}


//UART TRANSMITTING CODE
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == huart2.Instance) {
	  isSent = 1;
	  sent_count++;
	  char temp_buffer[512];
	  sprintf(temp_buffer, "sent %d\n", sent_count);
	  my_printf(temp_buffer);
	  if (sent_count > 10) {
		  //HAL_UART_DMAStop(&huart2);
	  }
	} else if (huart->Instance == huart1.Instance) {
		  isSent_ESP32 = 1;
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  /* USER CODE BEGIN 2 */


int UARTCom_init() {
	enable_timer = 0;
	timer = 0;
	isSent = 1;
	rx_val[0] = 0;
	return 0;
}


  //UART TRANSMITTING VALUES
int UARTCom_tx(uint8_t* tx_val) {
	  HAL_StatusTypeDef status = HAL_OK;
	  char temp_buffer[512];
	  /*
	  uint8_t tx_val[MAXLEN];

	  for (uint32_t i = 0; i < MAXLEN; i++) {
		tx_val[i] = 48;
	  }
	  */

	  if (isSent == 1) {
	    	status = HAL_UART_Transmit_DMA(&huart2, tx_val, strlen((const char*)tx_val));
	    	if (status != HAL_OK) {
	    			sprintf(temp_buffer, "HAL_UART_Transmit_DMA Error - %d", status);
	    			my_printf(temp_buffer);
	    			return status;
	    	}
	    	isSent = 0;
	  }

	  return 0;
}
  
  //RECEIVING VALUES
int UARTCom_rx() {
  HAL_StatusTypeDef status = HAL_OK;
  char temp_buffer[512];
  status = HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_temp, MAXTEMPLEN);
  if (status != HAL_OK) {
		sprintf(temp_buffer, "HAL_UARTEx_ReceiveToIdle_DMA Error - %d", status);
		my_printf(temp_buffer);
		return status;
  }
  __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);

  return 0;
}

int UARTCom_close() {
	HAL_UART_DMAStop(&huart2);
	return 0;
}

// ESP32 -> STM32

int UARTCom_initESP32() {
	enable_timer_ESP32 = 0;
	timer_ESP32 = 0;
	isSent_ESP32 = 1;
	rx_val_ESP32[0] = 0;
	return 0;
}

  //UART TRANSMITTING VALUES
int UARTCom_sendtoESP32(uint8_t* tx_val) {
	  HAL_StatusTypeDef status = HAL_OK;
	  char temp_buffer[512];

	  if (isSent_ESP32 == 1) {
	    	status = HAL_UART_Transmit_DMA(&huart1, tx_val, strlen((const char*)tx_val));
	    	if (status != HAL_OK) {
	    			sprintf(temp_buffer, "HAL_UART_Transmit_DMA Error - %d", status);
	    			my_printf(temp_buffer);
	    			return status;
	    	}
	    	isSent_ESP32 = 0;
	  }

	  return 0;
}

  //RECEIVING VALUES
int UARTCom_receivefromESP32() {
  HAL_StatusTypeDef status = HAL_OK;
  char temp_buffer[512];
  status = HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_temp_ESP32, MAXTEMPLEN);
  if (status != HAL_OK) {
		sprintf(temp_buffer, "HAL_UARTEx_ReceiveToIdle_DMA Error - %d", status);
		my_printf(temp_buffer);
		return status;
  }
  __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);

  return 0;
}

int UARTCom_closeESP32() {
	HAL_UART_DMAStop(&huart1);
	return 0;
}

  /* USER CODE END 2 */
