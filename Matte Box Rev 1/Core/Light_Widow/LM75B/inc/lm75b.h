/*  lm75b.h
 *	LM75B Digital Temperature Sensor Driver
 *
 *  Colton Crandell
 *  Light Widow 2020
 *
 * */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LM75B_H
#define LM75B_H


/******************************************************************************
 * INCLUDES
 *******************************************************************************/

#include <stdint.h>
#include "stm32f4xx_hal.h"
/******************************************************************************
 * DEFINES
 *******************************************************************************/

#define LM75B_I2C_ADDR			(0x48 << 1)
#define LM75B_I2C_TIMEOUT		1000

#define TEMP_REG				0x00			/* Temperature Register 16-bit Read Only 	*/
#define CONFIG_REG				0x01			/* Configuration Register, 8-bit R/W 		*/
#define THYST_REG				0x02			/* Hysteresis Register 16-bit R/W		 	*/
#define TOS_REG					0x03			/* Overtemp Shutdown Register 16-bit R/W 	*/

/******************************************************************************
 * TYPES
 *******************************************************************************/
typedef enum {
	LM75B_ERR,
	LM75B_OK,
} LM75B_error;

/******************************************************************************
 * PROTOTYPES
 *******************************************************************************/

LM75B_error LM75B_Init(I2C_HandleTypeDef *hi2c, GPIO_TypeDef *Power_Port, uint16_t Power_Pin);

int8_t LM75B_ReadTemp(void);


#endif // LM75B_H
