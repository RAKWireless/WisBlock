/**
 * @file RAK5801_4-20mA.ino
 * @author rakwireless.com
 * @brief Read data from a pressure sensor with 4-20mA interface.
 * @version 0.1
 * @date 2020-07-28
 * @copyright Copyright (c) 2020
 */
 
#include <Arduino.h>

#define NO_OF_SAMPLES 32

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

	/* WisBLOCK 5801 Power On*/
	pinMode(WB_IO1, OUTPUT);
	digitalWrite(WB_IO1, HIGH);
	/* WisBLOCK 5801 Power On*/
}

void loop()
{
	int i;

	int mcu_ain_raw = 0;

	int pressure; 									//KPa as unit
	int average_raw;
	float voltage_ain;
	float current_sensor; 							// variable to store the value coming from the sensor

	for (i = 0; i < NO_OF_SAMPLES; i++)
	{
		mcu_ain_raw += analogRead(WB_A1);				// select the input pin A1 for the potentiometer
	}
	average_raw = mcu_ain_raw / i;

	voltage_ain = average_raw * 3.6 / 1024; 		//raef 3.6v / 10bit ADC

	current_sensor = voltage_ain / 149.9 * 1000; 	//WisBlock RAK5801 (0 ~ 20mA) I=U/149.9*1000 (mA)

	/*
	 * Convert to millivolt. 3.95mA is the default output from sensor
	 * 0.01mA == 6.25KPa
	 */
	pressure = (current_sensor - 3.95) * 100 * 2.5;

	Serial.printf("-------average_value------ = %d\n", voltage_ain);
	Serial.printf("-------current_sensor------ = %f\n", current_sensor);
	Serial.printf("-------pressure------ = %d KPa\n", pressure);

	delay(2000);
}
