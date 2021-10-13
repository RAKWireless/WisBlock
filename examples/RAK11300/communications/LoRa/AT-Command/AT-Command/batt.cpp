/**
 * @file batt.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Initialize and read battery status
 * @version 0.1
 * @date 2021-10-09
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "main.h"

#define PIN_VBAT WB_A0

uint32_t vbat_pin = PIN_VBAT;

#define VBAT_MV_PER_LSB (0.806F)   // 3.0V ADC range and 12 - bit ADC resolution = 3300mV / 4096
#define VBAT_DIVIDER (0.6F)		   // 1.5M + 1M voltage divider on VBAT = (1.5M / (1M + 1.5M))
#define VBAT_DIVIDER_COMP (1.846F) //  // Compensation factor for the VBAT divider

#define REAL_VBAT_MV_PER_LSB (VBAT_DIVIDER_COMP * VBAT_MV_PER_LSB)

/**
 * @brief Initialize the battery analog input
 * 
 */
void init_batt(void)
{
	// Set the resolution to 12-bit (0..4095)
	analogReadResolution(12); // Can be 8, 10, 12 or 14
}

/**
 * @brief Read the analog value from the battery analog pin
 * and convert it to milli volt
 * 
 * @return float Battery level in milli volts 0 ... 4200
 */
float read_batt(void)
{
	float raw;

	// Get the raw 12-bit, 0..3000mV ADC value
	raw = analogRead(vbat_pin);

	// Convert the raw value to compensated mv, taking the resistor-
	// divider into account (providing the actual LIPO voltage)
	// ADC range is 0..3000mV and resolution is 12-bit (0..4095)
	return raw * REAL_VBAT_MV_PER_LSB;
}

/**
 * @brief Estimate the battery level in percentage
 * from milli volts
 * 
 * @param mvolts Milli volts measured from analog pin
 * @return uint8_t Battery level as percentage (0 to 100)
 */
uint8_t mv_to_percent(float mvolts)
{
	if (mvolts < 3300)
		return 0;

	if (mvolts < 3600)
	{
		mvolts -= 3300;
		return mvolts / 30;
	}

	if (mvolts > 4200)
	{
		return 100;
	}

	mvolts -= 3600;
	return 10 + (mvolts * 0.15F); // thats mvolts /6.66666666
}

/**
 * @brief Read the battery level as value
 * between 0 and 254. This is used in LoRaWan status requests
 * as the battery level
 * 
 * @return uint8_t Battery level as value between 0 and 254
 */
uint8_t get_lora_batt(void)
{
	uint16_t read_val = 0;
	for (int i = 0; i < 10; i++)
	{
		read_val += read_batt();
	}
	return (mv_to_percent(read_val / 10) * 2.54);
}
