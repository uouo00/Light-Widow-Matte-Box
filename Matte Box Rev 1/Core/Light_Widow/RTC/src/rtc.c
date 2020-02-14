/*  rtc.c
 *	Real Time Clock
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

#include "stm32f4xx_hal.h"
#include "rtc.h"
/******************************************************************************
 * DEFINES
 *******************************************************************************/

/******************************************************************************
 * MODULAR VARIABLES
 *******************************************************************************/
// Handle for the RTC
RTC_HandleTypeDef* pRtcHandle = NULL;

// Module Initialized Flag
static bool rtcModInit = false;

/******************************************************************************
 * PRIVATE FUNCTIONS
 *******************************************************************************/

/********************************************************************************
 * PUBLIC FUNCTIONS
 *******************************************************************************/
void rtcModuleInit(RTC_HandleTypeDef *hrtc) {
	// Register the Handle
	pRtcHandle = hrtc;
	rtcModInit = true;
}

bool rtcModuleIsInit( void ) {
	return rtcModInit;
}

RTC_error rtcSetTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
	// Check If Module is Initialized
	if (!rtcModInit) {
		return RTC_ERR;
	}

	RTC_TimeTypeDef tempTime;
	HAL_StatusTypeDef opReturn;

	// Load the Date Structure
	tempTime.Hours = hours;
	tempTime.Minutes = minutes;
	tempTime.Seconds = seconds;
	tempTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	tempTime.SubSeconds = 0;
	tempTime.SecondFraction = 0;
	tempTime.StoreOperation = RTC_STOREOPERATION_RESET;
	tempTime.TimeFormat = RTC_FORMAT_BIN;

	// Set the Date
	opReturn = HAL_RTC_SetTime(pRtcHandle, &tempTime, RTC_FORMAT_BIN);

	if (opReturn != HAL_OK)	{
		return RTC_ERR;
	}

	return RTC_OK;
}

RTC_error rtcSetDate(uint8_t month, uint8_t day, uint16_t year, uint8_t weekDay) {
	// Check If Module is Initialized
	if (!rtcModInit) {
		return RTC_ERR;
	}

	RTC_DateTypeDef tempDate;
	HAL_StatusTypeDef opReturn;

	// Load the Date Structure
	tempDate.Date = day;
	tempDate.Month = month;
	tempDate.WeekDay = weekDay;
	tempDate.Year = year;

	// Set the Date
	opReturn = HAL_RTC_SetDate(pRtcHandle, &tempDate, RTC_FORMAT_BIN);

	if (opReturn != HAL_OK)	{
		return RTC_ERR;
	}

	return RTC_OK;
}

RTC_error rtcGetDateTime(RTC_DateTypeDef* dateRet, RTC_TimeTypeDef* timeRet) {
	// Check If Module is Initialized
	if (!rtcModInit) {
		return RTC_ERR;
	}

	// Get the Time
	if (HAL_RTC_GetTime(pRtcHandle, timeRet, RTC_FORMAT_BIN) != HAL_OK) {
		// There was an issue communicating with the RTC
		return RTC_ERR;
	}

	// Get the Date
	if (HAL_RTC_GetDate(pRtcHandle, dateRet, RTC_FORMAT_BIN) != HAL_OK) {
		// There was an issue communicating with the RTC
		return RTC_ERR;
	}

	return RTC_OK;
}

RTC_error rtcTest(void) {
	RTC_error tmpErr;

	tmpErr = rtcSetTime(15, 34, 06);
	tmpErr = rtcSetDate(1, 25, 20, 6);

	RTC_DateTypeDef testDate;
	RTC_TimeTypeDef testTime;

	tmpErr = rtcGetDateTime(&testDate, &testTime);

	return tmpErr;
}
