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
	SIMPLE_PARAMETER_ROOT,
	SIMPLE_PARAMETER_VALUE_SELECTED,
	COLOR_PALETTE_ROOT,
	HSV_PICKER_ROOT,
	HSV_PICKER_VALUE_SELECTED,
	MOD_MATRIX_ROOT,
	MOD_MATRIX_DESTINATION_SELECTED,
	MOD_MATRIX_AMOUNT_SELECTED,
	SETTINGS_ROOT,
	SETTINGS_VALUE_SELECTED
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
#define CV_IN5_Pin GPIO_PIN_0
#define CV_IN5_GPIO_Port GPIOC
#define CV_IN4_Pin GPIO_PIN_1
#define CV_IN4_GPIO_Port GPIOC
#define CV_IN7_Pin GPIO_PIN_2
#define CV_IN7_GPIO_Port GPIOC
#define CV_IN6_Pin GPIO_PIN_3
#define CV_IN6_GPIO_Port GPIOC
#define CV_IN0_Pin GPIO_PIN_0
#define CV_IN0_GPIO_Port GPIOA
#define CV_IN1_Pin GPIO_PIN_1
#define CV_IN1_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define CV_IN2_Pin GPIO_PIN_4
#define CV_IN2_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_5
#define LED1_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_6
#define LED2_GPIO_Port GPIOA
#define BACK_BTN_Pin GPIO_PIN_7
#define BACK_BTN_GPIO_Port GPIOA
#define BACK_BTN_EXTI_IRQn EXTI9_5_IRQn
#define CV_IN3_Pin GPIO_PIN_4
#define CV_IN3_GPIO_Port GPIOC
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
#define SETTINGS_BTN_Pin GPIO_PIN_6
#define SETTINGS_BTN_GPIO_Port GPIOB
#define SETTINGS_BTN_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */
#define NUM_CV_INPUTS 8
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
