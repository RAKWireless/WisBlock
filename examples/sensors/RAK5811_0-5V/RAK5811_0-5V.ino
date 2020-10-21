/**
 * @file RAK5811_0-5V.ino
 * @author rakwireless.com
 * @brief 0 to 5V analog input example.
 * @version 0.1
 * @date 2020-07-28
 * 
 * @copyright Copyright (c) 2020
 * 
 * @note RAK5005-O GPIO mapping to RAK4631 GPIO ports
   RAK5005-O <->  nRF52840
   IO1       <->  P0.17 (Arduino GPIO number 17)
   IO2       <->  P1.02 (Arduino GPIO number 34)
   IO3       <->  P0.21 (Arduino GPIO number 21)
   IO4       <->  P0.04 (Arduino GPIO number 4)
   IO5       <->  P0.09 (Arduino GPIO number 9)
   IO6       <->  P0.10 (Arduino GPIO number 10)
   SW1       <->  P0.01 (Arduino GPIO number 1)
   A0        <->  P0.04/AIN2 (Arduino Analog A2
   A1        <->  P0.31/AIN7 (Arduino Analog A7
   SPI_CS    <->  P0.26 (Arduino GPIO number 26) 
 */
#include <Arduino.h>

#define ULB6_SENSOR

void setup()
{
	/*
  * WisBLOCK 5811 Power On
  */
	pinMode(17, OUTPUT);
	digitalWrite(17, HIGH);

	pinMode(A1, INPUT_PULLDOWN);
	analogReference(AR_INTERNAL_3_0);
	analogOversampling(128);

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

	int sensor_pin = A1; // the input pin A1 for the potentiometer
	int mcu_ain_value = 0;

	int depths; // variable to store the value of oil depths
	int average_value;
	float voltage_ain;
	float voltage_sensor; // variable to store the value coming from the sensor

	for (i = 0; i < 5; i++)
	{
		mcu_ain_value += analogRead(sensor_pin);
	}
	average_value = mcu_ain_value / i;

	voltage_ain = average_value * 3.0 / 1024; //raef 3.0V / 10bit ADC

	voltage_sensor = voltage_ain / 0.6; //WisBlock RAK5811 (0 ~ 5V).   Input signal reduced to 6/10 and output

	depths = (voltage_sensor * 1000 - 574) * 2.5; //Convert to millivolt. 574mv is the default output from sensor

	Serial.printf("-------average_value------ = %d\n", average_value);
	Serial.printf("-------voltage_sensor------ = %f\n", voltage_sensor);
	Serial.printf("-------depths------ = %d mm\n", depths);

	delay(2000);
}
