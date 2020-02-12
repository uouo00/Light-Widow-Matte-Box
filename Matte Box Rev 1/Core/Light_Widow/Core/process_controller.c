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
#include "process_controller.h"			/* Process Controller		*/
#include "rfid_controller.h"			/* RFID Controller			*/
#include "filter_controller.h"			/* Filter Controller		*/

#include "LW_USB_CLI.h"					/* USB CLI Interface		*/
#include "dataLogger.h"					/* SD Card Datalogger		*/
#include "epd_gfx.h"					/* E-Paper Graphics Library	*/
#include "dwt_delay.h"					/* Microsecond Delay		*/
#include "BQ27441.h"					/* Gas Gauge				*/
#include "lm75b.h"						/* Temperature Sensor		*/

/*******************************************************************************
 * LOCAL DEFINES
 *******************************************************************************/

/*******************************************************************************
 * LOCAL VARIABLES
 *******************************************************************************/
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;

extern I2C_HandleTypeDef hi2c1;

/* EPD  ----------------------------------------------------------------------*/
EPD_HandleTypeDef epd1;					/* EPD Handle Type					  */
Canvas canvas1;							/* To Handle Writing an Image to EPD  */

/* RFID ----------------------------------------------------------------------*/
uint8_t globalCommProtectCnt;			/* RFID Interrupt Protection Counter  */
detectedTags_t dTags;					/* Detected Tags					  */
filterSection_t fSection;				/* Filter Section					  */

/*******************************************************************************
 * LOCAL FUNCTION PROTOTYPES
 *******************************************************************************/
void gotoSleep(void);
extern void SystemClock_Config(void);	/* Re-set Clocks after Wake-up		*/

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
//
//  HAL_GPIO_WritePin(SDIO_POWER_GPIO_Port, SDIO_POWER_Pin, GPIO_PIN_SET);
//  HAL_Delay(5);

//  FatFsInit();
//  dataLoggerInit();
//
//  rtcModuleInit(&hrtc);


  HAL_GPIO_WritePin(RFID_POWER_GPIO_Port, RFID_POWER_Pin, GPIO_PIN_SET);
  HAL_Delay(5);
  spiInit(&hspi1);

  rfidControllerInit();
}


void processIOs(void) {
	filterSectionStatus_t filterStatus;

	while (1) {
		// Check on the CLI to see if any new commands are present
//		checkCliStatus();

		checkFilterTags(&dTags);

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

				break;

			case UNKNOWN_FILTER_NAME:
				// Ask user what the filter name should be.

				break;
		}
		// TODO -  Go to sleep here

		HAL_Delay(1000);

	}
}

/*******************************************************************************
 * LOCAL FUNCTIONS
 *******************************************************************************/

void gotoSleep( void ) {
	// Set SLEEPDEEP bit of Cortex System Control Register
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

	// Select Power Down Deep Sleep
	PWR->CR |= PWR_CR_PDDS;

	// Clear the wakeup flag
	PWR->CR |= PWR_CR_CWUF;

	// Wait for Interrupt
	__WFI();
}
