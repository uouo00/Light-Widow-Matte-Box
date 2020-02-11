/*  LW_USB_CLI.h
 *	Command Line Interface using the Virtual Comm Port USB
 *
 *  Colton Crandell
 *  Light Widow 2020
 *
 * */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef USB_CLI_H
#define USB_CLI_H


/******************************************************************************
 * INCLUDES
 *******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/******************************************************************************
 * DEFINES
 *******************************************************************************/
#define MAX_ARGUMENTS		5
#define CLI_BUFFER_LEN		100

/******************************************************************************
 * TYPES
 *******************************************************************************/
typedef enum {
	noCommand,
	setTime,
	setDate,
	getDateTime
} command_t;

typedef struct {
	command_t command;
	uint8_t arguments[MAX_ARGUMENTS];

	bool rxFlag;							/* Receive Flag							*/
	uint32_t cmd_len;						/* Length of the Returned CLI Command 	*/
	uint8_t cmd_buffer[CLI_BUFFER_LEN];		/* CLI Command Buffer				 	*/

} cliHandler_t;

typedef enum {
	CLI_ERR,
	CLI_OK
} cli_error;

/******************************************************************************
 * PROTOTYPES
 *******************************************************************************/
cli_error usbCliInit(void);

// Receive a string from the CLI. This is called from the CDC_Receive_FS function.
// It is responsible for loading the structure and
void rxCliInput(uint8_t* Buf, uint32_t *Len);

cli_error checkCliStatus(void);

cli_error cliTest(void);

#endif // USB_CLI_H
