/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef PROCESS_CONTROLLER_H_
#define PROCESS_CONTROLLER_H_


/********************************************************************************
 * INCLUDES
 *******************************************************************************/

/********************************************************************************
 * DEFINES
 *******************************************************************************/
#define MATTE_BOX_HW_ID				"LBSA0100P"	/* Hardware ID										*/
#define MATTE_BOX_FW_ID				0x010101	/* Firmware Revision								*/
#define FILTER_SECTION_SIZE			3			/* Number of filter slots installed					*/
#define FILTER_NAME_LEN				10			/* String Length for Filter Names					*/

#define FILTER_POSITION_TIMEOUT		5000		/* Amount of time(ms) until filter position timeout	*/

/********************************************************************************
 * TYPES
 *******************************************************************************/
// State machine states
typedef enum {
	NORMAL_OPERATION,
	CHANGE_FILTER_POS,
	UPDATE_FILTER_NAME,
} process_state_t;

typedef enum {
	INIT_FAT_FS = 1 << 0,				/* An SD Card was installed		*/
	DEINIT_FAT_FS = 1 << 1,				/* An SD Card was removed		*/
	BTN_1_SH_PRESS = 1 << 2,			/* Button 1 was short pressed	*/
	BTN_1_LG_PRESS = 1 << 3,			/* Button 1 was long pressed	*/
	BTN_2_SH_PRESS = 1 << 4,			/* Button 2 was short pressed	*/
	BTN_2_LG_PRESS = 1 << 5,			/* Button 2 was long pressed	*/
	BTN_3_SH_PRESS = 1 << 6,			/* Button 3 was short pressed	*/
	BTN_3_LG_PRESS = 1 << 7,			/* Button 4 was long pressed	*/
} isr_flags_t;

uint8_t isr_flags;						/* Global Variable to hold the flag status */
/********************************************************************************
 * PROTOTYPES
 *******************************************************************************/
// Main setup for the program.
void setupIOs(void);

// Main operation state
void processIOs(void);


#endif // PROCESS_CONTROLLER_H_
