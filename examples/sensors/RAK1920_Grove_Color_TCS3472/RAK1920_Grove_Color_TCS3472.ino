/**
 * @file RAK1920_Grove_Color_TCS3472.ino
 * @author rakwireless.com
 * @brief Setup and read location information from a Grove TCS3472 sensor breakout
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
#include <Wire.h>
#include "Adafruit_TCS34725.h" //http://librarymanager/All#Adafruit_TCS34725

// set to false if using a common cathode LED
#define commonAnode true

// our RGB -> eye-recognized gamma color
byte gammatable[256];

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

void setup()
{
	Serial.begin(115200);
	while (!Serial)
	{
		delay(10);
	}
	Serial.println("Color View Test!");

	if (tcs.begin())
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

	tcs.setInterrupt(false); // turn on LED

	delay(60); // takes 50ms to read

	tcs.getRGB(&red, &green, &blue);

	tcs.setInterrupt(true); // turn off LED

	Serial.print("R:\t");
	Serial.print(int(red));
	Serial.print("\tG:\t");
	Serial.print(int(green));
	Serial.print("\tB:\t");
	Serial.print(int(blue));
	Serial.print("\n");
}
