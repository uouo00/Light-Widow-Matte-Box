/*  lm75b.c
 *	LM75B Digital Temperature Sensor Driver
 *
 *  Colton Crandell
 *  Light Widow 2020
 *
 * */


/******************************************************************************
 * INCLUDES
 *******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include "lm75b.h"
#include "stm32f4xx_hal.h"

/******************************************************************************
 * Private Variables
 *******************************************************************************/
GPIO_TypeDef *LM75BPowerPort;		// Port for the Auxiliary Components power enable
uint16_t LM75BPowerPin;				// Pin for the Auxiliary Components power enable
I2C_HandleTypeDef *pi2c = 0;		// I2C Handle
bool LM75BInitialized = false;		// Initialized Flag

/******************************************************************************
 * PRIVATE PROTOTYPES
 *******************************************************************************/
uint8_t readConfigReg(uint8_t *confReg);

LM75B_error LM75B_i2cReadBytes(uint8_t subAddr, uint8_t *dest, uint8_t Size);
LM75B_error LM75B_i2cWriteBytes(uint8_t subAddr, uint8_t *src, uint8_t Size);

/********************************************************************************
 * PUBLIC FUNCTIONS
 *******************************************************************************/
LM75B_error LM75B_Init(I2C_HandleTypeDef *hi2c, GPIO_TypeDef *Power_Port, uint16_t Power_Pin) {
	// Setup the Power and Comms
	pi2c = hi2c;
	LM75BPowerPort = Power_Port;
	LM75BPowerPin = Power_Pin;

	HAL_GPIO_WritePin(LM75BPowerPort, LM75BPowerPin, GPIO_PIN_SET);
	HAL_Delay(1); // Small delay to ensure stable power

	// Read the configuration register and see if I2C Returns an issue
	uint8_t tempConfig;
	LM75B_error status = readConfigReg(&tempConfig);
	HAL_GPIO_WritePin(LM75BPowerPort, LM75BPowerPin, GPIO_PIN_RESET);

	if (status != LM75B_OK) {
		return LM75B_ERR;
	}

	if (tempConfig == 0x00){
		LM75BInitialized = true;
		return LM75B_OK;
	}

	return LM75B_ERR;
}

int8_t LM75B_ReadTemp(void) {
	if (!LM75BInitialized) {
		return LM75B_ERR;
	}

	HAL_GPIO_WritePin(LM75BPowerPort, LM75BPowerPin, GPIO_PIN_SET);
	HAL_Delay(1); // Small delay to ensure stable power

	// Read the temperature register
	uint8_t tempReg[2] = {0};
	LM75B_i2cReadBytes(TEMP_REG, tempReg, 2);

	// Shutdown power to conserve
	HAL_GPIO_WritePin(LM75BPowerPort, LM75BPowerPin, GPIO_PIN_RESET);

	// Convert Array into signed integer
	int16_t temperature = (tempReg[0] << 8) | tempReg[1];

	temperature = temperature >> 5;		// The temperature only uses the 11 most significant bits.
	temperature *= 0.125;				// 0.125 deg C Resolution

	// Check to see if temp is Negative
	int8_t checktemp = (uint8_t) temperature;

	if (checktemp & (1 << 7)) {
		// We are negative, Strip the negative qualifier and send result
		return (checktemp & (0x7F)) * -1;
	} else {
		return checktemp;
	}
}

/********************************************************************************
 * PRIVATE FUNCTIONS
 *******************************************************************************/
LM75B_error readConfigReg(uint8_t *confReg) {
	// Reads and returns the configuration register
	return LM75B_i2cReadBytes(CONFIG_REG, confReg, 1);
}


/********************************************************************************
 * I2C READ AND WRITE ROUTINES
 *******************************************************************************/

LM75B_error LM75B_i2cReadBytes(uint8_t subAddr, uint8_t *dest, uint8_t Size) {
	HAL_StatusTypeDef status = HAL_I2C_Mem_Read(pi2c, LM75B_I2C_ADDR, subAddr, I2C_MEMADD_SIZE_8BIT, dest, Size, LM75B_I2C_TIMEOUT);
	if (status != HAL_OK){
		return LM75B_ERR;
	}
	return LM75B_OK;
}

LM75B_error LM75B_i2cWriteBytes(uint8_t subAddr, uint8_t *src, uint8_t Size) {
	HAL_StatusTypeDef status = HAL_I2C_Mem_Write(pi2c, LM75B_I2C_ADDR, subAddr, I2C_MEMADD_SIZE_8BIT, src, Size, LM75B_I2C_TIMEOUT);
	if (status != HAL_OK){
		return LM75B_ERR;
	}
	return LM75B_OK;
}

