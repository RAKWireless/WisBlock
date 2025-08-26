/**
 * @file RAK12002_rtc.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Initialization and usage of RAK12002 RTC module
 * @version 0.1
 * @date 2022-02-18
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "app.h"
#include <Melopero_RV3028.h>

/** Instance of the RTC class */
Melopero_RV3028 rtc;

date_time_s g_date_time;

/**
 * @brief Initialize the RTC
 *
 * @return true if success
 * @return false if failed
 */
bool init_rak12002(void)
{
	Wire.begin();
	rtc.initI2C(Wire);

	rtc.useEEPROM(false);

	rtc.writeToRegister(0x35, 0x00);
	rtc.writeToRegister(0x37, 0xB4); // Direct Switching Mode (DSM): when VDD < VBACKUP, switchover occurs from VDD to VBACKUP

	rtc.set24HourMode(); // Set the device to use the 24hour format (default) instead of the 12 hour format

	g_date_time.year = rtc.getYear();
	g_date_time.month = rtc.getMonth();
	g_date_time.weekday = rtc.getWeekday();
	g_date_time.date = rtc.getDate();
	g_date_time.hour = rtc.getHour();
	g_date_time.minute = rtc.getMinute();
	g_date_time.second = rtc.getSecond();

	MYLOG("RTC", "%d.%02d.%02d %d:%02d:%02d", g_date_time.year, g_date_time.month, g_date_time.date, g_date_time.hour, g_date_time.minute, g_date_time.second);
	return true;
}

/**
 * @brief Set the RAK12002 date and time
 *
 * @param year in 4 digit format, e.g. 2020
 * @param month 1 to 12
 * @param date 1 to 31
 * @param hour 0 to 23
 * @param minute 0 to 59
 */
void set_rak12002(uint16_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute)
{
	uint8_t weekday = (date + (uint16_t)((2.6 * month) - 0.2) - (2 * (year / 100)) + year + (uint16_t)(year / 4) + (uint16_t)(year / 400)) % 7;
	MYLOG("RTC", "Calculated weekday is %d", weekday);
	rtc.setTime(year, month, weekday, date, hour, minute, 0);
}

/**
 * @brief Update g_data_time structure with current the date
 *        and time from the RTC
 *
 */
void read_rak12002(void)
{
	g_date_time.year = rtc.getYear();
	g_date_time.month = rtc.getMonth();
	g_date_time.weekday = rtc.getWeekday();
	g_date_time.date = rtc.getDate();
	g_date_time.hour = rtc.getHour();
	g_date_time.minute = rtc.getMinute();
	g_date_time.second = rtc.getSecond();
	MYLOG("RTC", "Got %d %d %d %02d:%02d:%d",
		  g_date_time.month, g_date_time.date, g_date_time.year,
		  g_date_time.hour, g_date_time.minute, g_date_time.second);
}