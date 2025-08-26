/**
 * @file RAK12019_uv.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Functions for RAK12019 UV light sensor
 * @version 0.1
 * @date 2022-02-20
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "app.h"
#include "UVlight_LTR390.h"

/** Light sensor instance using Wire*/
UVlight_LTR390 ltr = UVlight_LTR390();

/**
 * @brief Initialize UV light sensor
 *
 * @return true success
 * @return false failed
 */
bool init_rak12019(void)
{
	Wire.begin();
	if (!ltr.init())
	{
		MYLOG("LTR", "LTR390 not found");
		return false;
	}

	MYLOG("LTR", "Found LTR390");
	// if set LTR390_MODE_ALS,get ambient light data, if set LTR390_MODE_UVS,get ultraviolet light data.
	ltr.setMode(LTR390_MODE_UVS); // LTR390_MODE_UVS
	if (ltr.getMode() == LTR390_MODE_ALS)
	{
		MYLOG("LTR", "In ALS mode");
	}
	else
	{
		MYLOG("LTR", "In UVS mode");
	}

	// Set gain level
	ltr.setGain(LTR390_GAIN_18);

	// Set resolution
	ltr.setResolution(LTR390_RESOLUTION_20BIT);

	ltr.setThresholds(100, 1000); // Set the interrupt output threshold range for lower and upper.
	if (ltr.getMode() == LTR390_MODE_ALS)
	{
		ltr.configInterrupt(true, LTR390_MODE_ALS); // Configure the interrupt based on the thresholds in setThresholds()
	}
	else
	{
		ltr.configInterrupt(true, LTR390_MODE_UVS);
	}
	return true;
}

/**
 * @brief Read value from UV light sensor
 *     Data is added to Cayenne LPP payload as channel
 *     LPP_CHANNEL_UVI, LPP_CHANNEL_UVS
 *
 */
void read_rak12019(void)
{
	float _uvi_read = 0.0;
	uint32_t _uvs_read = 0;

	if (ltr.newDataAvailable())
	{
		if (ltr.getMode() == LTR390_MODE_ALS)
		{
			MYLOG("LTR", "Lux Data:%0.2f-----Als Data:%ld", ltr.getLUX(), ltr.readALS()); // calculate the lux
		}
		else
		{
			_uvi_read = ltr.getUVI();
			_uvs_read = ltr.readUVS();
			MYLOG("LTR", "Uvi Data:%0.2f-----Uvs Data:%ld", _uvi_read, _uvs_read);
		}
	}
	else
	{
		MYLOG("LTR", "No Data available");
	}

	g_solution_data.addAnalogInput(LPP_CHANNEL_UVI, _uvi_read);
	g_solution_data.addLuminosity(LPP_CHANNEL_UVS, _uvs_read);
}