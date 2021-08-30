/**
   @file RAK1920_Grove_Color_TCS3472.ino
   @author rakwireless.com
   @brief Setup and read values from TCS34725 sensor
   @version 0.1
   @date 2020-12-28
   @copyright Copyright (c) 2020
**/

#include "TCS34725.h" //http://librarymanager/All#TCS34725    by hideakitai
TCS34725 tcs;

void setup(void)
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
    Wire.begin();
    if (!tcs.attach(Wire))
        Serial.println("ERROR: TCS34725 NOT FOUND !!!");

    tcs.integrationTime(33); // ms
    tcs.gain(TCS34725::Gain::X01);

    // set LEDs...
}

void loop(void)
		{
    if (tcs.available()) // if current measurement has done
		{
        TCS34725::Color color = tcs.color();
        Serial.print("Color Temp : "); Serial.println(tcs.colorTemperature());
        Serial.print("Lux        : "); Serial.println(tcs.lux());
        Serial.print("R          : "); Serial.println(color.r);
        Serial.print("G          : "); Serial.println(color.g);
        Serial.print("B          : "); Serial.println(color.b);
}
}
