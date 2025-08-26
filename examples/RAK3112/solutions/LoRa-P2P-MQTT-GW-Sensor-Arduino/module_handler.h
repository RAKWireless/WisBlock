/**
 * @file module_handler.h
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Globals and defines for module handling
 * @version 0.1
 * @date 2022-02-15
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <Arduino.h>

#ifndef MODULE_HANDLER_H
#define MODULE_HANDLER_H

/** Wakeup triggers for application events */
#define VOC_REQ             0b0010000000000000
#define N_VOC_REQ           0b1101111111111111
#define BSEC_REQ            0b0000001000000000
#define N_BSEC_REQ          0b1111110111111111

typedef struct sensors_s
{
	uint8_t i2c_addr;  // I2C address
	bool found_sensor; // Flag if sensor is present
} sensors_t;

extern sensors_t found_sensors[];

// LoRaWAN stuff
#include <wisblock_cayenne.h>
// Cayenne LPP Channel numbers per sensor value
#define LPP_CHANNEL_BATT 1			   // Base Board
#define LPP_CHANNEL_HUMID 2			   // RAK1901
#define LPP_CHANNEL_TEMP 3			   // RAK1901
#define LPP_CHANNEL_PRESS 4			   // RAK1902
#define LPP_CHANNEL_LIGHT 5			   // RAK1903
#define LPP_CHANNEL_HUMID_2 6		   // RAK1906
#define LPP_CHANNEL_TEMP_2 7		   // RAK1906
#define LPP_CHANNEL_PRESS_2 8		   // RAK1906
#define LPP_CHANNEL_GAS_2 9			   // RAK1906
#define LPP_CHANNEL_GPS 10			   // RAK1910/RAK12500
#define LPP_CHANNEL_SOIL_TEMP 11	   // RAK12035
#define LPP_CHANNEL_SOIL_HUMID 12	   // RAK12035
#define LPP_CHANNEL_SOIL_HUMID_RAW 13  // RAK12035
#define LPP_CHANNEL_SOIL_VALID 14	   // RAK12035
#define LPP_CHANNEL_LIGHT2 15		   // RAK12010
#define LPP_CHANNEL_VOC 16			   // RAK12047
#define LPP_CHANNEL_GAS 17			   // RAK12004
#define LPP_CHANNEL_GAS_PERC 18		   // RAK12004
#define LPP_CHANNEL_CO2 19			   // RAK12008
#define LPP_CHANNEL_CO2_PERC 20		   // RAK12008
#define LPP_CHANNEL_ALC 21			   // RAK12009
#define LPP_CHANNEL_ALC_PERC 22		   // RAK12009
#define LPP_CHANNEL_TOF 23			   // RAK12014
#define LPP_CHANNEL_TOF_VALID 24	   // RAK12014
#define LPP_CHANNEL_GYRO 25			   // RAK12025
#define LPP_CHANNEL_GESTURE 26		   // RAK14008
#define LPP_CHANNEL_UVI 27			   // RAK12019
#define LPP_CHANNEL_UVS 28			   // RAK12019
#define LPP_CHANNEL_CURRENT_CURRENT 29 // RAK16000
#define LPP_CHANNEL_CURRENT_VOLTAGE 30 // RAK16000
#define LPP_CHANNEL_CURRENT_POWER 31   // RAK16000
#define LPP_CHANNEL_TOUCH_1 32		   // RAK14002
#define LPP_CHANNEL_TOUCH_2 33		   // RAK14002
#define LPP_CHANNEL_TOUCH_3 34		   // RAK14002
#define LPP_CHANNEL_CO2_2 35		   // RAK12037
#define LPP_CHANNEL_CO2_Temp_2 36	   // RAK12037
#define LPP_CHANNEL_CO2_HUMID_2 37	   // RAK12037
#define LPP_CHANNEL_TEMP_3 38		   // RAK12003
#define LPP_CHANNEL_TEMP_4 39		   // RAK12003
#define LPP_CHANNEL_PM_1_0 40		   // RAK12039
#define LPP_CHANNEL_PM_2_5 41		   // RAK12039
#define LPP_CHANNEL_PM_10_0 42		   // RAK12039
#define LPP_CHANNEL_EQ_EVENT 43		   // RAK12027
#define LPP_CHANNEL_EQ_SI 44		   // RAK12027
#define LPP_CHANNEL_EQ_PGA 45		   // RAK12027
#define LPP_CHANNEL_EQ_SHUTOFF 46	   // RAK12027
#define LPP_CHANNEL_EQ_COLLAPSE 47	   // RAK12027
#define LPP_CHANNEL_SWITCH 48		   // RAK13011
#define LPP_CHANNEL_WLEVEL 61		   // RAK12059
#define LPP_CHANNEL_WL_LOW 62		   // RAK12059
#define LPP_CHANNEL_WL_HIGH 63		   // RAK12059

extern WisCayenne g_solution_data;

// Index for known I2C devices
#define ACC_ID 0		  // RAK1904 accelerometer
#define LIGHT_ID 1		  // RAK1903 light sensor
#define GNSS_ID 2		  // RAK12500 GNSS sensor
#define PRESS_ID 3		  // RAK1902 barometric pressure sensor
#define TEMP_ID 4		  // RAK1901 temperature & humidity sensor
#define ENV_ID 5		  // RAK1906 environment sensor
#define SOIL_ID 6		  // RAK12035 soil moisture sensor
#define LIGHT2_ID 7		  // RAK12010 light sensor
#define EEPROM_ID 8		  // RAK15000 EEPROM module
#define MQ2_ID 9		  // RAK12004 MQ2 CO2 gas sensor
#define SCT31_ID 10		  // RAK12008 SCT31 CO2 gas sensor
#define MQ3_ID 11		  // RAK12009 MQ3 Alcohol gas sensor
#define TOF_ID 12		  // RAK12014 Laser ToF sensor
#define RTC_ID 13		  // RAK12002 RTC module
#define BAR_ID 14		  // RAK14003 LED bargraph module
#define VOC_ID 15		  // RAK12047 VOC sensor
#define GYRO_ID 16		  // RAK12025 Gyroscope
#define GESTURE_ID 17	  // RAK14008 Gesture sensor
#define OLED_ID 18		  // RAK1921 OLED display
#define UVL_ID 19		  // RAK12019 UV light sensor
#define TOUCH_ID 20		  // RAK14002 Touch Pad
#define CURRENT_ID 21	  // RAK16000 current sensor
#define MPU_ID 22		  // RAK1905 9DOF MPU9250 sensor
#define CO2_ID 23		  // RAK12037 CO2 sensor
#define FIR_ID 24		  // RAK12003 FIR temperature sensor
#define TEMP_ARR_ID 25	  // RAK12040 Temp Array sensor
#define DOF_ID 26		  // RAK12034 9DOF BMX160 sensor
#define ACC2_ID 27		  // RAK12032 ADXL313 accelerometer
#define PM_ID 28		  // RAK12039 particle matter sensor
#define SEISM_ID 29		  // RAK12027 D7S seismic sensor
#define WATER_LEVEL_ID 30 // RAK12059 Water Level sensor
#define TEMP_ARR_2_ID 41	  // RAK12040 Temp Array sensor

/** Sensor functions */
#include "RAK1901_temp.h"
#include "RAK1902_press.h"
#include "RAK1903_light.h"
#if USE_BSEC == 0
#include "RAK1906_env.h"
#else
#include "RAK1906_bsec.h"
#endif
#include "RAK12002_rtc.h"
#include "RAK12004_gas.h"
#include "RAK12008_gas.h"
#include "RAK12009_gas.h"
#include "RAK12010_light.h"
#include "RAK12019_uv.h"
#include "RAK12037_co2.h"
#include "RAK12039_pm.h"
#include "RAK12047_voc.h"

#include "user_at_cmd.h"

void find_modules(void);
void announce_modules(bool just_list = false);
void get_sensor_values(void);

#endif