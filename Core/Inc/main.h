/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef enum menuLayer
{
	NUMERIC_PARAMETER_ROOT,
	NUMERIC_PARAMETER_VALUE_SELECTED,
	COLOR_PALETTE_ROOT,
	HSV_PICKER_ROOT,
	HSV_PICKER_VALUE_SELECTED,
	MOD_MATRIX_ROOT,
	MOD_MATRIX_DESTINATION_SELECTED,
	MOD_MATRIX_AMOUNT_SELECTED
} menuLayer;
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
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define B1_EXTI_IRQn EXTI15_10_IRQn
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_5
#define LED1_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_6
#define LED2_GPIO_Port GPIOA
#define LED3_Pin GPIO_PIN_7
#define LED3_GPIO_Port GPIOA
#define BACK_BTN_Pin GPIO_PIN_7
#define BACK_BTN_GPIO_Port GPIOC
#define BACK_BTN_EXTI_IRQn EXTI9_5_IRQn
#define FX_CHANGE_BTN_Pin GPIO_PIN_10
#define FX_CHANGE_BTN_GPIO_Port GPIOA
#define FX_CHANGE_BTN_EXTI_IRQn EXTI15_10_IRQn
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define ENC_BTN_Pin GPIO_PIN_4
#define ENC_BTN_GPIO_Port GPIOB
#define MOD_BTN_Pin GPIO_PIN_5
#define MOD_BTN_GPIO_Port GPIOB
#define MOD_BTN_EXTI_IRQn EXTI9_5_IRQn
#define LED4_Pin GPIO_PIN_6
#define LED4_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define NUM_CV_INPUTS 3
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
