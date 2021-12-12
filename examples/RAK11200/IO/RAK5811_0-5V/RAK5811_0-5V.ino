/**
 * @file RAK5811_0-5V.ino
 * @author rakwireless.com
 * @brief 0 to 5V analog input example.
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
  * WisBLOCK 5811 Power On
  */
	pinMode(WB_IO1, OUTPUT | PULLUP);
	digitalWrite(WB_IO1, HIGH);

	adcAttachPin(WB_A1);	//Connect pin to ADC peripherals

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
	int sensor_pin = WB_A1; // the input pin A1 for the potentiometer
	int adc_raw = 0;
	int average_adc_raw;
	float voltage_mcu_ain;
	float voltage_sensor; // variable to store the value coming from the sensor

	for (i = 0; i < NO_OF_SAMPLES; i++)
	{
		adc_raw += analogRead(sensor_pin);
	}
	average_adc_raw = adc_raw / NO_OF_SAMPLES;

	/* Convert adc_raw to voltage in mV
	 * Func esp_adc_cal_raw_to_voltage only for attenuation == ADC_11db and sample bits == 12
	 */
	voltage_mcu_ain = esp_adc_cal_raw_to_voltage(average_adc_raw);
	voltage_sensor = voltage_mcu_ain / 0.6; 	//WisBlock RAK5811 (0 ~ 5V). Input signal reduced to 6/10 and output

  Serial.printf("-------average_value------ = %d\n", average_adc_raw);
  Serial.printf("-------voltage_sensor------ = %f\n", voltage_sensor);

	delay(2000);
}
