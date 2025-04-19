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
#define KICKER_CHARGE_Pin GPIO_PIN_0
#define KICKER_CHARGE_GPIO_Port GPIOC
#define KICKER_KICK_Pin GPIO_PIN_1
#define KICKER_KICK_GPIO_Port GPIOC
#define VOLTAGE_Pin GPIO_PIN_4
#define VOLTAGE_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_5
#define LED1_GPIO_Port GPIOA
#define LED_H_Pin GPIO_PIN_6
#define LED_H_GPIO_Port GPIOA
#define HOLD_FRONT_Pin GPIO_PIN_4
#define HOLD_FRONT_GPIO_Port GPIOC
#define HOLD_BACK_Pin GPIO_PIN_5
#define HOLD_BACK_GPIO_Port GPIOC
#define MOTOR2B_Pin GPIO_PIN_2
#define MOTOR2B_GPIO_Port GPIOB
#define MOTOR2A_Pin GPIO_PIN_10
#define MOTOR2A_GPIO_Port GPIOB
#define MOTOR1B_Pin GPIO_PIN_14
#define MOTOR1B_GPIO_Port GPIOB
#define MOTOR1A_Pin GPIO_PIN_15
#define MOTOR1A_GPIO_Port GPIOB
#define MOTOR4A_Pin GPIO_PIN_6
#define MOTOR4A_GPIO_Port GPIOC
#define MOTOR4B_Pin GPIO_PIN_8
#define MOTOR4B_GPIO_Port GPIOC
#define MOTOR3B_Pin GPIO_PIN_3
#define MOTOR3B_GPIO_Port GPIOB
#define MOTOR3A_Pin GPIO_PIN_5
#define MOTOR3A_GPIO_Port GPIOB
#define FRONT_DRIBBLER_A_Pin GPIO_PIN_6
#define FRONT_DRIBBLER_A_GPIO_Port GPIOB
#define FRONT_DRIBBLER_B_Pin GPIO_PIN_7
#define FRONT_DRIBBLER_B_GPIO_Port GPIOB
#define BACK_DRIBBLER_A_Pin GPIO_PIN_8
#define BACK_DRIBBLER_A_GPIO_Port GPIOB
#define BACK_DRIBBLER_B_Pin GPIO_PIN_9
#define BACK_DRIBBLER_B_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
