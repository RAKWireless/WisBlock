/**
 * @file RAK12008_gas.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Functions for RAK12008 CO2 gas sensor
 * @version 0.1
 * @date 2022-02-01
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "app.h"
#include <SparkFun_STC3x_Arduino_Library.h>

STC3x stc31_sensor;

/**
 * @brief Initialize the gas sensor
 *
 * @return true success
 * @return false failed
 */
bool init_rak12008(void)
{
	Wire.begin();
	if (!stc31_sensor.begin(0x2C))
	{
		MYLOG("STC31", "STC31 not found");
		return false;
	}

	if (stc31_sensor.setBinaryGas(STC3X_BINARY_GAS_CO2_AIR_25) == false)
	{
		MYLOG("STC31", "Could not set the binary gas!");
		return false;
	}

	// if (stc31_sensor.forcedRecalibration(0.0005) == false)
	// {
	// 	MYLOG("STC31", "Could not set the forced calibration!");
	// 	return false;
	// }

	if (stc31_sensor.enableAutomaticSelfCalibration() == false)
	{
		MYLOG("STC31", "Could not set AutomaticSelfCalibration!");
		return false;
	}

	return true;
}

/**
 * @brief Read data from the gas sensor
 *     Data is added to Cayenne LPP payload as channels
 *     LPP_CHANNEL_CO2_PERC
 *
 */
void read_rak12008(void)
{
	// Set binary gas, just to be sure.
	stc31_sensor.setBinaryGas(STC3X_BINARY_GAS_CO2_AIR_25);
	float t_h_values[3] = {0.0}; // temperature [0] & humidity [1] % pressure [2] value from T&H sensor

	if (found_sensors[ENV_ID].found_sensor)
	{
#if USE_BSEC == 0
		get_rak1906_values(t_h_values);
#else
		get_rak1906_bsec_values(t_h_values);
#endif
		MYLOG("CO2", "Rh: %.2f T: %.2f P: %.2f", t_h_values[1], t_h_values[0], t_h_values[2]);

		if ((t_h_values[0] != 0.0) && (t_h_values[1] != 0.0))
		{
			stc31_sensor.setTemperature(t_h_values[0]);
			stc31_sensor.setRelativeHumidity(t_h_values[1]);
			stc31_sensor.setPressure((uint16_t)t_h_values[2]);
		}
	}
	else
	{
		if (found_sensors[TEMP_ID].found_sensor)
		{
			get_rak1901_values(t_h_values);
			MYLOG("CO2", "Rh: %.2f T: %.2f", t_h_values[1], t_h_values[0]);

			if ((t_h_values[0] != 0.0) && (t_h_values[1] != 0.0))
			{
				stc31_sensor.setTemperature(t_h_values[0]);
				stc31_sensor.setRelativeHumidity(t_h_values[1]);
			}
		}
		if (found_sensors[PRESS_ID].found_sensor)
		{
			start_rak1902();
			delay(100);
			float pressure = get_rak1902();
			MYLOG("CO2", "P: %.2f", pressure);
			stc31_sensor.setPressure((uint16_t)pressure);
		}
	}
	if (stc31_sensor.measureGasConcentration()) // measureGasConcentration will return true when fresh data is available
	{
		float co2_perc = abs(stc31_sensor.getCO2());
		MYLOG("CO2", "CO2: %.2f", co2_perc);
		g_solution_data.addAnalogInput(LPP_CHANNEL_CO2, co2_perc); // Percent
	}
}