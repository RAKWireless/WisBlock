/**
 * @file Receiver.ino
 * @author rakwireless.com
 * @brief Receiver app for RS485 interface example.
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
##include<ArduinoRS485.h>

	void setup()
{
	Serial.begin(9600);
	while (!Serial)
		;

	RS485.begin(9600);

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
