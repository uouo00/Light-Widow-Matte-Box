/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FILTER_CONTROLLER_H_
#define FILTER_CONTROLLER_H_

/********************************************************************************
 * INCLUDES
 *******************************************************************************/
#include <stdint.h>

#include "process_controller.h"
#include "rfid_controller.h"
/********************************************************************************
 * DEFINES
 *******************************************************************************/

/********************************************************************************
 * TYPES
 *******************************************************************************/

typedef enum {
	NO_CHANGE,
	FILTER_REMOVED,
	FILTER_INSTALLED,
	UNKNOWN_FILTER_NAME
} filterSectionStatus_t;

typedef struct {										/* Associates a UID to a Filter Name */
	uint8_t filterName[10];
	uint8_t filterTagUID[8];
	uint8_t position;
} filterStage_t;

typedef struct {
	uint8_t filterCount;
	filterStage_t filter[FILTER_SECTION_SIZE];
} filterSection_t;

/********************************************************************************
 * PROTOTYPES
 *******************************************************************************/
filterSectionStatus_t updateFilterSection(detectedTags_t *dTag, filterSection_t *currentFilters);



#endif // FILTER_CONTROLLER_H_
