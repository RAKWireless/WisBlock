/**
   @file RAK1920_Grove_Color_TCS3472.ino
   @author rakwireless.com
   @brief Setup and read values from TCS34725 sensor
   @version 0.1
   @date 2020-12-28
   @copyright Copyright (c) 2020
**/


#include <Wire.h>
#include "Adafruit_TCS34725.h" //http://librarymanager/All#Adafruit_TCS34725

// set to false if using a common cathode LED
#define commonAnode true

// our RGB -> eye-recognized gamma color
byte gammatable[256];

Adafruit_TCS34725 g_tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

void setup()
{
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
	Serial.println("Color View Test!");

	if (g_tcs.begin())
	{
		Serial.println("Found sensor");
	}
	else
	{
		Serial.println("No TCS34725 found ... check your connections");
	}

	// thanks PhilB for this gamma table!
	// it helps convert RGB colors to what humans see
	for (int i = 0; i < 256; i++)
	{
		float x = i;
		x /= 255;
		x = pow(x, 2.5);
		x *= 255;

		if (commonAnode)
		{
			gammatable[i] = 255 - x;
		}
		else
		{
			gammatable[i] = x;
		}
	}
}

// The commented out code in loop is example of getRawData with clear value.
// Processing example colorview.pde can work with this kind of data too, but It requires manual conversion to
// [0-255] RGB value. You can still uncomments parts of colorview.pde and play with clear value.
void loop()
{
	float red, green, blue;

	g_tcs.setInterrupt(false); // turn on LED

	delay(60); // takes 50ms to read

	g_tcs.getRGB(&red, &green, &blue);

	g_tcs.setInterrupt(true); // turn off LED

	Serial.print("R:\t");
	Serial.print(int(red));
	Serial.print("\tG:\t");
	Serial.print(int(green));
	Serial.print("\tB:\t");
	Serial.print(int(blue));
	Serial.print("\n");
}
