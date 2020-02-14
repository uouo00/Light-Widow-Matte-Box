/*  dataLogger.h
 *	SD Card Data Logging
 *
 *  Colton Crandell
 *  Light Widow 2020
 *
 * */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H


/******************************************************************************
 * INCLUDES
 *******************************************************************************/
#include <stdint.h>

#include "filter_controller.h"
#include "process_controller.h"

/******************************************************************************
 * DEFINES
 *******************************************************************************/
#define TAG_UID_LEN			8U
/******************************************************************************
 * TYPES
 *******************************************************************************/
typedef struct {
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;

	uint8_t wkDay;
	uint8_t day;
	uint8_t month;
	uint16_t year;

} datetime_record_t;

typedef struct {
	char filterName[FILTER_NAME_LEN + 1];
} filtername_record_t;

typedef struct {
	// Matte Box ID						// Example: LBSA0100P
	char matteBoxID[10];				// [LB][S][A][01][00P] [L]ightWidow Matte [B]ox, [S]tudio Matte Box, Rev [A], January [01], Serial [00P] (Prototype)
	// Date-time Stamp
	datetime_record_t dateTimeStamp;
	// Filter Names
	filtername_record_t filterNames[FILTER_SECTION_SIZE];
} sdLog_record_t;

typedef enum {
	DATALOG_OK,
	DATALOG_ERR
} DL_error;

/******************************************************************************
 * PROTOTYPES
 *******************************************************************************/
DL_error dataLoggerInit(GPIO_TypeDef* sd_Port, uint16_t sd_Pin);
DL_error dataLoggerDeInit(void);
DL_error startSDCard(void);
DL_error stopSDCard(void);

DL_error logDataToSD(sdLog_record_t *sdLog);
DL_error SDDataLog(filterSection_t *fSection);

DL_error dataLogTest(void);
#endif // DATA_LOGGER_H
