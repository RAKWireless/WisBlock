/**
 * @file AT_Command_Test.ino
 * @author rakwireless.com
 * @brief ESP32 AT command example
 * @version 0.1
 * @date 2020-08-21
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

void setup()
{
	// Open serial communications and wait for port to open:
	Serial.begin(115200);
	while (!Serial)
		delay(10);

	Serial.println("================================");
	Serial.println("ESP32 AT CMD TEST!");
	Serial.println("================================");

	//esp32 init
	Serial1.begin(115200);
}

void loop()
{
	uint8_t resp;
	uint8_t snd;

	while (Serial1.available() > 0)
	{
		resp = (uint8_t)(Serial1.read());
		Serial.write(resp);
	}

	while (Serial.available() > 0)
	{
		snd = (uint8_t)(Serial.read());
		Serial1.write(snd);
	}
}
