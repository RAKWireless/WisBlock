/**
 * @file BG77_Unvarnished_Transmission.ino
 * @author rakwireless.com
 * @brief bg77-at module product testing
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

#define BG77_POWER_KEY 17

void setup()
{
	// Open serial communications and wait for port to open:
	Serial.begin(115200);
	while (!Serial)
		delay(10);
	Serial.println("BG77 AT CMD TEST!");
	// Check if the modem is already awake
	time_t timeout = millis();
	bool moduleSleeps = true;
	Serial1.begin(115200);
	delay(1000);
	Serial1.println("ATI");
	//BG77 init
	while ((millis() - timeout) < 4000)
	{
		if (Serial1.available())
		{
			String result = Serial1.readString();
			Serial.println("Modem response after start:");
			Serial.println(result);
			moduleSleeps = false;
		}
	}
	if (moduleSleeps)
	{
		// Module slept, wake it up
		pinMode(BG77_POWER_KEY, OUTPUT);
		digitalWrite(BG77_POWER_KEY, 0);
		delay(1000);
		digitalWrite(BG77_POWER_KEY, 1);
		delay(2000);
		digitalWrite(BG77_POWER_KEY, 0);
		delay(1000);
	}
	Serial.println("BG77 power up!");
}

void loop()
{
	int timeout = 100;
	String resp = "";
	String snd = "";
	char cArr[128] = {0};
	while (timeout--)
	{
		if (Serial1.available() > 0)
		{
			resp += char(Serial1.read());
		}
		if (Serial.available() > 0)
		{
			snd += char(Serial.read());
		}
		delay(1);
	}
	if (resp.length() > 0)
	{
		Serial.print(resp);
	}
	if (snd.length() > 0)
	{
		memset(cArr, 0, 128);
		snd.toCharArray(cArr, snd.length() + 1);
		Serial1.write(cArr);
		delay(10);
	}
	resp = "";
	snd = "";
}
