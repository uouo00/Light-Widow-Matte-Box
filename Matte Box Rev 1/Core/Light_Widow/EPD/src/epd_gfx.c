
#include <stdlib.h>
#include <string.h>

#include "epd_gfx.h"
#include "epd_g2.h"

uint8_t* b_canvas;

Canvas_error Canvas_Init(Canvas* canvas, uint16_t width, uint16_t height) {
	// Allocate memory for the buffer canvas depending on display size
	b_canvas = (uint8_t *)malloc(width * height);
	memset(b_canvas, 0x00, width * height);

	if (b_canvas == NULL){
		return CANVAS_MEM_ERROR;
	}
	canvas->rotate = ROTATE_0;
	canvas->image = b_canvas;
	/* 1 byte = 8 pixels, so the width should be the multiple of 8 */
	canvas->width = width % 8 ? width + 8 - (width % 8) : width;
	canvas->height = height;

	return CANVAS_OK;
}

// Responsible for taking the software filter set model and updating the EPD
void canvas_DrawFilters(Canvas* canvas, EPD_HandleTypeDef* epd, filterSection_t *currentFilters){
// This draws the filter names based on the position stored in the filter model
// It also recognizes longer names and splits them up into 2 lines
// It also keeps track of how many times the screen has been refreshed
	// If it has been refreshed 5+ times, perform a full refresh

// At some point, this will need to know how many filter slots are installed and draw the canvas correctly

	uint8_t tmpName[10] = {0};
	uint8_t nameLen;
	uint8_t startPixelY;
	uint8_t startPixelX;

	uint8_t prevImage[2400];
	static uint8_t refreshCount = 0;	// Keep track of how many times the screen has been updated

	// Store the current image so that a partial update can be compared
	memcpy(prevImage, canvas->image, 2400);

	//Start with a clear BG
	canvas_setBGImage(canvas, image_background);
	canvas_DrawStringAt(canvas, 10, 3, "FILTERS", &Font16, 1);

	// Loop through all filter stages
	for (uint8_t filterStage = 1; filterStage <= NUM_FILTER_STAGES; filterStage++){
		// Try to find a filter in the current position
		for (uint8_t filter = 0; filter < NUM_FILTER_STAGES; filter++){
			if (currentFilters->filter[filter].position == filterStage){
				// Found a filter loaded in the current position
				// Store the name into tmpName
				memcpy(tmpName, currentFilters->filter[filter].filterName, 10);

				// "0x00" is a signal that there isn't any more data
				for (nameLen = 0; nameLen < 10; nameLen++){
					if (tmpName[nameLen] == 0x00){
						break;
					}
				}

				// At this point, nameIndex has the length of the Name
				if (nameLen <= 5){
					// Only 1 row needed
					// Find the starting Y pixel
					uint8_t sectionHeight;
					sectionHeight = epd->dots_per_line - 15;			// Account for the header
					sectionHeight /= NUM_FILTER_STAGES;					// Account for the number of filters
					startPixelY = sectionHeight * (filterStage - 1);  	// Get the correct section
					startPixelY += sectionHeight / 2;					// Get to the center of the section
					startPixelY -= 12;									// Account for the text height (assuming font24)
					startPixelY += 15;									// Re-incorporate the header length

					// Find the staring X pixel
					startPixelX = epd->lines_per_display / 2;	// Find the center of the line
					startPixelX -= (nameLen * 17) / 2;			// difference of the center of text and center of line

					// Draw text
					canvas_DrawStringAt(canvas, startPixelX, startPixelY, tmpName, &Font24, 0);

				} else {
					// Need to split into 2 rows
					uint8_t topLine[6] = {0};							// Top Line Holder
					uint8_t botLine[6] = {0};							// Bottom Line Holder
					memcpy(topLine, tmpName, 5);

					for (uint8_t i = 5; i < 10; i++){
						botLine[i - 5] = tmpName[i];
					}

					// Find the starting Y pixel
					uint8_t sectionHeight;
					sectionHeight = epd->dots_per_line - 15;			// Account for the header
					sectionHeight /= NUM_FILTER_STAGES;					// Account for the number of filters
					startPixelY = sectionHeight * (filterStage - 1);  	// Get the correct section
					startPixelY += sectionHeight / 2;					// Get to the center of the section
					startPixelY -= 24;									// Account for the text height (assuming font24)
					startPixelY += 15;									// Re-incorporate the header length

//					// Find the staring X pixel
//					startPixelX = epd->lines_per_display / 2;	// Find the center of the line
//					startPixelX -= (nameLen * 17) / 2;			// difference of the center of text and center of line

					// Draw text
					canvas_DrawStringAt(canvas, 6, startPixelY, topLine, &Font24, 0);

//					// Find the staring X pixel
//					startPixelX = epd->lines_per_display / 2;	// Find the center of the line
//					startPixelX -= (nameLen * 17) / 2;			// difference of the center of text and center of line

					canvas_DrawStringAt(canvas, 6, startPixelY + 24, botLine, &Font24, 0);
				}
			}
		}
	}


	// Right now, it just does a complete refresh - Need to change in the future
	// Print the rendered image
	if (refreshCount < 5){
		canvas_UpdateEPD(canvas, epd, prevImage);
		refreshCount++;
	} else {
		canvas_PrintEPD(canvas, epd);
		refreshCount = 0;
	}


}

void canvas_SetRotate(Canvas* canvas, uint8_t rotate) {
	canvas->rotate = rotate;
}

/**
 *  @brief: this draws a pixel by absolute coordinates.
 *          this function won't be affected by the rotate parameter.
 */
void canvas_DrawAbsolutePixel(Canvas* canvas, uint16_t x, uint16_t y, int colored) {

	// swap x order. 7 to 0, 6 to 1, 5 to 2, etc.
	// 1. Get bit position
	uint8_t bit_pos = x % 8;

	switch (bit_pos){
		case 0:
			bit_pos = 7;
			break;
		case 1:
			bit_pos = 6;
			break;
		case 2:
			bit_pos = 5;
			break;
		case 3:
			bit_pos = 4;
			break;
		case 4:
			bit_pos = 3;
			break;
		case 5:
			bit_pos = 2;
			break;
		case 6:
			bit_pos = 1;
			break;
		case 7:
			bit_pos = 0;
			break;
	}

    if (x < 0 || x >= canvas->width || y < 0 || y >= canvas->height) {
        return;
    }
    if (IF_INVERT_COLOR) {
        if (colored) {
        	canvas->image[(x + y * canvas->width) / 8] |= 0x80 >> (bit_pos);
        } else {
        	canvas->image[(x + y * canvas->width) / 8] &= ~(0x80 >> (bit_pos));
        }
    } else {
        if (colored) {
        	canvas->image[(x + y * canvas->width) / 8] &= ~(0x80 >> (bit_pos));
        } else {
        	canvas->image[(x + y * canvas->width) / 8] |= 0x80 >> (bit_pos);
        }
    }
}

/**
 *  @brief: this draws a pixel by the coordinates
 */
void canvas_DrawPixel(Canvas* canvas, uint16_t x, uint16_t y, int colored) {
    int point_temp;
    if (canvas->rotate == ROTATE_0) {
        if(x < 0 || x >= canvas->width || y < 0 || y >= canvas->height) {
            return;
        }
        canvas_DrawAbsolutePixel(canvas, x, y, colored);
    } else if (canvas->rotate == ROTATE_90) {
        if(x < 0 || x >= canvas->height || y < 0 || y >= canvas->width) {
          return;
        }
        point_temp = x;
        x = canvas->width - y;
        y = point_temp;
        canvas_DrawAbsolutePixel(canvas, x, y, colored);
    } else if (canvas->rotate == ROTATE_180) {
        if(x < 0 || x >= canvas->width || y < 0 || y >= canvas->height) {
          return;
        }
        x = canvas->width - x;
        y = canvas->height - y;
        canvas_DrawAbsolutePixel(canvas, x, y, colored);
    } else if (canvas->rotate == ROTATE_270) {
        if(x < 0 || x >= canvas->height || y < 0 || y >= canvas->width) {
          return;
        }
        point_temp = x;
        x = y;
        y = canvas->height - point_temp;
        canvas_DrawAbsolutePixel(canvas, x, y, colored);
    }
}

/**
 *  @brief: this draws a character on the frame buffer but not refresh
 */
void canvas_DrawCharAt(Canvas* canvas, uint16_t x, uint16_t y, char ascii_char, sFONT* font, int colored) {
    int i, j;
    unsigned int char_offset = (ascii_char - ' ') * font->Height * (font->Width / 8 + (font->Width % 8 ? 1 : 0));
    const unsigned char* ptr = &font->table[char_offset];

    for (j = 0; j < font->Height; j++) {
        for (i = 0; i < font->Width; i++) {
            if (*ptr & (0x80 >> (i % 8))) {
                canvas_DrawPixel(canvas, x + i, y + j, colored);
            }
            if (i % 8 == 7) {
                ptr++;
            }
        }
        if (font->Width % 8 != 0) {
            ptr++;
        }
    }
}

/**
*  @brief: this displays a string on the frame buffer but not refresh
*/
void canvas_DrawStringAt(Canvas* canvas, uint16_t x, uint16_t y, const char* text, sFONT* font, int colored) {
    const char* p_text = text;
    unsigned int counter = 0;
    int refcolumn = x;

    /* Send the string character by character on EPD */
    while (*p_text != 0) {
        /* Display one character on EPD */
        canvas_DrawCharAt(canvas, refcolumn, y, *p_text, font, colored);
        /* Decrement the column position by 16 */
        refcolumn += font->Width;
        /* Point on the next character */
        p_text++;
        counter++;
    }
}

/**
*  @brief:
*/
void canvas_PrintEPD(Canvas* canvas, EPD_HandleTypeDef* epd){

	EPD_begin(epd);
	EPD_image_0(epd, canvas->image);
	EPD_end(epd);
}

/**
*  @brief: This updates the canvas background image
*/
void canvas_setBGImage(Canvas* canvas, const uint8_t* bg_image){
	uint16_t size = canvas->width * canvas->height;
	memcpy(canvas->image, bg_image, size);
}

void canvas_UpdateEPD(Canvas* canvas, EPD_HandleTypeDef* epd, uint8_t* old_img){

	EPD_begin(epd);
	EPD_partial_image(epd, old_img, canvas->image);
	EPD_end(epd);
}


