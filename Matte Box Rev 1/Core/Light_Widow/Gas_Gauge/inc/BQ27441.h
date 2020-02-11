/*
 *
 *
 * BQ27441 Lithium Polymer Gas Gauge Communications
 *
 *
 * */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef GAS_GAUGE_H_
#define GAS_GAUGE_H_

/*
 ******************************************************************************
 * INCLUDES
 ******************************************************************************
 */
#include <stdbool.h>
#include <stdint.h>

#include "BQ27441_Definitions.h"
#include "stm32f4xx_hal.h"

/********************************************************************************
 * DEFINES
 ******************************************************************************
 */
#define BQ72441_I2C_TIMEOUT 2000

/********************************************************************************
 * TYPES
 ******************************************************************************
 */

// Parameters for the current() function, to specify which current to read
typedef enum {
	AVG,  // Average Current (DEFAULT)
	STBY, // Standby Current
	MAX   // Max Current
} current_measure;

// Parameters for the capacity() function, to specify which capacity to read
typedef enum {
	REMAIN,     // Remaining Capacity (DEFAULT)
	FULL,       // Full Capacity
	AVAIL,      // Available Capacity
	AVAIL_FULL, // Full Available Capacity
	REMAIN_F,   // Remaining Capacity Filtered
	REMAIN_UF,  // Remaining Capacity Unfiltered
	FULL_F,     // Full Capacity Filtered
	FULL_UF,    // Full Capacity Unfiltered
	DESIGN      // Design Capacity
} capacity_measure;

// Parameters for the soc() function
typedef enum {
	FILTERED,  // State of Charge Filtered (DEFAULT)
	UNFILTERED // State of Charge Unfiltered
} soc_measure;

// Parameters for the soh() function
typedef enum {
	PERCENT,  // State of Health Percentage (DEFAULT)
	SOH_STAT  // State of Health Status Bits
} soh_measure;

// Parameters for the temperature() function
typedef enum {
	BATTERY,      // Battery Temperature (DEFAULT)
	INTERNAL_TEMP // Internal IC Temperature
} temp_measure;

// Parameters for the setGPOUTFunction() function
typedef enum {
	SOC_INT, // Set GPOUT to SOC_INT functionality
	BAT_LOW  // Set GPOUT to BAT_LOW functionality
} gpout_function;

// error codes
typedef enum {
	BQ27441_ERR,
	BQ27441_OK,
	BQ27441_CAPACITY_ERR
} BQ27441_error;

// Fuel Gauge Handle
typedef struct {
	BQ27441_error status;						/*!< Fuel Gauge Status		    	          */
	I2C_HandleTypeDef *i2c;						/*!< I2C Handle 			    	          */

	bool userConfigControl;						/*!< Config Under User Control    	          */
	bool sealFlag;								/*!< Seal Status			    	          */

	bool deviceInitialized;						/*!< Device Initialized		    	          */
	uint16_t deviceType;						/*!< Device Type = 0x0421	    	          */
	uint16_t firmwareVer;						/*!< Firmware Version		    	          */
	uint8_t dataMemCode;						/*!< Data Memory Code		    	          */

	uint16_t batteryCapacity;					/*!< Battery Capacity		    	          */
	uint16_t controlStatus;						/*!< Control Status			    	          */

	// Battery Characteristics
	uint16_t stateOfCharge;						/*!< State of Charge (%)	    	          */
	uint16_t voltage;							/*!< Cell Voltage (V)		    	          */
	int16_t avgCurrent;							/*!< Average Cell Current (mA)    	          */
	uint16_t remainingCapacity;					/*!< Remaining Capacity (mAh)    	          */
	uint16_t fullCapacity;						/*!< Full Charge Capacity (mAh)    	          */
	int16_t power;								/*!< Average Power Consumption (mW)	          */
	uint8_t health;								/*!< Cell Health (%)		    	          */
	uint16_t temperature;						/*!< Internal IC Temperature (0.1degK)		  */

} BQ27441_HandleTypeDef;


/********************************************************************************
 * PROTOTYPES
 *******************************************************************************/

/**
	@brief Initializes I2C and verifies communication with the BQ27441.
		Must be called before using any other functions.
	@param *i2c I2C Handle that will communicate with the BQ27441.
	@param *lipo BQ27441 handle
*/
void BQ27441_Init(I2C_HandleTypeDef *i2c);

/**
	@brief Runs the setup operations to configure the BQ27441.
	@param *lipo BQ27441 handle
	@retval OK if setup successful
*/
BQ27441_error BQ27441_Setup(void);

/**
	Configures the design capacity of the connected battery.

	@param capacity of battery (unsigned 16-bit value)
	@return OK if capacity successfully set.
*/
BQ27441_error setCapacity(uint16_t capacity);

/**
	Gets the latest status of the connected lipo battery

	@param *lipo BQ27441 handle
	@return OK if status successfully set.
*/
BQ27441_error BQ28441_getLipoStatus(void);

/********************************************************************************
 * Battery Characteristics Functions
 *******************************************************************************/
uint16_t soc(soc_measure type);
uint16_t voltage(void);
int16_t current(current_measure type);
uint16_t capacity(capacity_measure type);
int16_t power(void);
uint8_t soh(soh_measure type);
uint16_t temperature(temp_measure type);

/********************************************************************************
 * Control Sub Commands
 *******************************************************************************/

/**
	Read the device type - should be 0x0421

	@return 16-bit value read from DEVICE_TYPE subcommand
*/
uint16_t deviceType(void);



BQ27441_error testStuff(void);

#endif // GAS_GAUGE_H_
