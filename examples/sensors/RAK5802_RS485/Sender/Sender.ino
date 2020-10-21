/**
 * @file Sender.ino
 * @author rakwireless.com
 * @brief Sender app for RS485 interface example.
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
#include <ArduinoRS485.h> //Click here to get the library: http://librarymanager/All#ArduinoRS485

int counter = 0;

void setup()
{
	RS485.begin(9600);
}

void loop()
{
	RS485.beginTransmission();
	/* IO2 HIGH  3V3_S ON */
	pinMode(34, OUTPUT);
	digitalWrite(34, HIGH);
	delay(300);
	/* IO2 HIGH  3V3_S ON */

	RS485.write("hello world\n");
	RS485.endTransmission();
	/* IO2 LOW  3V3_S OFF */
	pinMode(34, OUTPUT);
	digitalWrite(34, LOW);
	delay(300);
	/* IO2 LOW  3V3_S OFF */

	delay(1000);
}
