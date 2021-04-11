/**
 * @file RAK5801_4-20mA.ino
 * @author rakwireless.com
 * @brief Read data from a pressure sensor with 4-20mA interface.
 * @version 0.1
 * @date 2020-12-18
 * 
 * @copyright Copyright (c) 2020
 */
#include <Arduino.h>
#define NO_OF_SAMPLES   128          //Multisampling

void setup()
{
	/*
  * WisBLOCK RAK5811 Power On
  */
	pinMode(WB_IO1, OUTPUT | PULLUP);
	digitalWrite(WB_IO1, HIGH);

	adcAttachPin(WB_A1);

	analogSetAttenuation(ADC_11db);
	analogReadResolution(12);

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
}

void loop()
{
	int i;

	int sensor_pin = WB_A1; // select the input pin for the potentiometer
	int mcu_ain_raw = 0;

	int pressure; //KPa as unit
	int average_adc_raw;
	float voltage_mcu_ain;  //mv as unit
	float current_sensor; // variable to store the value coming from the sensor

	for (i = 0; i < NO_OF_SAMPLES; i++)
	{
		mcu_ain_raw += analogRead(sensor_pin);
	}
	average_adc_raw = mcu_ain_raw / NO_OF_SAMPLES;

	voltage_mcu_ain = esp_adc_cal_raw_to_voltage(average_adc_raw);

	current_sensor = voltage_mcu_ain / 149.9; //WisBlock RAK5801 (0 ~ 20mA) I=U/149.9 (mA)

	//Convert to millivolt. 3.95mA is the default output from sensor
	//0.01mA == 6.25KPa
	pressure = (current_sensor - 3.95) * 100 * 2.5;
  	if (pressure < 0) 
	{
		pressure = 0;
	}

	Serial.printf("adc_raw: %d\tvoltage_mcu_ain: %f\tcurrent: %f\n", average_adc_raw, voltage_mcu_ain, current_sensor);
	Serial.printf("pressure: %d KPa\n\n", pressure);

	delay(2000);
}
