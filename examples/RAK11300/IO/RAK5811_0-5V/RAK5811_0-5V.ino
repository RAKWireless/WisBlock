/**
 * @file RAK5811_0-5V.ino
 * @author rakwireless.com
 * @brief 0 to 5V analog input example.
 * @version 0.1
 * @date 2020-07-28
 * @copyright Copyright (c) 2020
 */
 
#include <Arduino.h>

#define NO_OF_SAMPLES 32

#define ADC0  WB_A1

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

	/* WisBLOCK 5811 Power On*/
	pinMode(WB_IO1, OUTPUT);
	digitalWrite(WB_IO1, HIGH);
	/* WisBLOCK 5811 Power On*/

	pinMode(ADC0, INPUT_PULLDOWN);
	analogWriteResolution(12);

}

void loop()
{
	int i;

	int mcu_ain_raw = 0;

	int depths; 									// variable to store the value of oil depths
	int average_raw;
	float mcu_ain_voltage;
	float voltage_sensor; 							// variable to store the value coming from the sensor

	for (i = 0; i < NO_OF_SAMPLES; i++)
	{
		mcu_ain_raw += analogRead(ADC0);				// the input pin A1 for the potentiometer
	}
	average_raw = mcu_ain_raw / NO_OF_SAMPLES;

	mcu_ain_voltage = average_raw * 3.3 / 4095; 	//raef 3.3V / 12bit ADC

	voltage_sensor = mcu_ain_voltage / 0.6; 		//WisBlock RAK5811 (0 ~ 5V).   Input signal reduced to 6/10 and output

	depths = (voltage_sensor * 1000 - 574) * 2.5; 	//Convert to millivolt. 574mv is the default output from sensor

	Serial.printf("-------average_value------ = %d\n", average_raw);
	Serial.printf("-------voltage_sensor------ = %f\n", voltage_sensor);
	Serial.printf("-------depths------ = %d mm\n", depths);

	delay(2000);
}
