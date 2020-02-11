/*  LW_USB_CLI.c
 *	Command Line Interface using the Virtual Comm Port USB
 *
 *  Colton Crandell
 *  Light Widow 2020
 *
 *	Commands Look Like this:
 *	setTime 12,00,00 (returns "OK")
 *	setDate 01,01,2020 (returns "OK")
 *	getDateTime (returns date and time -> "01/01/2020 12:00:00")
 *
 *
 *
 * */

/******************************************************************************
 * INCLUDES
 *******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "LW_USB_CLI.h"

#include "usbd_cdc_if.h"
#include "rtc.h"

/******************************************************************************
 * DEFINES
 *******************************************************************************/


/******************************************************************************
 * Global Variables
 *******************************************************************************/
static cliHandler_t usbCli;
static bool cliInitialized = false;

/******************************************************************************
 * PRIVATE PROTOTYPES
 *******************************************************************************/
cli_error parseCommand(void);		/* Command Parser			*/
cli_error parseArguments(void);		/* Command Parser			*/
cli_error executeCommand(void);		/* Command Parser			*/

/******************************************************************************
 * PRIVATE FUNCTIONS
 *******************************************************************************/
cli_error parseCommand(void) {
	// Allocate enough memory for the command
	uint8_t *tempCommand = malloc(usbCli.cmd_len);
	// Copy the Buffer into tempCommand
	memcpy(tempCommand, usbCli.cmd_buffer, usbCli.cmd_len + 1);

	// Split the string based on the space token
	char *token = strtok((char*)tempCommand, " ");

	// Initialize command
	usbCli.command = noCommand;

	// Figure out which command was entered
	if (strcmp(token, "setTime") == 0) {
		usbCli.command = setTime;
	}
	if (strcmp(token, "setDate") == 0) {
		usbCli.command = setDate;
	}
	if (strcmp(token, "getDateTime") == 0) {
		usbCli.command = getDateTime;
	}

	free(tempCommand);
	return CLI_OK;
}

cli_error parseArguments(void) {
	uint8_t i = 0;

	// Check if arguments should be available
	if ((usbCli.command == setTime) | (usbCli.command == setDate)) {

		// Allocate enough memory for the argument string
		uint8_t *tempCommand = malloc(usbCli.cmd_len);

		// Copy the Buffer into tempCommand
		memcpy(tempCommand, usbCli.cmd_buffer, usbCli.cmd_len);

		// Split the string based on the space token
		char *token = strtok((char*)tempCommand, " ");
		// Get the arguments after the command
		token = strtok(NULL, " ");

		char *tempArgs = malloc(strlen(token));
		strcpy(tempArgs, token);

		switch (usbCli.command) {
			case noCommand:
				break;

			case setTime:
				// Break up each argument based on ":" Delimiter
				tempArgs = strtok(token, ":");
				i = 0;
				while (tempArgs != NULL) {
					usbCli.arguments[i] = atoi(tempArgs);
					tempArgs = strtok(NULL, ":");
					i++;
				}
				break;

			case setDate:
				// Break up each argument based on "/" Delimiter
				tempArgs = strtok(token, "/");
				i = 0;
				while (tempArgs != NULL) {
					usbCli.arguments[i] = atoi(tempArgs);
					tempArgs = strtok(NULL, "/");
					i++;
				}
				break;

			case getDateTime:
				break;

		}
		free(tempCommand);
		free(tempArgs);
	}
	return CLI_OK;
}

cli_error executeCommand(void) {
	static volatile RTC_DateTypeDef tmpDate;
	static volatile RTC_TimeTypeDef tmpTime;
	uint8_t dateTimeRet[25] = {0};

	switch (usbCli.command) {
		case noCommand:
			// Command Not Valid, Do Nothing
			CDC_Transmit_FS("Incorrect Command!\r\n", 22);
			break;

		case setTime:
			// Run the set time rtc function
			rtcSetTime(usbCli.arguments[0], usbCli.arguments[1], usbCli.arguments[2]);
			CDC_Transmit_FS("OK\r\n", 6);
			break;

		case setDate:
			rtcSetDate(usbCli.arguments[0], usbCli.arguments[1], usbCli.arguments[2], 0);
			CDC_Transmit_FS("OK\r\n", 6);
			break;

		case getDateTime:

			rtcGetDateTime(&tmpDate, &tmpTime);

			// Compile the Date and Time
			sprintf(dateTimeRet, "%02d/%02d/20%02d %02d:%02d:%02d\r\n",
					tmpDate.Month,
					tmpDate.Date,
					tmpDate.Year,
					tmpTime.Hours,
					tmpTime.Minutes,
					tmpTime.Seconds);

			CDC_Transmit_FS(dateTimeRet, strlen((char *)dateTimeRet));

			break;
	}

	// Reset Command Status
	usbCli.command = noCommand;
	// Reset Buffer
	memset(usbCli.cmd_buffer, 0, usbCli.cmd_len);
	return CLI_OK;
}

/******************************************************************************
 * PUBLIC FUNCTIONS
 *******************************************************************************/
cli_error usbCliInit(void) {

	cliInitialized = true;
	return CLI_OK;
}

void rxCliInput(uint8_t* Buf, uint32_t *Len) {
	if (Buf[0] != 13){					// Ignore Return Character
		usbCli.rxFlag = true;			/* Set the flag for the parser				*/
		usbCli.cmd_len = Len[0];		/* Store the command length for the parser	*/

		// Copy the Received String to the Buffer in the structure
		memcpy(usbCli.cmd_buffer, Buf, usbCli.cmd_len);
	}
}

cli_error checkCliStatus(void) {
	if (usbCli.rxFlag) {
		// We received Something
		parseCommand();
		parseArguments();
		executeCommand();

		// Reset the flag after parsing
		usbCli.rxFlag = false;
	}
	return CLI_OK;
}

cli_error cliTest(void) {

	strcpy(usbCli.cmd_buffer, "setTime 12:00:00");
	usbCli.cmd_len = 17;

	parseCommand();
	parseArguments();
	executeCommand();

	return CLI_OK;
}

