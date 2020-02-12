/*  \file filter_controller.c
 *  \author Colton Crandell
 *  \brief  Responsible for determining if the filter status has changed and taking appropriate action
 *
 */


/*******************************************************************************
 * INCLUDES
 *******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "filter_controller.h"

/*******************************************************************************
 * LOCAL DEFINES
 *******************************************************************************/

/*******************************************************************************
 * LOCAL VARIABLES
 *******************************************************************************/

/*******************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 *******************************************************************************/

/*******************************************************************************
 * GLOBAL FUNCTIONS
 *******************************************************************************/
filterSectionStatus_t updateFilterSection(detectedTags_t *dTag, filterSection_t *currentFilters){
	filterSectionStatus_t status = NO_CHANGE;
	bool UIDMatched = false;
	uint8_t tempZero[10] = {0};

	// For Loop Variables
	uint8_t detectedTag;
	uint8_t currentTag;

	// Look for filters that have been removed
	for (currentTag = 0; currentTag < FILTER_SECTION_SIZE; currentTag++) {
		UIDMatched = false;
		// Take each current filter tag UID and try to locate it within the detected tags.
		for (detectedTag = 0; detectedTag < FILTER_SECTION_SIZE; detectedTag++){
			if (memcmp(dTag->filterTags[detectedTag].tagUID, currentFilters->filter[currentTag].filterTagUID, 8) == 0){
				// To Prevent matching 2 empty stages
				if (memcmp(dTag->filterTags[detectedTag].tagUID, tempZero, 8) != 0){
					// Found a match. The tag is still here.
					UIDMatched = true;
					break;
				}
			}
		}

		if (!UIDMatched){
			// Make sure the UID is not all 0's
			if (memcmp(currentFilters->filter[currentTag].filterTagUID, tempZero, 8) != 0){
				// A filter has been taken out
				// Clear UID, Name, and Position
				memset(currentFilters->filter[currentTag].filterTagUID, 0x00, 8);
				memset(currentFilters->filter[currentTag].filterName, 0x00, 10);
				currentFilters->filter[currentTag].position = 0;
				status = FILTER_REMOVED;
			}
		}
	}

	// Look for filters that have been installed
	for (detectedTag = 0; detectedTag < FILTER_SECTION_SIZE; detectedTag++){
		UIDMatched = false;
		// Loop through each detected UID and try to find it within the current filters
		for (currentTag = 0; currentTag < FILTER_SECTION_SIZE; currentTag++){
			if (memcmp(dTag->filterTags[detectedTag].tagUID, currentFilters->filter[currentTag].filterTagUID, 8) == 0){
				// Found a match. The tag is still installed.
				UIDMatched = true;
				break;
			}
		}

		if (!UIDMatched){
			// Make sure the detected UID is not all 0's
			if (memcmp(dTag->filterTags[detectedTag].tagUID, tempZero, 8) != 0){
				// A new filter has been installed.
				bool stageEmpty; // Holder for the empty stage
				uint8_t stagePos;

				// Check the name to see if it isn't all zero's
				if (memcmp(dTag->filterTags[detectedTag].tagName, tempZero, 10) != 0){
					// Find first empty stage
					for (stagePos = 0; stagePos < FILTER_SECTION_SIZE; stagePos++){
						stageEmpty = true; // Assume the stage is empty
						for (uint8_t curFilter = 0; curFilter < FILTER_SECTION_SIZE; curFilter++){
							if (currentFilters->filter[curFilter].position == stagePos + 1){
								//There is a filter loaded in this position
								stageEmpty = false;
							}
						}

						if (stageEmpty) {
							// Searched through all Filter positions, and this stage is empty
							break; // Break for loop and access stagePos for first empty stage
						}
					}

					// The first open stage is stored in stagePos
					// Write the new UID, Name, and Position
					memcpy(currentFilters->filter[stagePos].filterTagUID, dTag->filterTags[detectedTag].tagUID, 8);
					memcpy(currentFilters->filter[stagePos].filterName, dTag->filterTags[detectedTag].tagName, 10);
					currentFilters->filter[stagePos].position = stagePos + 1;
					status = FILTER_INSTALLED;
				} else {
					// The name is unknown
					status = UNKNOWN_FILTER_NAME;
				}
			}
		}
	}

	// Update the filter counts
	currentFilters->filterCount = dTag->tagCount;

	return status;
}
/*******************************************************************************
 * LOCAL FUNCTIONS
 *******************************************************************************/
