/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#define EPD_POWER_Pin GPIO_PIN_0
#define EPD_POWER_GPIO_Port GPIOC
#define EPD_SPI_CS_Pin GPIO_PIN_1
#define EPD_SPI_CS_GPIO_Port GPIOC
#define EPD_SPI_MISO_Pin GPIO_PIN_2
#define EPD_SPI_MISO_GPIO_Port GPIOC
#define EPD_SPI_MOSI_Pin GPIO_PIN_3
#define EPD_SPI_MOSI_GPIO_Port GPIOC
#define EPD_DISCHARGE_Pin GPIO_PIN_0
#define EPD_DISCHARGE_GPIO_Port GPIOA
#define EPD_NRST_Pin GPIO_PIN_1
#define EPD_NRST_GPIO_Port GPIOA
#define EPD_BUSY_Pin GPIO_PIN_2
#define EPD_BUSY_GPIO_Port GPIOA
#define RFID_POWER_Pin GPIO_PIN_3
#define RFID_POWER_GPIO_Port GPIOA
#define RF_SPI_BSS_Pin GPIO_PIN_4
#define RF_SPI_BSS_GPIO_Port GPIOA
#define RF_SPI_SCLK_Pin GPIO_PIN_5
#define RF_SPI_SCLK_GPIO_Port GPIOA
#define RF_SPI_MISO_Pin GPIO_PIN_6
#define RF_SPI_MISO_GPIO_Port GPIOA
#define RF_SPI_MOSI_Pin GPIO_PIN_7
#define RF_SPI_MOSI_GPIO_Port GPIOA
#define RF_IRQ_Pin GPIO_PIN_4
#define RF_IRQ_GPIO_Port GPIOC
#define RF_IRQ_EXTI_IRQn EXTI4_IRQn
#define LED1_Pin GPIO_PIN_0
#define LED1_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_1
#define LED2_GPIO_Port GPIOB
#define B_INPUT1_Pin GPIO_PIN_12
#define B_INPUT1_GPIO_Port GPIOB
#define B_INPUT1_EXTI_IRQn EXTI15_10_IRQn
#define B_INPUT2_Pin GPIO_PIN_13
#define B_INPUT2_GPIO_Port GPIOB
#define B_INPUT2_EXTI_IRQn EXTI15_10_IRQn
#define B_INPUT3_Pin GPIO_PIN_14
#define B_INPUT3_GPIO_Port GPIOB
#define B_INPUT3_EXTI_IRQn EXTI15_10_IRQn
#define SDIO_POWER_Pin GPIO_PIN_6
#define SDIO_POWER_GPIO_Port GPIOC
#define SD_DETECT_Pin GPIO_PIN_7
#define SD_DETECT_GPIO_Port GPIOC
#define SD_DETECT_EXTI_IRQn EXTI9_5_IRQn
#define USB_RENUM_Pin GPIO_PIN_8
#define USB_RENUM_GPIO_Port GPIOA
#define DBG_OUT_TX_Pin GPIO_PIN_9
#define DBG_OUT_TX_GPIO_Port GPIOA
#define DBG_OUT_RX_Pin GPIO_PIN_10
#define DBG_OUT_RX_GPIO_Port GPIOA
#define TC_SIGNAL_Pin GPIO_PIN_4
#define TC_SIGNAL_GPIO_Port GPIOB
#define AUX_POWER_Pin GPIO_PIN_5
#define AUX_POWER_GPIO_Port GPIOB
#define AUX_I2C_SCL_Pin GPIO_PIN_6
#define AUX_I2C_SCL_GPIO_Port GPIOB
#define AUX_I2C_SDA_Pin GPIO_PIN_7
#define AUX_I2C_SDA_GPIO_Port GPIOB
#define TEMP_IRQ_Pin GPIO_PIN_8
#define TEMP_IRQ_GPIO_Port GPIOB
#define TEMP_IRQ_EXTI_IRQn EXTI9_5_IRQn
#define LOW_BAT_IRQ_Pin GPIO_PIN_9
#define LOW_BAT_IRQ_GPIO_Port GPIOB
#define LOW_BAT_IRQ_EXTI_IRQn EXTI9_5_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
