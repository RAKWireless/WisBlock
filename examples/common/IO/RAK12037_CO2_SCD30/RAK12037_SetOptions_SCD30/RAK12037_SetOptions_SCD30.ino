/**
   @file RAK12037_GetSettings_SCD30.ino
   @author rakwireless.com
   @brief  Example of SCD30 set options.
   @version 0.1
   @date 2022-1-18
   @copyright Copyright (c) 2022
**/

#include <Wire.h>

#include "SparkFun_SCD30_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD30
SCD30 airSensor;

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
  Serial.println("SCD30 Set Options Example.");
	
  Wire.begin();

  delay(500);
  
  if (airSensor.begin() == false)
  {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
		while (1)
		{
			delay(10);
		}
  }
	
	// Change number of seconds between measurements: 2 to 1800 (30 minutes), stored in non-volatile memory of SCD30
  airSensor.setMeasurementInterval(16); 

  // While the setting is recorded, it is not immediately available to be read.
  delay(200);

  int interval = airSensor.getMeasurementInterval();
  Serial.print("Measurement Interval: ");
  Serial.println(interval);

  // My desk is ~1600m above sealevel
  airSensor.setAltitudeCompensation(1600); // Set altitude of the sensor in m, stored in non-volatile memory of SCD30

  // Read altitude compensation value
  unsigned int altitude = airSensor.getAltitudeCompensation();
  Serial.print("Current altitude: ");
  Serial.print(altitude);
  Serial.println("m");

  // Pressure in Boulder, CO is 24.65inHg or 834.74mBar
  airSensor.setAmbientPressure(835); // Current ambient pressure in mBar: 700 to 1200, will overwrite altitude compensation

  airSensor.setTemperatureOffset(5); // Optionally we can set temperature offset to 5°C, stored in non-volatile memory of SCD30

  // Read temperature offset
  float offset = airSensor.getTemperatureOffset();
  Serial.print("Current temp offset: ");
  Serial.print(offset, 2);
  Serial.println("C");
}

void loop()
{
  if (airSensor.dataAvailable())
  {
    Serial.print("co2(ppm):");
    Serial.print(airSensor.getCO2());

    Serial.print(" temp(C):");
    Serial.print(airSensor.getTemperature(), 1);

    Serial.print(" humidity(%):");
    Serial.print(airSensor.getHumidity(), 1);

    Serial.println();
  }
  else
    Serial.print(".");

  delay(1000);
}
