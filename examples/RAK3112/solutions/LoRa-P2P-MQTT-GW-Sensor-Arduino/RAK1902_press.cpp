/**
 * @file RAK1902_press.cpp
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Initialize and read values from the LPS22HB sensor
 * @version 0.2
 * @date 2022-01-30
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "app.h"
#include <LPS35HW.h>

/** Sensor instance */
LPS35HW lps;

/**
 * @brief Initialize barometric pressure sensor
 *
 * @return true if sensor was found
 * @return false if initialization failed
 */
bool init_rak1902(void)
{
	Wire.begin();
	if (!lps.begin(&Wire))
	{
		MYLOG("PRESS", "Could not initialize LPS2X on Wire");
		return false;
	}

	lps.setLowPower(true);
	lps.setOutputRate(LPS35HW::OutputRate_OneShot);	   // get results on demand
	lps.setLowPassFilter(LPS35HW::LowPassFilter_ODR9); // default is off
	return true;
}

void start_rak1902(void)
{
	lps.requestOneShot(); // important to request new data before reading
	delay(250);			  // Give the sensor some time
}

/**
 * @brief Read the barometric pressure
 *     Data is added to Cayenne LPP payload as channel
 *     LPP_CHANNEL_PRESS
 *
 */
void read_rak1902(void)
{
	MYLOG("PRESS", "Reading LPS22HB");

	delay(500); // Give the sensor some time

	float pressure = lps.readPressure(); // hPa

	MYLOG("PRESS", "P: %.2f MSL: %.2f", pressure, at_MSL);

	g_solution_data.addBarometricPressure(LPP_CHANNEL_PRESS, pressure);
}

/**
 * @brief Get the rak1902 sensor data
 * 
 * @return float measured pressure
 */
float get_rak1902(void)
{
	delay(500); // Give the sensor some time

	return lps.readPressure(); // hPa
}

/**
 * @brief Calculate and return the altitude
 *        based on the barometric pressure
 *        Requires to have MSL set
 *
 * @return uint16_t altitude in cm
 */
uint16_t get_alt_rak1902(void)
{
	// Get latest values
	start_rak1902();
	delay(250);

	MYLOG("PRESS", "Compute altitude");
	// pressure in HPa
	float pressure = lps.readPressure(); // hPa
	MYLOG("PRESS", "P: %.2f", pressure);

	float A = pressure / at_MSL; // (1013.25) by default;
	float B = 1 / 5.25588;
	float C = pow(A, B);
	C = 1.0 - C;
	C = C / 0.0000225577;
	uint16_t new_val = C * 100;
	MYLOG("PRESS", "Altitude: %.2f m / %d cm", C, new_val);
	return new_val;
}
