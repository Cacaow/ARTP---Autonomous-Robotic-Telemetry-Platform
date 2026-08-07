/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : UARTCom.h
  * @brief          : Header for UARTCom.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UARTCOM_H
#define __UARTCOM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


/* USER CODE END Includes */
/* USER CODE BEGIN ET */

/* USER CODE END ET */
/* USER CODE BEGIN EC */
int UARTCom_init();
int UARTCom_tx(uint8_t* tx_val);
int UARTCom_rx();
int UARTCom_close();

int UARTCom_initESP32();
int UARTCom_sendtoESP32(uint8_t* tx_val);
int UARTCom_receivefromESP32();
int UARTCom_closeESP32();

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */


#ifdef __cplusplus
}
#endif

#endif /* __UARTCOM_H */
