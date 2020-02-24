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
#include "st25r3916/st25r3916_aat.h"
#include "rfal_rf.h"


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
	struct st25r3916AatTuneResult tuningStatus;

	rfalAnalogConfigInitialize();                                                     /* Initialize RFAL's Analog Configs */

	if ((err = rfalInitialize()) == ERR_NONE) {
		rfidInitialized = true;
	}

	err = st25r3916AatTune(NULL, &tuningStatus);

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

ReturnCode startWakeUpMode(void) {
	rfalWakeUpConfig wkupConfig;

	// Set up the Wake Up Configuration
	wkupConfig.period = RFAL_WUM_PERIOD_100MS;
	wkupConfig.irqTout = false;
	wkupConfig.swTagDetect = false;

	// Inductive Amplitude Wake Up
	wkupConfig.indAmp.enabled = false;
	wkupConfig.indAmp.delta = 1;
	wkupConfig.indAmp.reference = RFAL_WUM_REFERENCE_AUTO;
	wkupConfig.indAmp.autoAvg = true;
	wkupConfig.indAmp.aaInclMeas = true;
	wkupConfig.indAmp.aaWeight = RFAL_WUM_AA_WEIGHT_16;

	// Inductive Phase Wake Up - Disabled
	wkupConfig.indPha.enabled = true;
	wkupConfig.indPha.delta = 4;
	wkupConfig.indPha.reference = RFAL_WUM_REFERENCE_AUTO;
	wkupConfig.indPha.autoAvg = true;
	wkupConfig.indPha.aaInclMeas = true;
	wkupConfig.indPha.aaWeight = RFAL_WUM_AA_WEIGHT_8;

//	// Capacitive Wake Up - Disabled
	wkupConfig.cap.enabled = false;
	wkupConfig.cap.delta = 1;
	wkupConfig.cap.reference = RFAL_WUM_REFERENCE_AUTO;
	wkupConfig.cap.autoAvg = true;
	wkupConfig.cap.aaInclMeas = true;
	wkupConfig.cap.aaWeight = RFAL_WUM_AA_WEIGHT_16;

	ReturnCode err = rfalWakeUpModeStart(&wkupConfig);

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




