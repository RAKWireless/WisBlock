/**
 * @file Receiver.ino
 * @author rakwireless.com
 * @brief Receiver app for RS485 interface example.
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
  
	/* IO2 HIGH  3V3_S ON */
	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, HIGH);
	delay(500);
	/* IO2 HIGH  3V3_S ON */
  
	// enable reception, can be disabled with: RS485.noReceive();
	RS485.receive();
}

void loop()
{
	if (RS485.available())
	{
		Serial.write(RS485.read());
	}
}
