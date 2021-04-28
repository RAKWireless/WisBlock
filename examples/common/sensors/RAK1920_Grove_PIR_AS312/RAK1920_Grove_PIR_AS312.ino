/**
   @file RAK1920_Grove_PIR_AS312.ino
   @author rakwireless.com
   @brief Setup and read values from a PIR_AS312 sensor
   @version 0.1
   @date 2020-12-28
   @copyright Copyright (c) 2020
**/

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
	pinMode(WB_IO1, INPUT);
}

void loop()
{

	if (digitalRead(WB_IO1) == 1)
	{
		Serial.println("PIR Status: Sensing");
		Serial.println(" value: 1");
	}
	else
	{
		Serial.println("PIR Status: Not Sensed");
		Serial.println(" value: 0");
	}
	delay(500);
}
