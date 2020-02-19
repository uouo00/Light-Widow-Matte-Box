/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef RFID_CONTROLLER_H_
#define RFID_CONTROLLER_H_


/********************************************************************************
 * INCLUDES
 *******************************************************************************/
#include "st_errno.h"
#include "iso15693_3.h"
#include "process_controller.h"

/********************************************************************************
 * DEFINES
 *******************************************************************************/

/********************************************************************************
 * TYPES
 *******************************************************************************/
typedef struct {
	uint8_t tagUID[ISO15693_UID_LENGTH];
	uint8_t tagName[FILTER_NAME_LEN];
} filterTag_t;

typedef struct {
	uint8_t tagCount;
	filterTag_t filterTags[FILTER_SECTION_SIZE];
} detectedTags_t;

/********************************************************************************
 * PROTOTYPES
 *******************************************************************************/
ReturnCode rfidControllerInit(void);
ReturnCode checkFilterTags(detectedTags_t *dTags);
ReturnCode startWakeUpMode(void);

#endif // RFID_CONTROLLER_H_
