/*  \file process_controller.c
 *  \author Colton Crandell
 *  \brief  Main Process Controller for the Matte Box
 *
 *  The main purpose of this module is to keep main.c clean and encapsulate all operations.
 *
 */


/*******************************************************************************
 * INCLUDES
 *******************************************************************************/
#include "fatfs.h"

#include "process_controller.h"			/* Process Controller		*/
#include "rfid_controller.h"			/* RFID Controller			*/
#include "filter_controller.h"			/* Filter Controller		*/

#include "LW_USB_CLI.h"					/* USB CLI Interface		*/
#include "dataLogger.h"					/* SD Card Datalogger		*/
#include "epd_gfx.h"					/* E-Paper Graphics Library	*/
#include "dwt_delay.h"					/* Microsecond Delay		*/
#include "BQ27441.h"					/* Gas Gauge				*/
#include "lm75b.h"						/* Temperature Sensor		*/
#include "rtc.h"						/* Real Time Clock			*/

/*******************************************************************************
 * LOCAL DEFINES
 *******************************************************************************/

/*******************************************************************************
 * LOCAL VARIABLES
 *******************************************************************************/
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;

extern I2C_HandleTypeDef hi2c1;

static process_state_t processState = NORMAL_OPERATION;

/* EPD  ----------------------------------------------------------------------*/
EPD_HandleTypeDef epd1;					/* EPD Handle Type					  */
Canvas canvas1;							/* To Handle Writing an Image to EPD  */

/* RFID ----------------------------------------------------------------------*/
uint8_t globalCommProtectCnt;			/* RFID Interrupt Protection Counter  */
detectedTags_t dTags;					/* Detected Tags					  */
filterSection_t fSection;				/* Filter Section					  */

uint8_t changeFilterPos1;				/* Filter Change Position 1			  */

/*******************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 *******************************************************************************/
void checkISREvents(void);
void gotoSleep(void);

/*******************************************************************************
 * EXTERNAL FUNCTIONS
 *******************************************************************************/
extern void SystemClock_Config(void);	/* To Re-set Clocks after Wake-up		*/

/*******************************************************************************
 * GLOBAL FUNCTIONS
 *******************************************************************************/

void setupIOs(void){

	// Initialize the temperature sensor
	LM75B_Init(&hi2c1, AUX_POWER_GPIO_Port, AUX_POWER_Pin);

	// Initialize and Setup the E-Paper Display
	EPD_Init(EPD_2_0, &hspi2, &epd1);

	// Initialize the GFX Library for the EPD
	Canvas_Init(&canvas1, epd1.dots_per_line, epd1.lines_per_display);
	// Text rotation for a vertical display orientation
	canvas_SetRotate(&canvas1, ROTATE_90);

	// Clear the EPD
	EPD_begin(&epd1);
	EPD_clear(&epd1);
	EPD_end(&epd1);

	// Render a string of text to the buffer image
	canvas_setBGImage(&canvas1, image_background);
	canvas_DrawStringAt(&canvas1, 10, 3, "FILTERS", &Font16, 1);
	// Update the EPD with the new image
	canvas_PrintEPD(&canvas1, &epd1);

	//  // Initialize the Fuel Gauge
	//  BQ27441_Init(&hi2c1, &lipo1);
	//  // Setup the Fuel Gauge
	//  BQ27441_Setup(&lipo1);

	// Initialize the SD Card
	dataLoggerInit(SDIO_POWER_GPIO_Port, SDIO_POWER_Pin);

	HAL_GPIO_WritePin(RFID_POWER_GPIO_Port, RFID_POWER_Pin, GPIO_PIN_SET);
	HAL_Delay(5);
	spiInit(&hspi1);

	rfidControllerInit();
}


void processIOs(void) {
	filterSectionStatus_t filterStatus;

	while (1) {

		switch (processState) {
			case NORMAL_OPERATION:
				// Check ISR events
				checkISREvents();

				// Check on the CLI to see if any new commands are present
				//checkCliStatus();

				// Find filters present in the matte box
				checkFilterTags(&dTags);

				// Compare found filter tags with previous filter section
				filterStatus = updateFilterSection(&dTags, &fSection);

				switch (filterStatus) {
					case NO_CHANGE:
						// Don't need to do anything.
						break;

					case FILTER_REMOVED:
					case FILTER_INSTALLED:
						// Update the EPD with the new filter line up.
						canvas_DrawFilters(&canvas1, &epd1, &fSection);
						// Log to the SD Card
						SDDataLog(&fSection);
						break;

					case UNKNOWN_FILTER_NAME:
						// Change state
						processState = UPDATE_FILTER_NAME;
						break;
				}

				// TODO -  Go to sleep here
				//gotoSleep();

				HAL_Delay(1000);
				break;

			case CHANGE_FILTER_POS:
				// Wait for another button press. Timeout after X amount of time.
				HAL_Delay(FILTER_POSITION_TIMEOUT);
				processState = NORMAL_OPERATION;
				break;

			case UPDATE_FILTER_NAME:

				// Place Holder
				processState = NORMAL_OPERATION;
				break;
		}
	}
}

/*******************************************************************************
 * LOCAL FUNCTIONS
 *******************************************************************************/
void checkISREvents(void) {
	if (isr_flags > 0){
		if (isr_flags & INIT_FAT_FS) {
			// An SD Card was inserted, Re-initialize FatFs and DataLogger
			dataLoggerInit(SDIO_POWER_GPIO_Port, SDIO_POWER_Pin);
		}

		if (isr_flags & DEINIT_FAT_FS) {
			// An SD Card was removed, De-initialize FatFs and DataLogger
			dataLoggerDeInit();
		}

		if (isr_flags & BTN_1_SH_PRESS) {
			// Button 1 was short pressed.
			switch (processState) {
				case NORMAL_OPERATION:
					// Change filter position. Store button 1 as the first button.
					changeFilterPos1 = 1;			// Log Button 1 as the first Selected Filter
					// Highlight the Filter Name

					processState = CHANGE_FILTER_POS;
					break;

				case CHANGE_FILTER_POS:
					// Update the Filter Order and redraw the screen
					// We need to know the first button that was pushed
					// If the button is the same, do nothing.
					break;

				case UPDATE_FILTER_NAME:
					// This button is reserved as "up". Change the list focus.
					// If the top slot is selected, load new page
					break;
			}
		}

		if (isr_flags & BTN_1_LG_PRESS) {
			// Button 1 was long pressed. Update state to Update Filter Name
			switch (processState) {
				case NORMAL_OPERATION:
					break;

				case CHANGE_FILTER_POS:
					break;

				case UPDATE_FILTER_NAME:
					break;
			}
		}

		if (isr_flags & BTN_2_SH_PRESS) {
			// Button 2 was short pressed. Update state to Change Filter Position
			switch (processState) {
				case NORMAL_OPERATION:
					break;

				case CHANGE_FILTER_POS:
					break;

				case UPDATE_FILTER_NAME:
					break;
			}
		}

		if (isr_flags & BTN_2_LG_PRESS) {
			// Button 2 was long pressed. Update state to Update Filter Name
			switch (processState) {
				case NORMAL_OPERATION:
					break;

				case CHANGE_FILTER_POS:
					break;

				case UPDATE_FILTER_NAME:
					break;
			}
		}

		if (isr_flags & BTN_3_SH_PRESS) {
			// Button 3 was short pressed. Update state to Change Filter Position
			switch (processState) {
				case NORMAL_OPERATION:
					break;

				case CHANGE_FILTER_POS:
					break;

				case UPDATE_FILTER_NAME:
					break;
			}
		}

		if (isr_flags & BTN_3_LG_PRESS) {
			// Button 3 was long pressed. Update state to Update Filter Name
			switch (processState) {
				case NORMAL_OPERATION:
					break;

				case CHANGE_FILTER_POS:
					break;

				case UPDATE_FILTER_NAME:
					break;
			}
		}

		isr_flags = 0;	// Reset the flag after handling
	}
}


void gotoSleep( void ) {
	HAL_GPIO_WritePin(RFID_POWER_GPIO_Port, RFID_POWER_Pin, GPIO_PIN_RESET);

	// Set SLEEPDEEP bit of Cortex System Control Register
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

	// Select Power Down Deep Sleep
//	PWR->CR |= PWR_CR_PDDS;

	// Clear the wakeup flag
	PWR->CR |= PWR_CR_CWUF;

	// Wait for Interrupt
	__WFI();
}
