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
  adc_init();
  adc_gpio_init(WB_A1);

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
	int average_raw;
	float mcu_ain_voltage;
	float voltage_sensor; 							// variable to store the value coming from the sensor

	for (i = 0; i < NO_OF_SAMPLES; i++)
	{
    adc_select_input(1);
		mcu_ain_raw += adc_read();  //analogRead(ADC0);				// the input pin A1 for the potentiometer
	}
	average_raw = mcu_ain_raw / NO_OF_SAMPLES;

	mcu_ain_voltage = average_raw * 3.3 / 4095; 	//raef 3.3V / 12bit ADC

	voltage_sensor = mcu_ain_voltage / 0.6; 		//WisBlock RAK5811 (0 ~ 5V).   Input signal reduced to 6/10 and output

	Serial.printf("-------voltage_sensor------ = %f\n", voltage_sensor);

	delay(2000);
}
