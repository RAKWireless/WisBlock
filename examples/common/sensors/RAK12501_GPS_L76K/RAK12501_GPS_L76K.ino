/**
   @file RAK12501_GPS_L76K.ino
   @author rakwireless.com
   @brief get and parse gps data
   @version 0.2
   @date 2025-08-07
   @copyright Copyright (c) 2025
**/

#include <Wire.h>

#include <TinyGPSPlus.h> //http://librarymanager/All#TinyGPSPlus
TinyGPSPlus gps;

String tmp_data = "";
int direction_S_N = 0; // 0--S, 1--N
int direction_E_W = 0; // 0--E, 1--W

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

	// gps init
	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, 0);
	delay(1000);
	digitalWrite(WB_IO2, 1);
	delay(1000);
	pinMode(LED_GREEN, OUTPUT);
	digitalWrite(LED_GREEN, HIGH);
	pinMode(LED_BLUE, OUTPUT);
	digitalWrite(LED_BLUE, LOW);

	Serial1.begin(9600);
	while (!Serial1)
		;
	Serial.println("GPS uart init ok!");
}

void direction_parse(String tmp)
{
	if (tmp.indexOf(",E,") != -1)
	{
		direction_E_W = 0;
	}
	else
	{
		direction_E_W = 1;
	}

	if (tmp.indexOf(",S,") != -1)
	{
		direction_S_N = 0;
	}
	else
	{
		direction_S_N = 1;
	}
}

void loop()
{
	bool newData = false;
	// For one second we parse GPS data and report some key values
	for (unsigned long start = millis(); millis() - start < 1000;)
	{
		while (Serial1.available())
		{
			char c = Serial1.read();
			tmp_data += c;
			// Serial.write((char)c);
			if (gps.encode(c))
				newData = true;
		}
	}
	direction_parse(tmp_data);
	tmp_data = "";

	if (newData)
	{
		float flat, flon;
		if (!gps.location.isValid())
		{
			Serial.println("No location fix");
			digitalWrite(LED_GREEN, HIGH);
			digitalWrite(LED_BLUE, LOW);
		}
		else
		{
			digitalWrite(LED_GREEN, LOW);
			digitalWrite(LED_BLUE, HIGH);
			if (direction_S_N == 0)
			{
				Serial.print("(S):");
			}
			else
			{
				Serial.print("(N):");
			}
			Serial.print("LAT=");
			if (!gps.location.isValid())
			{
				flat = 0.0;
			}
			Serial.print(flat, 6);
			if (direction_E_W == 0)
			{
				Serial.print(" (E):");
			}
			else
			{
				Serial.print(" (W):");
			}
			Serial.print("LON=");
			if (!gps.location.isValid())
			{
				flon = 0.0;
			}
			Serial.print(flon, 6);
			Serial.print(" SAT=");
			if (!gps.satellites.isValid())
			{
				Serial.print("none");
			}
			else
			{
				Serial.print(gps.satellites.value());
			}
			Serial.print(" PREC=");
			if (!gps.hdop.isValid())
			{
				Serial.print("none");
			}
			else
			{
				Serial.print(gps.hdop.hdop());
			}
			Serial.println("");
		}
	}
	else
	{
		digitalWrite(LED_GREEN, HIGH);
		digitalWrite(LED_BLUE, LOW);
	}
}