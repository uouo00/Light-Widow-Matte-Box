/*  dataLogger.c
 *	SD Card Data Logging Routines
 *	This module is responsible for the communication between the FilterMachine and SD Card Logging
 *
 *  Colton Crandell
 *  Light Widow 2020
 *
 * */

/******************************************************************************
 * INCLUDES
 *******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dataLogger.h"
#include "fatfs.h"

#include "rtc.h"

/******************************************************************************
 * DEFINES
 *******************************************************************************/
#define DL_LOG_DIR_PATH		"0:/LIGHT_WIDOW"
#define DL_DELIM			","
#define DL_ENDL				"\n"

/******************************************************************************
 * MODULAR VARIABLES
 *******************************************************************************/
static bool dlModInit = false;

/******************************************************************************
 * PRIVATE FUNCTIONS
 *******************************************************************************/
bool isLogDirValid(void) {
	bool retVal = false;
	DIR logDir;

	// Try to open the directory
	FRESULT res = f_opendir(&logDir, DL_LOG_DIR_PATH);
	if (FR_OK == res)
	{
		// The directory exists, close it back up
		f_closedir(&logDir);
		retVal = true;
	}

	return retVal;
}


/********************************************************************************
 * PUBLIC FUNCTIONS
 *******************************************************************************/
DL_error dataLoggerInit(void) {
	DL_error retVal = DATALOG_ERR;

	// TODO - Verify RTC Initialization here

	// Verify that FatFs is initialized
	if (FatFsIsModuleInit()) {
		if (!isLogDirValid()) {
			// Create the new directory and file
			FRESULT fr;
			fr = f_mkdir(DL_LOG_DIR_PATH);
			if (fr == FR_OK) {
				// The directory was created successfully
				retVal = DATALOG_OK;
			} else {
				// FatFs Error
				return retVal;
			}
		}
	} else {
		// FatFs Not Initialized
		return retVal;
	}

	dlModInit = true;
	return retVal;
}

/*	This function get's called from the FilterMachine whenever there's
 * a filter change. It first fills out a Log Record, then sends it to
 * this function to be copied to the SD Card.
 * */

DL_error logDataToSD(sdLog_record_t *sdLog) {

	// Ensure the modules are initialized
	if (dlModInit) {

		// Verify that the directory has been initialized
		if (!isLogDirValid()) {
			return DATALOG_ERR;
		}

		// The log directory is valid. Start organizing the data.
		// Convert all non-string fields
		char dateStamp_s[11] = {0};
		char logFileName_s[35] = {0};
		char timeStamp_s[11] = {0};

		// Compile the Date
		sprintf(dateStamp_s, "%02d-%02d-%d",
				sdLog->dateTimeStamp.month,
				sdLog->dateTimeStamp.day,
				sdLog->dateTimeStamp.year);

		// File Name Format: FilterLog_Date_Matte Box ID
		// Example: FilterLog_01-01-2020_LBSA0100P
		sprintf(logFileName_s, "FilterLog_%s_%s.csv",
				dateStamp_s,
				sdLog->matteBoxID);

		// Compile the time
		sprintf(timeStamp_s, "%02d:%02d:%02d,",
				sdLog->dateTimeStamp.hours,
				sdLog->dateTimeStamp.minutes,
				sdLog->dateTimeStamp.seconds);

		// Prepare to write the file
		FRESULT fr;
		FIL* logFil = malloc(sizeof(FIL));

		// Change to the directory
		fr = f_chdir(DL_LOG_DIR_PATH);
		// If the file exists, open it. Otherwise, create the log file.
		fr = f_open(logFil, (const TCHAR*) logFileName_s, FA_OPEN_APPEND | FA_WRITE);

		if (fr != FR_OK) {
			// Can't open the file
			return DATALOG_ERR;
		}

		// The file is open and ready. Write the fields to the file.
		// Check to see if the file is empty
		if (logFil->fptr == 0){
			// Brand New File. Make the Headers
			char headers[100] = {0};

			sprintf(headers, "Light Widow Matte Box\nMatte Box ID:,%s\nDate:,%s\nNotes:\n\n",
					sdLog->matteBoxID,
					dateStamp_s);
			const char tmpStr[50] = "Time,Filter Slot 1,Filter Slot 2,Filter Slot 3\n";

			f_puts(headers, (FIL*) logFil);
			f_puts(tmpStr, (FIL*) logFil);
		}

		// Write the Time Stamp
		f_puts(timeStamp_s, (FIL*) logFil);

		// Write the Filter Name in Slot 1
		f_puts(sdLog->filterNames[0].filterName, (FIL*) logFil);
		f_puts(DL_DELIM, (FIL*) logFil);

		// Write the Filter Name in Slot 2
		f_puts(sdLog->filterNames[1].filterName, (FIL*) logFil);
		f_puts(DL_DELIM, (FIL*) logFil);

		// Write the Filter Name in Slot 3
		f_puts(sdLog->filterNames[2].filterName, (FIL*) logFil);

		// Make a new line
		f_puts(DL_ENDL, (FIL*) logFil);

		// Finish and Close the file
		fr = f_close((FIL*) logFil);

		// De-allocate the FatFs
		free(logFil);

		if (fr == FR_OK) {
			// The file closed successfully
			return DATALOG_OK;
		}
	}
	return DATALOG_ERR;
}

DL_error dataLogTest(void) {
	RTC_DateTypeDef testDate;
	RTC_TimeTypeDef testTime;

	// Load the Log Structure
	sdLog_record_t testLog;
	DL_error res;

	strcpy(testLog.matteBoxID, "LBSA0100P");

	// Get the Time and Store it
	rtcGetDateTime(&testDate, &testTime);

	// Load the DateTime results into the DataLog structure
	testLog.dateTimeStamp.month = testDate.Month;
	testLog.dateTimeStamp.day = testDate.Date;
	testLog.dateTimeStamp.year = testDate.Year + 2000;

	testLog.dateTimeStamp.hours = testTime.Hours;
	testLog.dateTimeStamp.minutes = testTime.Minutes;
	testLog.dateTimeStamp.seconds = testTime.Seconds;

	strcpy(testLog.filterNames[0].filterName, "Filter 1  ");
	strcpy(testLog.filterNames[1].filterName, "Filter 2  ");
	strcpy(testLog.filterNames[2].filterName, "Filter 3  ");

	// Log to the SD Card
	res = logDataToSD(&testLog);

	return res;
}



















