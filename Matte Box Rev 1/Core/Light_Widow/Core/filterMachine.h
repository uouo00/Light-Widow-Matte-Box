/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FILTER_MACHINE_H_
#define FILTER_MACHINE_H_

/*
 ******************************************************************************
 * INCLUDES
 ******************************************************************************
 */
#include <stdint.h>
#include <stdbool.h>
#include "handle_rfid.h"

/********************************************************************************
 * DEFINES
 ******************************************************************************
 */
#define NUM_FILTER_STAGES 			RFAL_POLLER_DEVICES		/* Defines the Number of different filter types */

/********************************************************************************
 * TYPES
 ******************************************************************************
 */

typedef struct {										/* Associates a UID to a Filter Name */
	uint8_t filterName[10];
	uint8_t filterTagUID[8];
	uint8_t position;
} filterStage_t;

typedef struct {
	uint8_t filterCount;
	filterStage_t filter[NUM_FILTER_STAGES];
} filterSection_t;


/********************************************************************************
 * PROTOTYPES
 ******************************************************************************
 */

/*!
 *****************************************************************************
 * \brief Updates the currently installed tags structure using the detected UIDs
 *
 * Removes any undetected UIDs and converts to blank slots
 * Adds newly detected UIDs to first open slots
 *
 * \param[in] dTag: pointer with content showing UIDs detected
 * \param[in] currentFilters: pointer with content showing current filter stage UIDs
 *
 *
 * \returns nothing
 *****************************************************************************
 */
bool updateFilterSet(detectedTags_t *dTag, filterSection_t *currentFilters);

/*!
 *****************************************************************************
 * \brief Uses EEPROM storage to get associated filter names
 *
 * Accesses EEPROM association location and matches the UID then,
 * updates the filter name based on UID. The result will be a current filters
 * struct that correctly resembles the installed filters.
 *
 * \param[in] currentFilters: pointer with content showing current filter stage UIDs
 *
 * \returns nothing
 *****************************************************************************
 */
void loadFilterNames(filterSection_t *currentFilters);


#endif // FILTER_MACHINE_H_
