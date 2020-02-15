/*
 * epd_gfx.h
 *
 *  Created on: 11-17-19
 *      Author: Colton Crandell
 *      Ref: https://github.com/soonuse/epd-library-stm32/
 */

#ifndef _EPD_GFX_H
#define _EPD_GFX_H

#include "epd_g2.h"  // EPD V231_G2 Driver

#include "fonts.h"
#include "LW_Logo.h"

#include "filter_controller.h"
#include "process_controller.h"


// Display orientation
#define ROTATE_0            0
#define ROTATE_90           1
#define ROTATE_180          2
#define ROTATE_270          3

// Color inverse. 1 or 0 = set or reset a bit if set a colored pixel
#define IF_INVERT_COLOR     0

#define CYCLES_UNTIL_REFRESH		6			/* The number of partial write cycles before a full refresh */

typedef enum {           // error codes
	CANVAS_OK,
	CANVAS_MEM_ERROR,
	CANVAS_ERR
} Canvas_error;

typedef struct Canvas_t {
	uint8_t* image;
    uint16_t width;
    uint16_t height;
    uint8_t rotate;
} Canvas;

Canvas_error Canvas_Init(Canvas* canvas, uint16_t width, uint16_t height);
void canvas_DrawFilters(Canvas* canvas, EPD_HandleTypeDef* epd, filterSection_t *currentFilters, uint8_t invPosition);

void canvas_SetRotate(Canvas* canvas, uint8_t rotate);

void canvas_DrawAbsolutePixel(Canvas* canvas, uint16_t x, uint16_t y, int colored);
void canvas_DrawPixel(Canvas* canvas, uint16_t x, uint16_t y, int colored);
void canvas_DrawCharAt(Canvas* canvas, uint16_t x, uint16_t y, char ascii_char, sFONT* font, int colored);
void canvas_DrawStringAt(Canvas* canvas, uint16_t x, uint16_t y, const char* text, sFONT* font, int colored);

void canvas_PrintEPD(Canvas* canvas, EPD_HandleTypeDef* epd);
void canvas_setBGImage(Canvas* canvas, const uint8_t* bg_image);
void canvas_UpdateEPD(Canvas* canvas, EPD_HandleTypeDef* epd, uint8_t* old_img);


#endif // _EPD_GFX_H
