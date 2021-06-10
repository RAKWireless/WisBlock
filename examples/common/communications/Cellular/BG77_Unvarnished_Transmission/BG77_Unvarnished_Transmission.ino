/**
   @file BG77_Unvarnished_Transmission.ino
   @author rakwireless.com
   @brief BG77 unvarnished transmission via USB
   @version 0.1
   @date 2020-12-28
   @copyright Copyright (c) 2020
**/

#ifdef _VARIANT_RAK4630_
#include "Adafruit_TinyUSB.h"
#endif

#define BG77_POWER_KEY WB_IO1

void setup()
{
	time_t serial_timeout = millis();
	Serial.begin(115200);
	while (!Serial)
	{
		if ((millis() - serial_timeout) < 5000)
		{
            delay(100);
        }
        else
        {
            break;
        }
	}
	
	Serial.println("BG77 AT CMD TEST!");
	// Check if the modem is already awake
	time_t timeout = millis();
	bool moduleSleeps = true;
	Serial1.begin(115200);
	delay(1000);
	Serial1.println("ATI");
	//BG77 init
	while ((millis() - timeout) < 6000)
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
