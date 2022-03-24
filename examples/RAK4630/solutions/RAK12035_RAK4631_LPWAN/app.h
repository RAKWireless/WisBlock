/**
 * @file app.h
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief For application specific includes and definitions
 *        Will be included from main.h
 * @version 0.1
 * @date 2021-04-23
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef APP_H
#define APP_H

#include <Arduino.h>
/** Add you required includes after Arduino.h */
#include <Wire.h>

#include <WisBlock-API.h>  // Click here to get the library: http://librarymanager/All#WisBlock-API

#include <at_cmd.h>

// Debug output set to 0 to disable app debug output
#ifndef MY_DEBUG
#define MY_DEBUG 0
#endif

#if MY_DEBUG > 0
#define MYLOG(tag, ...)           \
	do                            \
	{                             \
		if (tag)                  \
			PRINTF("[%s] ", tag); \
		PRINTF(__VA_ARGS__);      \
		PRINTF("\n");             \
	} while (0)
#else
#define MYLOG(...)
#endif

/** Application function definitions */
void setup_app(void);
bool init_app(void);
void app_event_handler(void);
void ble_data_handler(void) __attribute__((weak));
void lora_data_handler(void);

/** Application events */
#define ACC_TRIGGER 0b1000000000000000
#define N_ACC_TRIGGER 0b0111111111111111

/** Application stuff */
extern BaseType_t g_higher_priority_task_woken;

/** Soil sensor stuff */
#include <RAK12035_SoilMoisture.h> // Click here to get the library: http://librarymanager/All#RAK12035_SoilMoisture
bool init_soil(void);
void read_soil(void);
uint16_t start_calib(bool is_dry);
void save_calib(void);
void read_calib(void);
uint16_t get_calib(bool is_dry);
extern bool has_acc;

// LoRaWan functions
struct soil_data_s
{
	uint8_t data_flag0 = 0x01; // 1
	uint8_t data_flag1 = 0x68; // 2
	uint8_t humid_1 = 0;	   // 3
	uint8_t data_flag2 = 0x02; // 4
	uint8_t data_flag3 = 0x67; // 5
	uint8_t temp_1 = 0;		   // 6
	uint8_t temp_2 = 0;		   // 7
	uint8_t data_flag4 = 0x03; // 8
	uint8_t data_flag5 = 0x02; // 9
	uint8_t batt_1 = 0;		   // 10
	uint8_t batt_2 = 0;		   // 11
	uint8_t data_flag6 = 0x04; // 12
	uint8_t data_flag7 = 0x66; // 13
	uint8_t valid = 0;		   // 14
	uint8_t data_flag8 = 0x05; // 15
	uint8_t data_flag9 = 0x02; // 16
	uint8_t cap_1 = 0;		   // 17
	uint8_t cap_2 = 0;		   // 18
};
extern soil_data_s g_soil_data;
#define SOIL_DATA_LEN 18 // sizeof(g_soil_data)

/** Battery level uinion */
union batt_s
{
	uint16_t batt16 = 0;
	uint8_t batt8[2];
};

/** Accelerometer stuff */
#include <SparkFunLIS3DH.h>
#define INT_ACC_PIN WB_IO3
bool init_acc(void);
void clear_acc_int(void);
void read_acc(void);

#endif
