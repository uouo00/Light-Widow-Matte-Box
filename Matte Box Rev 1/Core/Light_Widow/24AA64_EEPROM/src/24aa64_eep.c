/*
 *
 *
 * */


#include <string.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "24aa64_eep.h"

/*------------------------------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c1;

/*------------------------------------------------------------------------------*/
// Searches for and returns a filter name based on UID
EEPROM_Status_t readNameFromUID(uint8_t *UID, uint8_t *name){
// Search through the UIDs
// If the UID is found, read the associated Filter Name and return it

	uint16_t counter;				// To store the number of UIDs
	uint16_t uidIndex;				// For loop variable and UID index holder
	uint16_t uidAddr;				// UID address storage
	uint16_t nameAddr;				// Name address storage
	uint8_t tmpUID[UID_SIZE];		// Temporary storage for the UIDs
	bool UIDMatched = false;		// Search match flag for the UID
	uint8_t nameIndex;				// Store the name index of the matched UID

	uidAddr = EEPROM_ASSC_COUNT_LEN + EEPROM_ASSC_COUNT_ADDR;

	// Get the number of associated entries
	readAssociationCount(&counter);

	// Search through the entries until UID is matched, or it runs out.
	for (uidIndex = 0; uidIndex < counter; uidIndex++){
		// Find the address of the current UID and read the eeprom
		uidAddr = (uidIndex * EEPROM_ASSC_BLOCK_LEN) + EEPROM_ASSC_COUNT_LEN + EEPROM_ASSC_COUNT_ADDR;
		eepromRead(uidAddr, tmpUID, UID_SIZE);

		// Check to see if UID matches
		if (memcmp(tmpUID, UID, UID_SIZE) == 0) {
			// UID matches, flag and break loop
			UIDMatched = true;
			break;
		}
	}

	if (UIDMatched) {
		// UID has been matched. Read the Name Index
		uidAddr += UID_SIZE;
		eepromRead(uidAddr, &nameIndex, NAME_SIZE);

		// Calculate the name address from the index
		nameAddr = (nameIndex * EEPROM_LIST_BLOCK_LEN) + EEPROM_LIST_COUNT_LEN + EEPROM_LIST_COUNT_ADDR;

		// Read and Return the Name
		eepromRead(nameAddr, name, NAME_SIZE);

		return EEPROM_OK;
	} else {
		// UID has not been matched
		return ENTRY_NOT_FOUND;
	}
}

// Adds a new Name/UID entry to EEPROM
void associateNewUID(uint8_t *UID, uint8_t *name, uint8_t nameLen){
// Search for the name within the Common Filter Name Section
// If the name is found, return the index, otherwise, store the name and return the index

	uint16_t counter;				// To store the number of Names and UIDs

	uint16_t nameIndex;				// For loop variable and Name index holder
	uint16_t uidIndex;				// For loop variable and UID index holder

	uint8_t cpyName[NAME_SIZE];		// Pad the end of Name with 0's

	uint8_t tmpName[NAME_SIZE];		// Temporary storage for the filter name
	uint8_t tmpUID[UID_SIZE];		// Temporary storage for the UIDs

	bool nameFound = false;			// Search match flag for the filter name
	bool uidFound = false;			// Search match flag for the UID

	uint16_t nameAddr = 0;			// Name address storage
	uint16_t uidAddr = 0;			// UID address storage

	memset(cpyName, 0, NAME_SIZE);  // Set all 0's into cpyName
	memcpy(cpyName, name, nameLen); // Only copy the name into the array

	nameAddr = EEPROM_LIST_COUNT_LEN + EEPROM_LIST_COUNT_ADDR;
	uidAddr = EEPROM_ASSC_COUNT_LEN + EEPROM_ASSC_COUNT_ADDR;

	// Get the Filter Name Index
	readFilterNameCount(&counter);

	// Step through each name looking for a match
	for (nameIndex = 0; nameIndex < counter; nameIndex++){
		// Reset tmpName with all 0's to re-initialize for next name
		memset(tmpName, 0, sizeof(tmpName));

		// Find the address of the current name and read the eeprom
		nameAddr = (nameIndex * EEPROM_LIST_BLOCK_LEN) + EEPROM_LIST_COUNT_LEN + EEPROM_LIST_COUNT_ADDR;
		eepromRead(nameAddr, tmpName, NAME_SIZE);

		// Check to see if name matches
		if (memcmp(tmpName, cpyName, NAME_SIZE) == 0){
			// Name matches, flag and break loop
			nameFound = true;
			break;
		}
	}

	nameAddr = (nameIndex * EEPROM_LIST_BLOCK_LEN) + EEPROM_LIST_COUNT_LEN + EEPROM_LIST_COUNT_ADDR;

	// Check to see if name was found in the EEPROM
	if (!nameFound){
		// The name is not in the list. Store it and update the filter name count in the EEPROM.
		// Store Name at previously determined address
		eepromWrite(nameAddr, cpyName, NAME_SIZE);
		// Update Filter Name Count
		writeFilterNameCount(counter + 1);
	} // At this point, we have a name index that we can store along side the UID

// Next Step: search for the UID within the UID Block section
	// If UID is found, return a duplicate UID error, otherwise, store the first open UID index

	// Get the UID Count
	readAssociationCount(&counter);

	// Step through each UID looking for a match
	for (uidIndex = 0; uidIndex < counter; uidIndex++) {
		// Reset tmpUID with all 0's to re-initialize for next name
		memset(tmpUID, 0, sizeof(tmpUID));

		// Find the address of the current UID and read the eeprom
		uidAddr = (uidIndex * EEPROM_ASSC_BLOCK_LEN) + EEPROM_ASSC_COUNT_LEN + EEPROM_ASSC_COUNT_ADDR;
		eepromRead(uidAddr, tmpUID, UID_SIZE);

		// Check to see if UID matches
		if (memcmp(tmpUID, UID, UID_SIZE) == 0){
			// UID matches, flag and break loop
			uidFound = true;
			break;
		}
	}

	uidAddr = (uidIndex * EEPROM_ASSC_BLOCK_LEN) + EEPROM_ASSC_COUNT_LEN + EEPROM_ASSC_COUNT_ADDR;

	// Check to see if the UID was found in the EEPROM
	if (!uidFound){
		// The UID is not in the list. Store it and update the UID association count in the EEPROM.
		// Store UID at previously determined address
		eepromWrite(uidAddr, UID, UID_SIZE);
		// Update UID Association Count
		writeAssociationCount(counter + 1);
	} // At this point, If the UID wasn't found, a new entry was created.
	  // If the UID was matched, the code continues with the found UID index
	  // The effect is to update the name association of an existing UID

	// Write the Filter Name Index to the last 2 bytes in the same UID Block
	uidAddr += UID_SIZE;	// Get to the last 2 bytes in the UID block

	// Write the associated name index to the end of the UID
	eepromWrite(uidAddr, (uint8_t *) &nameIndex, ASSC_SIZE);

}

// Returns the number of associated entries
void readAssociationCount(uint16_t *aCount){
	eepromRead(EEPROM_ASSC_COUNT_ADDR, (uint8_t *) aCount, EEPROM_ASSC_COUNT_LEN);
}

// Writes/Updates the number of associated entries
void writeAssociationCount(uint16_t aCount){
	uint16_t entryCount = aCount;
	eepromWrite(EEPROM_ASSC_COUNT_ADDR, (uint8_t *) &entryCount, EEPROM_ASSC_COUNT_LEN);
}

/*------------------------------------------------------------------------------*/
void readFilterNameCount(uint16_t *fCount){
	eepromRead(EEPROM_LIST_COUNT_ADDR, (uint8_t *) fCount, EEPROM_LIST_COUNT_LEN);
}

void writeFilterNameCount(uint16_t fCount){
	uint16_t entryCount = fCount;
	eepromWrite(EEPROM_LIST_COUNT_ADDR, (uint8_t *) &entryCount, EEPROM_LIST_COUNT_LEN);
}

/*------------------------------------------------------------------------------*/
// Low-level EEPROM Read Handler
void eepromRead(uint16_t memAddr, uint8_t *data, uint8_t size){
	HAL_I2C_Mem_Read(&hi2c1, EEPROM_I2C_ADDR, memAddr, I2C_MEMADD_SIZE_16BIT, data, size, EEPROM_I2C_TIMEOUT);
	HAL_Delay(10);
}

// Low-level EEPROM Write Handler
void eepromWrite(uint16_t memAddr, uint8_t *data, uint8_t size){
	HAL_I2C_Mem_Write(&hi2c1, EEPROM_I2C_ADDR, memAddr, I2C_MEMADD_SIZE_16BIT, data, size, EEPROM_I2C_TIMEOUT);
	HAL_Delay(10);
}

