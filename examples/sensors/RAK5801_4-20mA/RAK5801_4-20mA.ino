/**
 * @file RAK5801_4-20mA.ino
 * @author rakwireless.com
 * @brief Read data from a pressure sensor with 4-20mA interface.
 * @version 0.1
 * @date 2020-07-28
 * 
 * @copyright Copyright (c) 2020
 * 
 * @note RAK5005-O GPIO mapping to RAK4631 GPIO ports
 * IO1 <-> P0.17 (Arduino GPIO number 17)
 * IO2 <-> P1.02 (Arduino GPIO number 34)
 * IO3 <-> P0.21 (Arduino GPIO number 21)
 * IO4 <-> P0.04 (Arduino GPIO number 4)
 * IO5 <-> P0.09 (Arduino GPIO number 9)
 * IO6 <-> P0.10 (Arduino GPIO number 10)
 * SW1 <-> P0.01 (Arduino GPIO number 1)
 */
#include <Arduino.h>

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);

	/*
  * WisBLOCK RAK5811 Power On
  */
	pinMode(17, OUTPUT);
	digitalWrite(17, HIGH);

	// Initialize Serial for debug output
	Serial.begin(115200);
	while (!Serial)
	{
		delay(10);
	}
}

void loop()
{
	int i;

	int sensor_pin = A1; // select the input pin for the potentiometer
	int mcu_ain_value = 0;

	int pressure; //KPa as unit
	int average_value;
	float voltage_ain;
	float current_sensor; // variable to store the value coming from the sensor

	for (i = 0; i < 3; i++)
	{
		mcu_ain_value += analogRead(sensor_pin);
	}
	average_value = mcu_ain_value / i;

	voltage_ain = average_value * 3.6 / 1024; //raef 3.6v / 10bit ADC

	current_sensor = voltage_ain / 149.9 * 1000; //WisBlock RAK5801 (0 ~ 20mA) I=U/149.9*1000 (mA)

	//Convert to millivolt. 3.95mA is the default output from sensor
	//0.01mA == 6.25KPa
	pressure = (current_sensor - 3.95) * 100 * 2.5;

	Serial.printf("-------average_value------ = %d\n", average_value);
	Serial.printf("-------current_sensor------ = %f\n", current_sensor);
	Serial.printf("-------pressure------ = %d KPa\n", pressure);

	delay(2000);
}
