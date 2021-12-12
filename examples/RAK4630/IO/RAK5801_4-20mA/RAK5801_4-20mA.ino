/**
 * @file RAK5801_4-20mA.ino
 * @author rakwireless.com
 * @brief Print current value.
 * @version 0.2
 * @date 2020-07-28
 * @copyright Copyright (c) 2020
 */
 
#include <Arduino.h>
#include <Wire.h>
#ifdef _VARIANT_RAK4630_
#include <Adafruit_TinyUSB.h>
#endif

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
	int average_raw;
	float voltage_ain;
	float current_sensor; 							// variable to store the value coming from the sensor

	for (i = 0; i < NO_OF_SAMPLES; i++)
	{
		mcu_ain_raw += analogRead(WB_A1);				// select the input pin A1 for the potentiometer
	}
	average_raw = mcu_ain_raw / i;

	voltage_ain = average_raw * 3.6 / 1024; 		//raef 3.6v / 10bit ADC

	current_sensor = voltage_ain / 149.9*1000; 	//WisBlock RAK5801 (0 ~ 20mA) I=U/149.9(mA)

	Serial.printf("-------current_sensor------ = %f mA\n", current_sensor);

	delay(2000);
}
