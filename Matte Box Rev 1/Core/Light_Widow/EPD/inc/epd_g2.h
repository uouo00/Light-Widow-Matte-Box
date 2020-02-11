/*
 * epd_g2.h
 *
 *  Created on: 11-17-19
 *      Author: Colton Crandell
 */

#ifndef EPD_H
#define EPD_H

#include <stdbool.h>

#include "stm32f4xx_hal.h"

// compile-time #if configuration
#define EPD_CHIP_VERSION      2
#define EPD_FILM_VERSION      231
#define EPD_PWM_REQUIRED      0
#define EPD_IMAGE_ONE_ARG     0
#define EPD_IMAGE_TWO_ARG     1
#define EPD_PARTIAL_AVAILABLE 1

// display panels supported
#define EPD_1_44_SUPPORT      1
#define EPD_1_9_SUPPORT       1
#define EPD_2_0_SUPPORT       1
#define EPD_2_6_SUPPORT       1
#define EPD_2_7_SUPPORT       1

// values for border byte
#define BORDER_BYTE_BLACK 0xff
#define BORDER_BYTE_WHITE 0xaa
#define BORDER_BYTE_NULL  0x00

// possible panel sizes
typedef enum {
	EPD_1_44,        // 128 x 96
	EPD_1_9,         // 144 x 128
	EPD_2_0,         // 200 x 96
	EPD_2_6,         // 232 x 128
	EPD_2_7          // 264 x 176
} EPD_size;

typedef enum {           // error codes
	EPD_OK,
	EPD_UNSUPPORTED_COG,
	EPD_PANEL_BROKEN,
	EPD_DC_FAILED,
	EPD_UNDEFINED
} EPD_error;

typedef enum {
	EPD_BORDER_BYTE_NONE,  // no border byte requred
	EPD_BORDER_BYTE_ZERO,  // border byte == 0x00 requred
	EPD_BORDER_BYTE_SET,   // border byte needs to be set
} EPD_border_byte;

typedef struct __EPD_HandleTypeDef {
	EPD_error			status;					/*!< EPD Status					              */

	EPD_size 			size;					/*!< EPD Panel Size				              */

	uint16_t 			panel_on_pin;			/*!< Panel On Pin and Port		              */
	GPIO_TypeDef 		*panel_on_port;

	uint16_t 			discharge_pin;			/*!< Discharge Pin and Port		              */
	GPIO_TypeDef 		*discharge_port;

	uint16_t 			reset_pin;				/*!< Reset Pin and Port		 	              */
	GPIO_TypeDef 		*reset_port;

	uint16_t 			busy_pin;				/*!< Busy Pin and Port		    	          */
	GPIO_TypeDef 		*busy_port;

	uint16_t 			spi_cs_pin;				/*!< Chip Select Pin and Port		   	      */
	GPIO_TypeDef 		*spi_cs_port;

	SPI_HandleTypeDef 	*spi;					/*!< SPI Handle 			    	          */

	uint16_t base_stage_time;
	uint16_t factored_stage_time;
	uint16_t lines_per_display;
	uint16_t dots_per_line;
	uint16_t bytes_per_line;
	uint16_t bytes_per_scan;

	bool middle_scan;
	bool pre_border_byte;

	EPD_border_byte border_byte;

	const uint8_t *channel_select;
	size_t channel_select_length;

	uint8_t *line_buffer;
	size_t line_buffer_size;

	bool COG_on;
} EPD_HandleTypeDef;


// functions
// =========

// Initialize Function
void EPD_Init(EPD_size size, SPI_HandleTypeDef *spi, EPD_HandleTypeDef *epd);

// set the temperature compensation (call before begin)
void EPD_set_temperature(EPD_HandleTypeDef *epd, int temperature);

// set factored_stage_time directly ('F' command)
	//void EPD_set_factored_stage_time(EPD_type *epd, int pu_stagetime);

// sequence start/end
void EPD_begin(EPD_HandleTypeDef *epd);
void EPD_end(EPD_HandleTypeDef *epd);

// ok/error status
EPD_error EPD_status(EPD_HandleTypeDef *epd);

// items below must be bracketed by begin/end
// ==========================================

// clear the screen
void EPD_clear(EPD_HandleTypeDef *epd);

// assuming a clear (white) screen output an image
void EPD_image_0(EPD_HandleTypeDef *epd, const uint8_t *image);

// change from old image to new image
void EPD_image(EPD_HandleTypeDef *epd, const uint8_t *old_image, const uint8_t *new_image);

// change from old image to new image only updating changed pixels
void EPD_partial_image(EPD_HandleTypeDef *epd, const uint8_t *old_image, const uint8_t *new_image);


#endif  /* EPD_H */
