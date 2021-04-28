/**
 * @file Sender.ino
 * @author rakwireless.com
 * @brief Sender app for RS485 interface example.
 * @version 0.1
 * @date 2020-07-28
 * @copyright Copyright (c) 2020
 */
 
#include <ArduinoRS485.h> //Click here to get the library: http://librarymanager/All#ArduinoRS485

void setup()
{
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
	

	RS485.begin(9600);
}

void loop()
{
	RS485.beginTransmission();
	
	/* IO2 HIGH  3V3_S ON */
	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, HIGH);
	delay(300);
	/* IO2 HIGH  3V3_S ON */

	RS485.write("hello world\n");
	RS485.endTransmission();
	
	/* IO2 LOW  3V3_S OFF */
	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, LOW);
	delay(300);
	/* IO2 LOW  3V3_S OFF */

	delay(1000);
}
