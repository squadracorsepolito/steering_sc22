/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Push2_Pin GPIO_PIN_4
#define Push2_GPIO_Port GPIOE
#define Manettino1_Pin GPIO_PIN_0
#define Manettino1_GPIO_Port GPIOC
#define Manettino2_Pin GPIO_PIN_1
#define Manettino2_GPIO_Port GPIOC
#define Manettino3_Pin GPIO_PIN_2
#define Manettino3_GPIO_Port GPIOC
#define Push1_Pin GPIO_PIN_2
#define Push1_GPIO_Port GPIOF
#define Orange_LED_Pin GPIO_PIN_5
#define Orange_LED_GPIO_Port GPIOA
#define RST_Display_Pin GPIO_PIN_6
#define RST_Display_GPIO_Port GPIOA
#define Push4_Pin GPIO_PIN_11
#define Push4_GPIO_Port GPIOE
#define Push3_Pin GPIO_PIN_13
#define Push3_GPIO_Port GPIOE
#define Red_LED_Pin GPIO_PIN_10
#define Red_LED_GPIO_Port GPIOD
#define Green_LED_Pin GPIO_PIN_11
#define Green_LED_GPIO_Port GPIOD
#define Blue_LED_Pin GPIO_PIN_12
#define Blue_LED_GPIO_Port GPIOD
#define Out_LED_CMD_Pin GPIO_PIN_7
#define Out_LED_CMD_GPIO_Port GPIOC
#define Yellow_LED_Pin GPIO_PIN_12
#define Yellow_LED_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
