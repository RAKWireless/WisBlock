/**
 * @file RAK5801_4-20mA.ino
 * @author rakwireless.com
 * @brief Print current value.
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
  
  adc_init();
  adc_gpio_init(WB_A1);
  
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
    adc_select_input(1);
		mcu_ain_raw += adc_read();//analogRead(WB_A1);				// select the input pin A1 for the potentiometer
	}
	average_raw = mcu_ain_raw / NO_OF_SAMPLES;  
	voltage_ain = average_raw * 3.3 / 4095; 		//raef 3.3v / 12bit ADC  
	current_sensor = voltage_ain / 149.9 *1000; 	//WisBlock RAK5801 (0 ~ 20mA) I=U/149.9(mA)

  Serial.printf("-------current_sensor------ = %f mA\n", current_sensor);

	delay(2000);
}
