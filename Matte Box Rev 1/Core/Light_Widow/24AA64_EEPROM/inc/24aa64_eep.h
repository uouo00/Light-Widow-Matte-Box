/*
 *
 *
 * The EEPROM is used to store 4 items:
 * Common List Names, Common List Name Counter, Associated UIDs, Associated UID Counter
 *
 * Memory Structure:
 * Addresses: 0x0000 and 0x0001 hold the counter for the number of Common Filter Names
 * Address 0x0002-0x03FF hold the Common Filter Names in blocks of 10 bytes (characters)
 * This is enough memory space for 102 Common Filter Names
 *
 * Address 0x0400 holds the UID Associations
 * UID Association Structure:
 * UID Length is 8 bytes long
 * 2 bytes are reserved for the index of the filter name
 * Total UID Block size is 10 bytes long
 * */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef EEPROM_DRIVER_H_
#define EEPROM_DRIVER_H_

/*
 ******************************************************************************
 * INCLUDES
 ******************************************************************************
 */
#include <stdbool.h>
#include <stdint.h>

/********************************************************************************
 * DEFINES
 ******************************************************************************
 */

// Starting Base address for EEPROM
#define EEPROM_BASE_ADDR			0x0000
#define EEPROM_I2C_ADDR				0xA0
#define EEPROM_I2C_TIMEOUT			50
#define UID_SIZE					8													/* Size of UID								    */
#define NAME_SIZE					10													/* Size of Name								    */
#define ASSC_SIZE					2													/* Storage size of Association Index after UID	*/

// Address locations for the Common Name List
#define EEPROM_LIST_COUNT_ADDR		EEPROM_BASE_ADDR									/* Address of the Common Name List Counter	    */
#define EEPROM_LIST_COUNT_LEN		2
#define EEPROM_LIST_START_ADDR		(EEPROM_LIST_COUNT_ADDR + EEPROM_LIST_COUNT_LEN)	/* Starting Address for the Common Name List    */
#define EEPROM_LIST_BLOCK_LEN		10													/* Length of Names in Characters 			    */

// Address locations for the UID Associations
#define EEPROM_ASSC_COUNT_ADDR		0x0400												/* Address of the Common Name List Counter */
#define EEPROM_ASSC_COUNT_LEN		2
#define EEPROM_ASSC_START_ADDR		(EEPROM_ASSC_COUNT_ADDR + EEPROM_ASSC_COUNT_LEN)	/* Starting Address for the UID Associations 	*/
#define EEPROM_ASSC_BLOCK_LEN		10													/* Length of UID Association Block 		        */

/********************************************************************************
 * TYPES
 ******************************************************************************
 */
typedef enum{
	EEPROM_OK 		= 		0,
	ENTRY_NOT_FOUND = 		1,
	EEPROM_ERR		= 		2
} EEPROM_Status_t;


/********************************************************************************
 * PROTOTYPES
 ******************************************************************************
 */

// Read Filter Association
EEPROM_Status_t readNameFromUID(uint8_t *UID, uint8_t *name);

// Write Filter Association
void associateNewUID(uint8_t *UID, uint8_t *name, uint8_t nameLen);

// Read Filter Association Counter
void readAssociationCount(uint16_t *aCount);

// Write Filter Association Counter
void writeAssociationCount(uint16_t aCount);

/*------------------------------------------------------------------------------*/

// Read Filter Name Counter
void readFilterNameCount(uint16_t *fCount);

// Write Filter Name Counter
void writeFilterNameCount(uint16_t fCount);

/*------------------------------------------------------------------------------*/

void eepromRead(uint16_t memAddr, uint8_t *data, uint8_t size);
void eepromWrite(uint16_t memAddr, uint8_t *data, uint8_t size);

#endif // EEPROM_DRIVER_H_
