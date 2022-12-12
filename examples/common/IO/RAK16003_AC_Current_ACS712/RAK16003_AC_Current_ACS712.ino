/**
 * @file RAK16003_AC_Current_ACS712.ino
 * @author rakwireless.com
 * @brief Output current value collected by ACS712 through serial port.
 * @version 0.1
 * @date 2022-09-15
 * @copyright Copyright (c) 2022
 */

#include <Wire.h>
#include "ADC121C021.h" // Click to install library: http://librarymanager/All#RAKwireless_MQx_library

#define ACS712_ADDRESS   0x58    // The device i2c address.

float Sensitivity = 0.4; // 400mV/A or 0.4V/A.
float Vref = 2.51;

ADC121C021 ACS712;

void setup()
{
	pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH); // power on RAK16003.

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

	while (!(ACS712.begin(ACS712_ADDRESS, Wire)))
	{
		Serial.println("please check device!!!");
		delay(200);
	}
	Serial.println("ACS712 Basic Readings Example.");

  ACS712.configCycleTime(CYCLE_TIME_2048); // Set ADC121C021 Conversion Cycle Time.
  ACS712.setVoltageResolution(3.3);
}

void loop()
{
	float Vout ;
  float Current ;

	Vout = (ACS712.getSensorVoltage() / 1.5 )* 2.5;
  Serial.printf("ACS712 Vout = %4.3f(V)\r\n", Vout);

  Current =  (Vout - Vref) / Sensitivity ;
  Serial.printf("Current = %4.3f(A)\r\n", Current);
  
  delay(200);
}
