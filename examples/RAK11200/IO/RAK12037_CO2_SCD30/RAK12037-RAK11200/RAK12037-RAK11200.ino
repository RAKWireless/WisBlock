/**
   @file RAK12037_BasicReadings_SCD30.ino
   @author rakwireless.com
   @brief  Example of reading SCD30 sensor and displaying data through serial port.
   @version 0.1
   @date 2022-1-18
   @copyright Copyright (c) 2022
**/

#include <Wire.h>

#include <Adafruit_SCD30.h> // Click here to get the library: http://librarymanager/All#Adafruit_SCD30

#ifdef NRF52
#include <Adafruit_TinyUSB.h>
#endif

Adafruit_SCD30 scd30;

void setup()
{
	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, HIGH);

	// Initialize Serial for debug output
	time_t timeout = millis();
	Serial.begin(115200);
	while (!Serial)
	{
		if ((millis() - timeout) < 5000)
		{
			delay(100);
		}
		else
		{
			break;
		}
	}

	Serial.println("SCD30 Basic Readings Example.");

	Wire.begin();

	if (scd30.begin() == false)
	{
		Serial.println("Air sensor not detected. Please check wiring. Freezing...");
		while (1)
		{
			delay(10);
		}
	}

	/*** Adjust the rate at which measurements are taken, from 2-1800 seconds */
	// if (!scd30.setMeasurementInterval(5)) {
	//   Serial.println("Failed to set measurement interval");
	//   while(1){ delay(10);}
	// }
	Serial.print("Measurement interval: ");
	Serial.print(scd30.getMeasurementInterval());
	Serial.println(" seconds");

	/*** Restart continuous measurement with a pressure offset from 700 to 1400 millibar.
	 * Giving no argument or setting the offset to 0 will disable offset correction
	 */
	// if (!scd30.startContinuousMeasurement(15)){
	//   Serial.println("Failed to set ambient pressure offset");
	//   while(1){ delay(10);}
	// }
	Serial.print("Ambient pressure offset: ");
	Serial.print(scd30.getAmbientPressureOffset());
	Serial.println(" mBar");

	/*** Set an altitude offset in meters above sea level.
	 * Offset value stored in non-volatile memory of SCD30.
	 * Setting an altitude offset will override any pressure offset.
	 */
	// if (!scd30.setAltitudeOffset(110)){
	//   Serial.println("Failed to set altitude offset");
	//   while(1){ delay(10);}
	// }
	Serial.print("Altitude offset: ");
	Serial.print(scd30.getAltitudeOffset());
	Serial.println(" meters");

	/*** Set a temperature offset in hundredths of a degree celcius.
	 * Offset value stored in non-volatile memory of SCD30.
	 */
	// if (!scd30.setTemperatureOffset(1984)){ // 19.84 degrees celcius
	//   Serial.println("Failed to set temperature offset");
	//   while(1){ delay(10);}
	// }
	Serial.print("Temperature offset: ");
	Serial.print((float)scd30.getTemperatureOffset() / 100.0);
	Serial.println(" degrees C");

	/*** Force the sensor to recalibrate with the given reference value
	 * from 400-2000 ppm. Writing a recalibration reference will overwrite
	 * any previous self calibration values.
	 * Reference value stored in non-volatile memory of SCD30.
	 */
	// if (!scd30.forceRecalibrationWithReference(400)){
	//   Serial.println("Failed to force recalibration with reference");
	//   while(1) { delay(10); }
	// }
	Serial.print("Forced Recalibration reference: ");
	Serial.print(scd30.getForcedCalibrationReference());
	Serial.println(" ppm");

	/*** Enable or disable automatic self calibration (ASC).
	 * Parameter stored in non-volatile memory of SCD30.
	 * Enabling self calibration will override any previously set
	 * forced calibration value.
	 * ASC needs continuous operation with at least 1 hour
	 * 400ppm CO2 concentration daily.
	 */
	// if (!scd30.selfCalibrationEnabled(true)){
	//   Serial.println("Failed to enable or disable self calibration");
	//   while(1) { delay(10); }
	// }
	if (scd30.selfCalibrationEnabled())
	{
		Serial.println("Self calibration enabled");
	}
	else
	{
		Serial.println("Self calibration disabled");
	}
}

void loop()
{
	if (scd30.dataReady())
	{
		if (!scd30.read())
		{
			Serial.println("Error reading sensor data");
			return;
		}
		Serial.print("co2(ppm):");
		Serial.print(scd30.CO2,3);

		Serial.print(" temp(C):");
		Serial.print(scd30.temperature, 1);

		Serial.print(" humidity(%):");
		Serial.print(scd30.relative_humidity, 1);

		Serial.println();
	}
	else
		Serial.println("Waiting for new data");

	delay(3000);
}