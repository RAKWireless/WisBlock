/**
   @file RAK1920_MikroBUS_Temperature_TMP102.ino
   @author rakwireless.com
   @brief Setup and read values from a Temperature_TMP102 sensor
   @version 0.1
   @date 2020-12-28
   @copyright Copyright (c) 2020
**/

#include <Wire.h>			// Used to establied serial communication on the I2C bus
#include <SparkFunTMP102.h> // Click here to get the library: http://librarymanager/All#SparkFun_TMP102

TMP102 g_sensor0;

void setup()
{
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

	Wire.begin(); //Join I2C Bus

	/* The TMP102 uses the default settings with the address 0x48 using Wire.
	Optionally, if the address jumpers are modified, or using a different I2C bus,
	these parameters can be changed here. E.g. sensor0.begin(0x49,Wire1)
	It will return true on success or false on failure to communicate. 
	*/
	while (!g_sensor0.begin())
	{
		Serial.println("Cannot connect to TMP102.");
		Serial.println("Is the board connected? Is the device ID correct?");
		delay(1000);
	}

	Serial.println("Connected to TMP102!");
	delay(100);

	// Initialize sensor0 settings
	// set the number of consecutive faults before triggering alarm.
	// 0-3: 0:1 fault, 1:2 faults, 2:4 faults, 3:6 faults.
	g_sensor0.setFault(0); // Trigger alarm immediately

	// set the polarity of the Alarm. (0:Active LOW, 1:Active HIGH).
	g_sensor0.setAlertPolarity(1); // Active HIGH

	// set the sensor in Comparator Mode (0) or Interrupt Mode (1).
	g_sensor0.setAlertMode(0); // Comparator Mode.

	// set the Conversion Rate (how quickly the sensor gets a new reading)
	//0-3: 0:0.25Hz, 1:1Hz, 2:4Hz, 3:8Hz
	g_sensor0.setConversionRate(2);

	//set Extended Mode.
	//0:12-bit Temperature(-55C to +128C) 1:13-bit Temperature(-55C to +150C)
	g_sensor0.setExtendedMode(0);

	//set T_HIGH, the upper limit to trigger the alert on
	g_sensor0.setHighTempF(85.0); // set T_HIGH in F

	//set T_LOW, the lower limit to shut turn off the alert
	g_sensor0.setLowTempF(84.0); // set T_LOW in F
}

void loop()
{
	float temperature;
	// Turn sensor on to start temperature measurement.
	// Current consumtion typically ~10uA.
	g_sensor0.wakeup();

	// read temperature data
	//temperature = sensor0.readTempF();
	temperature = g_sensor0.readTempC();

	// Place sensor in sleep mode to save power.
	// Current consumtion typically <0.5uA.
	g_sensor0.sleep();

	// Print temperature and alarm state
	Serial.print("Temperature: ");
	Serial.println(temperature);

	delay(1000); // Wait 1000ms
}
