#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "filterMachine.h"
#include "24aa64_eep.h"

bool updateFilterSet(dTags_t *dTag, filtSection_t *currentFilters){

	bool changeOccurred = false;
	bool UIDMatched = false;
	uint8_t tempZero[10] = {0};

	// For Loop Variables
	uint8_t detectedTag;
	uint8_t currentTag;

	// Look for filters that have been removed
	for (currentTag = 0; currentTag < NUM_FILTER_STAGES; currentTag++) {
		UIDMatched = false;
		// Take each current filter tag UID and try to locate it within the detected tags.
		for (detectedTag = 0; detectedTag < NUM_FILTER_STAGES; detectedTag++){
			if (memcmp(dTag->TagUID[detectedTag], currentFilters->filter[currentTag].filterTagUID, 8) == 0){
				// To Prevent matching 2 empty stages
				if (memcmp(dTag->TagUID[detectedTag], tempZero, 8) != 0){
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
				changeOccurred = true;
			}
		}
	}

	// Look for filters that have been installed
	for (detectedTag = 0; detectedTag < NUM_FILTER_STAGES; detectedTag++){
		UIDMatched = false;
		// Loop through each detected UID and try to find it within the current filters
		for (currentTag = 0; currentTag < NUM_FILTER_STAGES; currentTag++){
			if (memcmp(dTag->TagUID[detectedTag], currentFilters->filter[currentTag].filterTagUID, 8) == 0){
				// Found a match. The tag is still installed.
				UIDMatched = true;
				break;
			}
		}

		if (!UIDMatched){
			// Make sure the detected UID is not all 0's
			if (memcmp(dTag->TagUID[detectedTag], tempZero, 8) != 0){
				// A new filter has been installed.
				bool stageEmpty; // Holder for the empty stage
				uint8_t stagePos;

				// Find first empty stage
				for (stagePos = 0; stagePos < NUM_FILTER_STAGES; stagePos++){
					stageEmpty = true; // Assume the stage is empty
					for (uint8_t curFilter = 0; curFilter < NUM_FILTER_STAGES; curFilter++){
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
				memcpy(currentFilters->filter[stagePos].filterTagUID, dTag->TagUID[detectedTag], 8);
				readNameFromUID(currentFilters->filter[stagePos].filterTagUID, currentFilters->filter[stagePos].filterName);
				currentFilters->filter[stagePos].position = stagePos + 1;
				changeOccurred = true;
			}
		}
	}

	// Update the filter counts
	currentFilters->filterCount = dTag->tagCount;

	// Reset dTag for the next cycle
	for (uint8_t i = 0; i < NUM_FILTER_STAGES; i++){
		memset(dTag->TagUID[i], 0x00, 8);
	}
	dTag->tagCount = 0;

	return changeOccurred;

}

void loadFilterNames(filtSection_t *currentFilters){
// Loop through each filter that is loaded, then load the filter name stored in eeprom

	uint8_t tmpName[10] = {0};

	// Loop through each loaded filter
	for (uint8_t counter = 0; counter < currentFilters->filterCount; counter++){
		// Read associated name from eeprom
		readNameFromUID(currentFilters->filter[counter].filterTagUID, tmpName);
		// Write the name to the structure
		memcpy(currentFilters->filter[counter].filterName, tmpName, sizeof(tmpName));
	}
}





