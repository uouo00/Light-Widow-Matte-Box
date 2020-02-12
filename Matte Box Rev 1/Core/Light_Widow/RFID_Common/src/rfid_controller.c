/*  \file rfid_controller.c
 *  \author Colton Crandell
 *  \brief  ISO15693 RFID Tag Controller
 *
 *  This module acts as an interface for the iso15693_3 module
 */


/* *****************************************************************************
 * INCLUDES
 * *****************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "rfid_controller.h"
#include "iso15693_3.h"
#include "st_errno.h"

#include "rfal_analogConfig.h"


/* *****************************************************************************
 * LOCAL DEFINES
 * *****************************************************************************/

/* *****************************************************************************
 * LOCAL VARIABLES
 * *****************************************************************************/
static bool rfidInitialized = false;

static uint8_t crdcnt;
static iso15693ProximityCard_t cards[FILTER_SECTION_SIZE];

/*******************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 *******************************************************************************/

/*******************************************************************************
 * GLOBAL FUNCTIONS
 *******************************************************************************/
ReturnCode rfidControllerInit(void) {
	ReturnCode err = ERR_NONE;

	rfalAnalogConfigInitialize();                                                     /* Initialize RFAL's Analog Configs */

	if ((err = rfalInitialize()) == ERR_NONE) {
		rfidInitialized = true;
	}
	return err;
}

ReturnCode checkFilterTags(detectedTags_t *dTags) {
	ReturnCode err = ERR_NONE;
	iso15693ProximityCard_t* crdptr;
	uint8_t  resFlags;
	uint16_t actLen;

	uint8_t rxNameBuffer[16];

	if (!rfidInitialized) return ERR_PARAM;

	// Reset dTags
	dTags->tagCount = 0;
	for (uint8_t i = 0; i < FILTER_SECTION_SIZE; i++) {
		memset(dTags->filterTags[i].tagUID, 0, ISO15693_UID_LENGTH);
		memset(dTags->filterTags[i].tagName, 0, FILTER_NAME_LEN);
	}

	crdptr = NULL;

	// Initialize the Reader. Turn the field on and get ready to detect cards.
	if ((err = iso15693Initialize(false, false)) != ERR_NONE) goto deinitReader;

	// Get the card inventory. This will load the [cards] variable with UIDs and update the [crdcnt] variable
	if ((err = iso15693Inventory(ISO15693_NUM_SLOTS_1, 0, NULL, cards, FILTER_SECTION_SIZE, &crdcnt)) != ERR_NONE) goto deinitReader;

	// Get the Filter names stored on each tag
	for (uint8_t i = 0; i < crdcnt; i++) {
		// Loop through each card by updating the UID pointer, selecting the card, read the mem block, and update the filter section name.
		// Select the next card
		crdptr = &cards[i];
		if ((err = iso15693SelectPicc(crdptr)) != ERR_NONE) goto deinitReader;

		// Read the Memory Block
		if ((err = iso15693ReadMultipleBlocks(crdptr, 0, 4, &resFlags, rxNameBuffer, sizeof(rxNameBuffer) + 1, &actLen)) != ERR_NONE) goto deinitReader;

		// Copy the UID and Filter Name into the filterSection
		memcpy(dTags->filterTags[i].tagUID, cards[i].uid, ISO15693_UID_LENGTH);
		memcpy(dTags->filterTags[i].tagName, rxNameBuffer, FILTER_NAME_LEN);
	}

	dTags->tagCount = crdcnt;

	// Deinitialize the Reader
	deinitReader:
		iso15693Deinitialize(false);

	return err;
}



/* *****************************************************************************
 * LOCAL FUNCTIONS
 * *****************************************************************************/

// Send Stay Quiet

// Select PICC

// Get System Information

// Read Single Block

// Fast Read Single Block

// Fast Read Multiple Bocks

// Read Multiple Blocks

// Write Single Block




