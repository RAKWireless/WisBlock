/**
 * @file RAK12010_light.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Functions for RAK12010 light sensor
 * @version 0.1
 * @date 2022-02-01
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "app.h"
#include "Light_VEML7700.h"

/** Light sensor instance */
Light_VEML7700 VEML = Light_VEML7700();

/**
 * @brief Initialize light sensor
 *
 * @return true success
 * @return false failed
 */
bool init_rak12010(void)
{
	Wire.begin();
	if (!VEML.begin())
	{
		MYLOG("VEML", "VEML7700 not found");
		return false;
	}
	MYLOG("VEML", "Found VEML7700");
	VEML.setGain(VEML7700_GAIN_1_8);
	VEML.setIntegrationTime(VEML7700_IT_25MS);

	// VEML.powerSaveEnable(true);
	// VEML.setPowerSaveMode(VEML7700_POWERSAVE_MODE4);
	return true;
}

/**
 * @brief Read value from light sensor
 *     Data is added to Cayenne LPP payload as channel
 *     LPP_CHANNEL_LIGHT2
 *
 */
void read_rak12010(void)
{
	float light_lux = VEML.readLux();
#if MY_DEBUG > 0
	float light_white = VEML.readWhite();
	float light_als = VEML.readALS();
	MYLOG("VEML", "L: %.2fLux W: %.2f ALS: %.2f", light_lux, light_white, light_als);
#endif

	g_solution_data.addLuminosity(LPP_CHANNEL_LIGHT2, (uint32_t)VEML.readLux());
}