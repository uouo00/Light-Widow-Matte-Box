/*  rtc.h
 *	Real Time Clock
 *
 *  Colton Crandell
 *  Light Widow 2020
 *
 * */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LW_RTC_H
#define LW_RTC_H


/******************************************************************************
 * INCLUDES
 *******************************************************************************/
#include <stdint.h>

#include "stm32f4xx_hal.h"

/******************************************************************************
 * DEFINES
 *******************************************************************************/

/******************************************************************************
 * TYPES
 *******************************************************************************/


typedef enum {
	RTC_ERR,
	RTC_OK,
} RTC_error;
/******************************************************************************
 * PROTOTYPES
 *******************************************************************************/

void rtcModuleInit(RTC_HandleTypeDef *hrtc);

RTC_error rtcSetTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
RTC_error rtcSetDate(uint8_t month, uint8_t day, uint16_t year, uint8_t weekDay);

RTC_error rtcGetDateTime(RTC_DateTypeDef* dateRet, RTC_TimeTypeDef* timeRet);

RTC_error rtcTest(void);

#endif // LW_RTC_H
