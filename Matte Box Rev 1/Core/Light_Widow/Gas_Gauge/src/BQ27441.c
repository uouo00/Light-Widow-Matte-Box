/*  BQ27441.c
 *	BQ27441 Battery Fuel Gauge Main Source File
 *
 *  Colton Crandell
 *  Light Widow 2020
 *
 *  Original Code from Sparkfun Electronics and Modified for use in Light Widow Matte Boxes
 *  https://github.com/sparkfun/SparkFun_BQ27441_Arduino_Library
 *
 * */

/********************************************************************************
 * Includes
 *******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "BQ27441.h"
#include "BQ27441_Definitions.h"

#include "stm32f4xx_hal.h"


/********************************************************************************
 * Private Defines
 *******************************************************************************/

#define BAT_CAPACITY 400 				/* Battery Capacity - 400mAh Default	*/

/********************************************************************************
 * Private Variables
 *******************************************************************************/
BQ27441_HandleTypeDef lipo;				/* Lipo Gas Gauge					    */
bool FuelGaugeInitialized = false;		/* Initialization Flag					*/

/********************************************************************************
 * Private Prototypes
 *******************************************************************************/

/* Control Sub-Commands */

bool enterConfig(bool userControl);
bool exitConfig(bool resim);
uint16_t flags(void);
uint16_t status(void);

/* Private Functions */

bool sealed(void);
bool seal(void);
bool unseal(void);

bool softReset(void);
uint16_t readWord(uint16_t subAddress);
uint16_t readControlWord(uint16_t function);
bool executeControlWord(uint16_t function);

/* Extended Data Commands */

bool blockDataControl(void);
bool blockDataClass(uint8_t id);
bool blockDataOffset(uint8_t offset);
uint8_t blockDataChecksum(void);
uint8_t readBlockData(uint8_t offset);
bool writeBlockData(uint8_t offset, uint8_t data);
uint8_t computeBlockChecksum(void);
bool writeBlockChecksum(uint8_t csum);

BQ27441_error writeExtendedData(uint8_t classID, uint8_t offset, uint8_t *data, uint8_t len);


/* I2C Read and Write Routines */

void i2cReadBytes(uint8_t subAddr, uint8_t *dest, uint8_t Size);
void i2cWriteBytes(uint8_t subAddr, uint8_t *src, uint8_t Size);


/********************************************************************************
 * Initialization Functions
 *******************************************************************************/

// Initializes I2C and verifies communication with the BQ27441.
void BQ27441_Init(I2C_HandleTypeDef *i2c) {

	// Hard Reset
	executeControlWord(BQ27441_CONTROL_RESET);

	// Variable Initialization
	lipo.deviceInitialized = false;
	lipo.i2c = i2c;
	lipo.status = BQ27441_OK;
	lipo.userConfigControl = false;
	lipo.sealFlag = true;

	lipo.deviceType = readControlWord(BQ27441_CONTROL_DEVICE_TYPE);
	lipo.firmwareVer = readControlWord(BQ27441_CONTROL_FW_VERSION);
	lipo.dataMemCode = (uint8_t) readControlWord(BQ27441_CONTROL_DM_CODE); // Only LSB

	if (lipo.deviceType == BQ27441_DEVICE_ID) {
		lipo.deviceInitialized = true;
		FuelGaugeInitialized = true;
	} else {
		lipo.status = BQ27441_ERR;
	}
}

// Setup the Gauge to interrupt the host when battery cap goes below a threshold
BQ27441_error BQ27441_Setup(void) {

	// Check that the device has been initialized
	if (!lipo.deviceInitialized){
		return BQ27441_ERR;
	}

	// Manually Enter Configuration Mode
	enterConfig(true);

	// Set OpConfig BIE to 0 (Disable Battery Input Detection)

	// Send BAT_INSERT Command


	// Set Battery Capacity
	if (!setCapacity(BAT_CAPACITY)){
		return BQ27441_ERR;
	}

	// Set GPOUT to active-high
//	setGPOUTPolarity(LOW);

	// Set GPOUT to BAT_LOW mode
//	setGPOUTFunction(BAT_LOW);

	// Set SOCI set and clear thresholds
//	setSOC1Thresholds(SOCI_SET, SOCI_CLR);

	// Set SOCF set and clear thresholds
//	setSOCFThresholds(SOCF_SET, SOCF_CLR);

	// Exit Configuration Mode
	exitConfig(true);

	return BQ27441_OK;
}



// Configures the design capacity of the connected battery.
BQ27441_error setCapacity(uint16_t cap) {
	// Write to STATE subclass (82) of BQ27441 extended memory.
	// Design capacity is a 2-byte piece of data - MSB first
	uint8_t capMSB = cap >> 8;
	uint8_t capLSB = cap & 0x00FF;
	uint16_t designEnergy = cap * 3.7;
	uint8_t energyMSB = designEnergy >> 8;
	uint8_t energyLSB = designEnergy & 0x00FF;
	uint8_t taperRate = (cap * 1.15) / 10;
	uint8_t configData[6] = {capMSB, capLSB, energyMSB, energyLSB, capMSB, capLSB};

	// Update the Design Capacity, Design Energy, and Default Design Capacity
	writeExtendedData(BQ27441_ID_STATE, 10, configData, 6);

	// Update the Taper Rate (400mA + 15% * 0.1 = 46mA)
	writeExtendedData(BQ27441_ID_STATE, 27, taperRate, 2);

	if (capacity(DESIGN) != BAT_CAPACITY){
		lipo.status = BQ27441_CAPACITY_ERR;
		return BQ27441_ERR;
	} else {
		lipo.status = BQ27441_OK;
		lipo.batteryCapacity = BAT_CAPACITY;
	}
	return BQ27441_OK;
}

BQ27441_error BQ28441_getLipoStatus(void) {
	// Make sure the device has been initialized
	if (!lipo.deviceInitialized){
		return BQ27441_ERR;
	}

	lipo.stateOfCharge = soc(FILTERED);
	lipo.voltage = voltage();
	lipo.avgCurrent = current(AVG);
	lipo.remainingCapacity = capacity(REMAIN);
	lipo.fullCapacity = capacity(FULL);
	lipo.power = power();
	lipo.health = soh(PERCENT);
	lipo.temperature = temperature(INTERNAL_TEMP);
	lipo.controlStatus = status();

	return BQ27441_OK;
}

BQ27441_error testStuff() {

	uint16_t tmpFlags = flags();
	uint16_t tmpCap = capacity(DESIGN);
	BQ28441_getLipoStatus();

	return BQ27441_OK;
}

/********************************************************************************
 * Battery Characteristics Functions
 *******************************************************************************/
// Reads and returns the battery voltage
uint16_t voltage(void) {
	return readWord(BQ27441_COMMAND_VOLTAGE);
}

// Reads and returns the specified current measurement
int16_t current(current_measure type) {
	int16_t current = 0;
	switch (type)
	{
	case AVG:
		current = (int16_t) readWord(BQ27441_COMMAND_AVG_CURRENT);
		break;
	case STBY:
		current = (int16_t) readWord(BQ27441_COMMAND_STDBY_CURRENT);
		break;
	case MAX:
		current = (int16_t) readWord(BQ27441_COMMAND_MAX_CURRENT);
		break;
	}

	return current;
}

// Reads and returns the specified capacity measurement
uint16_t capacity(capacity_measure type) {
	uint16_t capacity = 0;
	switch (type)
	{
	case REMAIN:
		return readWord(BQ27441_COMMAND_REM_CAPACITY);
		break;
	case FULL:
		return readWord(BQ27441_COMMAND_FULL_CAPACITY);
		break;
	case AVAIL:
		capacity = readWord(BQ27441_COMMAND_NOM_CAPACITY);
		break;
	case AVAIL_FULL:
		capacity = readWord(BQ27441_COMMAND_AVAIL_CAPACITY);
		break;
	case REMAIN_F:
		capacity = readWord(BQ27441_COMMAND_REM_CAP_FIL);
		break;
	case REMAIN_UF:
		capacity = readWord(BQ27441_COMMAND_REM_CAP_UNFL);
		break;
	case FULL_F:
		capacity = readWord(BQ27441_COMMAND_FULL_CAP_FIL);
		break;
	case FULL_UF:
		capacity = readWord(BQ27441_COMMAND_FULL_CAP_UNFL);
		break;
	case DESIGN:
		capacity = readWord(BQ27441_EXTENDED_CAPACITY);
	}

	return capacity;
}

// Reads and returns measured average power
int16_t power(void) {
	return (int16_t) readWord(BQ27441_COMMAND_AVG_POWER);
}

// Reads and returns specified state of charge measurement
uint16_t soc(soc_measure type) {
	uint16_t socRet = 0;
	switch (type)
	{
	case FILTERED:
		socRet = readWord(BQ27441_COMMAND_SOC);
		break;
	case UNFILTERED:
		socRet = readWord(BQ27441_COMMAND_SOC_UNFL);
		break;
	}

	return socRet;
}

// Reads and returns specified state of health measurement
uint8_t soh(soh_measure type) {
	uint16_t sohRaw = readWord(BQ27441_COMMAND_SOH);
	uint8_t sohStatus = sohRaw >> 8;
	uint8_t sohPercent = sohRaw & 0x00FF;

	if (type == PERCENT)
		return sohPercent;
	else
		return sohStatus;
}

// Reads and returns specified temperature measurement
uint16_t temperature(temp_measure type) {
	uint16_t temp = 0;
	switch (type)
	{
	case BATTERY:
		temp = readWord(BQ27441_COMMAND_TEMP);
		break;
	case INTERNAL_TEMP:
		temp = readWord(BQ27441_COMMAND_INT_TEMP);
		break;
	}

	// Convert 0.1k to C
	temp = (temp / 10) - 273.15;

	return temp;
}

/********************************************************************************
 * GPOUT Control Functions - Private
 *******************************************************************************/

/********************************************************************************
 * Control Sub-Commands - Private
 *******************************************************************************/

// Enter configuration mode - set userControl when you want control over when to exitConfig
bool enterConfig(bool userControl)
{
	if (userControl) {
		lipo.userConfigControl = true;
	}

	if (sealed()) {
		unseal(); // Must be unsealed before making changes
	}

	if (executeControlWord(BQ27441_CONTROL_SET_CFGUPDATE))
	{
		int16_t timeout = BQ72441_I2C_TIMEOUT;
		while ((timeout--) && (!(flags() & BQ27441_FLAG_CFGUPMODE)))
			HAL_Delay(1);

		if (timeout > 0)
			return true;
	}

	return false;
}

// Exit configuration mode with the option to perform a resimulation
bool exitConfig(bool resim) {
	// There are two methods for exiting config mode:
	//    1. Execute the EXIT_CFGUPDATE command
	//    2. Execute the SOFT_RESET command
	// EXIT_CFGUPDATE exits config mode _without_ an OCV (open-circuit voltage)
	// measurement, and without resimulating to update unfiltered-SoC and SoC.
	// If a new OCV measurement or resimulation is desired, SOFT_RESET or
	// EXIT_RESIM should be used to exit config mode.
	lipo.userConfigControl = false;

	if (resim) {
		HAL_Delay(10); //TESTING
		if (softReset()) {
			int16_t timeout = BQ72441_I2C_TIMEOUT;
			while ((timeout--) && ((flags() & BQ27441_FLAG_CFGUPMODE)))
				HAL_Delay(1);
			if (timeout > 0) {
				if (lipo.sealFlag) {
					seal(); // Seal back up if the IC was sealed coming in
				}
				return true;
			}
		}
		return false;
	} else {
		return executeControlWord(BQ27441_CONTROL_EXIT_CFGUPDATE);
	}
}

//--------------------------------------------------CC UNTESTED
// Read the flags() command
uint16_t flags(void) {
	return readWord(BQ27441_COMMAND_FLAGS);
}

// Read the CONTROL_STATUS subcommand of control()
uint16_t status(void) {
	return readControlWord(BQ27441_CONTROL_STATUS);
}
//--------------------------------------------------CC

/********************************************************************************
 * Private Functions
 *******************************************************************************/

// Check if the BQ27441-G1A is sealed or not.
bool sealed(void) {
	uint16_t stat = status();
	return stat & BQ27441_STATUS_SS;
}

// Seal the BQ27441-G1A
bool seal(void) {
	return executeControlWord(BQ27441_CONTROL_SEALED);
}

// UNseal the BQ27441-G1A
bool unseal(void) {
	// To unseal the BQ27441, write the key to the control
	// command. Then immediately write the same key to control again.
	if (executeControlWord(BQ27441_UNSEAL_KEY))
	{
		return executeControlWord(BQ27441_UNSEAL_KEY);
	}
	return false;
}

//// Read the 16-bit opConfig register from extended data
//uint16_t opConfig(void)
//{
//	return readWord(BQ27441_EXTENDED_OPCONFIG);
//}
//
//// Write the 16-bit opConfig register in extended data
//bool writeOpConfig(uint16_t value)
//{
//	uint8_t opConfigMSB = value >> 8;
//	uint8_t opConfigLSB = value & 0x00FF;
//	uint8_t opConfigData[2] = {opConfigMSB, opConfigLSB};
//
//	// OpConfig register location: BQ27441_ID_REGISTERS id, offset 0
//	return writeExtendedData(BQ27441_ID_REGISTERS, 0, opConfigData, 2);
//}

// Issue a soft-reset to the BQ27441-G1A
bool softReset(void) {
	return executeControlWord(BQ27441_CONTROL_SOFT_RESET);
}

//--------------------------------------------------CC UNTESTED
// Read a 16-bit command word from the BQ27441-G1A
uint16_t readWord(uint16_t subAddress) {
	uint8_t data[2];
	i2cReadBytes(subAddress, data, 2);
	return ((uint16_t) data[1] << 8) | data[0];
}
//--------------------------------------------------CC


//--------------------------------------------------CC
// Read a 16-bit subcommand() from the BQ27441-G1A's control()
uint16_t readControlWord(uint16_t function) {
	uint8_t subCommandMSB = (function >> 8);
	uint8_t subCommandLSB = (function & 0x00FF);
	uint8_t command[2] = {subCommandLSB, subCommandMSB};
	uint8_t data[2] = {0, 0};

	i2cWriteBytes((uint8_t) 0, command, 2);
	i2cReadBytes((uint8_t) 0, data, 2);

	return ((uint16_t) data[1] << 8) | data[0];
}
//--------------------------------------------------CC


//--------------------------------------------------CC UNTESTED
// Execute a subcommand() from the BQ27441-G1A's control()
bool executeControlWord(uint16_t function) {
	uint8_t subCommandMSB = (function >> 8);
	uint8_t subCommandLSB = (function & 0x00FF);
	uint8_t command[2] = {subCommandLSB, subCommandMSB};
	//uint8_t data[2] = {0, 0};

	i2cWriteBytes((uint8_t) 0, command, 2);
	return true;

}
//--------------------------------------------------CC

/********************************************************************************
 * Extended Data Commands - Private
 *******************************************************************************/
// Issue a BlockDataControl() command to enable BlockData access
bool blockDataControl(void) {
	uint8_t enableByte = 0x00;
	i2cWriteBytes(BQ27441_EXTENDED_CONTROL, &enableByte, 1);
	return true;
}

// Issue a DataClass() command to set the data class to be accessed
bool blockDataClass(uint8_t id) {
	i2cWriteBytes(BQ27441_EXTENDED_DATACLASS, &id, 1);
	return true;
}

// Issue a DataBlock() command to set the data block to be accessed
bool blockDataOffset(uint8_t offset) {
	i2cWriteBytes(BQ27441_EXTENDED_DATABLOCK, &offset, 1);
	return true;
}

// Read the current checksum using BlockDataCheckSum()
uint8_t blockDataChecksum(void) {
	uint8_t csum;
	i2cReadBytes(BQ27441_EXTENDED_CHECKSUM, &csum, 1);
	return csum;
}

// Use BlockData() to read a byte from the loaded extended data
uint8_t readBlockData(uint8_t offset) {
	uint8_t ret;
	uint8_t address = offset + BQ27441_EXTENDED_BLOCKDATA;
	i2cReadBytes(address, &ret, 1);
	return ret;
}

// Use BlockData() to write a byte to an offset of the loaded data
bool writeBlockData(uint8_t offset, uint8_t data) {
	uint8_t address = offset + BQ27441_EXTENDED_BLOCKDATA;
	i2cWriteBytes(address, &data, 1);
	return true;
}

// Read all 32 bytes of the loaded extended data and compute a
// checksum based on the values.
uint8_t computeBlockChecksum(void) {
	uint8_t data[32];
	i2cReadBytes(BQ27441_EXTENDED_BLOCKDATA, data, 32);

	uint8_t csum = 0;
	for (int i=0; i<32; i++)
	{
		csum += data[i];
	}
	csum = 255 - csum;

	return csum;
}

// Use the BlockDataCheckSum() command to write a checksum value
bool writeBlockChecksum(uint8_t csum) {
	i2cWriteBytes(BQ27441_EXTENDED_CHECKSUM, &csum, 1);
	return true;
}

//// Read a byte from extended data specifying a class ID and position offset
//uint8_t readExtendedData(uint8_t classID, uint8_t offset)
//{
//	uint8_t retData = 0;
//	if (!_userConfigControl) enterConfig(false);
//
//	if (!blockDataControl()) // // enable block data memory control
//		return false; // Return false if enable fails
//	if (!blockDataClass(classID)) // Write class ID using DataBlockClass()
//		return false;
//
//	blockDataOffset(offset / 32); // Write 32-bit block offset (usually 0)
//
//	computeBlockChecksum(); // Compute checksum going in
//	uint8_t oldCsum = blockDataChecksum();
//	/*for (int i=0; i<32; i++)
//		Serial.print(String(readBlockData(i)) + " ");*/
//	retData = readBlockData(offset % 32); // Read from offset (limit to 0-31)
//
//	if (!_userConfigControl) exitConfig();
//
//	return retData;
//}


// Write a specified number of bytes to extended data specifying a
// class ID, position offset.
BQ27441_error writeExtendedData(uint8_t classID, uint8_t offset, uint8_t *data, uint8_t len) {

	if (len > 32){
		return BQ27441_ERR;
	}

	// Enter Configuration Mode - Without User Control
	if (!lipo.userConfigControl) {
		enterConfig(false);
	}

	// Enable Block Data Memory Control
	if (!blockDataControl()) {
		return BQ27441_ERR;
	}

	// Access State Sub Class
	if (!blockDataClass(classID)) {
		return BQ27441_ERR;
	}

	// Write Block Offset Location
	blockDataOffset(offset / 32); // Offset 0-31, Use 0x00 | 32-41, Use 0x01

	// Read the Old Checksum - This line is needed for some reason (probably to clear the register)
	uint8_t oldCsum = readBlockData(BQ27441_EXTENDED_CHECKSUM);

	// Write data bytes:
	for (int i = 0; i < len; i++) {
		// Write to offset, mod 32 if offset is greater than 32
		writeBlockData((offset % 32) + i, data[i]);
	}

	// Write new checksum using BlockDataChecksum (0x60)
	uint8_t newCsum = computeBlockChecksum(); // Compute the new checksum
	writeBlockChecksum(newCsum);

	// Exit Configuration Mode - Without User Control and with Re-Simulation
	if (!lipo.userConfigControl) {
		exitConfig(true);
	}

	return BQ27441_OK;
}


//--------------------------------------------------CC

/********************************************************************************
 * I2C Read and Write Routines - Private
 *******************************************************************************/


// Read a specified number of bytes over I2C at a given subAddress
void i2cReadBytes(uint8_t subAddr, uint8_t *dest, uint8_t Size) {
	HAL_I2C_Mem_Read(lipo.i2c, BQ72441_I2C_ADDRESS, subAddr, I2C_MEMADD_SIZE_8BIT, dest, Size, BQ72441_I2C_TIMEOUT);
}

// Write a specified number of bytes over I2C to a given subAddress
void i2cWriteBytes(uint8_t subAddr, uint8_t *src, uint8_t Size) {
	HAL_I2C_Mem_Write(lipo.i2c, BQ72441_I2C_ADDRESS, subAddr, I2C_MEMADD_SIZE_8BIT, src, Size, BQ72441_I2C_TIMEOUT);
}

//--------------------------------------------------CC







