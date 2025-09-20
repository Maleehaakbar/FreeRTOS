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
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include <stdio.h>
#include <string.h>

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
#define NUM_TIMERS 2
extern QueueHandle_t xQueue1, xQueue2;  //queue 1 for receiving user data, queue 2 for print task
extern UART_HandleTypeDef huart2;
extern RTC_HandleTypeDef hrtc;

extern TaskHandle_t handle_menu_task;
extern TaskHandle_t handle_cmd_task ;
extern TaskHandle_t handle_print_task ;
extern TaskHandle_t handle_LED_task;
extern TaskHandle_t handle_RTC_task;

extern TimerHandle_t xTimers[ NUM_TIMERS ];

typedef struct cmd
{
	uint8_t payload[10];
	uint32_t len;
}command_t;

typedef enum
{
	MainMenu = 0,
	Led_state,
	RtcMenu,
  RtcTimeConfig,
	RtcReport,
}state_t;

extern state_t curr_state;

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
void menu_task(void *parameters);
void cmd_task(void *parameters);
void print_task(void *parameters);
void LED_task(void *parameters);
void RTC_task(void *parameters);

void LED_control_1();
void LED_control_2();
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define HOUR_CONFIG     1
#define MINUTES_CONFIG  2
#define SECONDS_CONFIG  3

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
