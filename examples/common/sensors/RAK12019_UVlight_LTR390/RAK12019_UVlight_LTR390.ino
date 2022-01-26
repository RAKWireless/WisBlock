/**
   @file RAK12019_UVlight_LTR390.ino
   @author rakwireless.com
   @brief Example of reading ambient light or ultraviolet light data
   @version 0.1`
   @date 2021-09-13
*/
#include <Wire.h>
#include "UVlight_LTR390.h" //Click here to get the library: http://librarymanager/All#RAK12019_LTR390

//if set LTR390_MODE_ALS,get ambient light data, if set LTR390_MODE_UVS,get ultraviolet light data.

UVlight_LTR390 ltr = UVlight_LTR390();
/*
   For device under tinted window with coated-ink of flat transmission rate at 400-600nm wavelength,
   window factor  is to  compensate light  loss due to the  lower  transmission rate from the coated-ink.
      a. WFAC = 1 for NO window / clear window glass.
      b. WFAC >1 device under tinted window glass. Calibrate under white LED.
*/
void setup()
{
	pinMode(LED_BLUE, OUTPUT);
	digitalWrite(LED_BLUE, HIGH);
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
	//Sensor power switch
	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, HIGH);
	delay(300);

	Serial.println("RAK12019 test");
	Wire.begin();
	if (!ltr.init())
	{
		Serial.println("Couldn't find LTR sensor!");
		while (1)
			delay(10);
	}
	Serial.println("Found LTR390 sensor!");

	//if set LTR390_MODE_ALS,get ambient light data, if set LTR390_MODE_UVS,get ultraviolet light data.
	ltr.setMode(LTR390_MODE_ALS); //LTR390_MODE_UVS
	if (ltr.getMode() == LTR390_MODE_ALS)
	{
		Serial.println("In ALS mode");
	}
	else
	{
		Serial.println("In UVS mode");
	}

	ltr.setGain(LTR390_GAIN_3);
	Serial.print("Gain : ");
	switch (ltr.getGain())
	{
	case LTR390_GAIN_1:
		Serial.println(1);
		break;
	case LTR390_GAIN_3:
		Serial.println(3);
		break;
	case LTR390_GAIN_6:
		Serial.println(6);
		break;
	case LTR390_GAIN_9:
		Serial.println(9);
		break;
	case LTR390_GAIN_18:
		Serial.println(18);
		break;
	default:
		Serial.println("Failed to set gain");
		break;
	}
	ltr.setResolution(LTR390_RESOLUTION_16BIT);
	Serial.print("Integration Time (ms): ");
	switch (ltr.getResolution())
	{
	case LTR390_RESOLUTION_13BIT:
		Serial.println(13);
		break;
	case LTR390_RESOLUTION_16BIT:
		Serial.println(16);
		break;
	case LTR390_RESOLUTION_17BIT:
		Serial.println(17);
		break;
	case LTR390_RESOLUTION_18BIT:
		Serial.println(18);
		break;
	case LTR390_RESOLUTION_19BIT:
		Serial.println(19);
		break;
	case LTR390_RESOLUTION_20BIT:
		Serial.println(20);
		break;
	default:
		Serial.println("Failed to set Integration Time");
		break;
	}

	ltr.setThresholds(100, 1000); //Set the interrupt output threshold range for lower and upper.
	if (ltr.getMode() == LTR390_MODE_ALS)
	{
		ltr.configInterrupt(true, LTR390_MODE_ALS); //Configure the interrupt based on the thresholds in setThresholds()
	}
	else
	{
		ltr.configInterrupt(true, LTR390_MODE_UVS);
	}
}

void loop()
{
	if (ltr.newDataAvailable())
	{
		if (ltr.getMode() == LTR390_MODE_ALS)
		{
			Serial.printf("Lux Data:%0.2f-----Als Data:%d\r\n", ltr.getLUX(), ltr.readALS()); //calculate the lux
		}
		else
		{
			Serial.printf("Uvi Data:%0.2f-----Uvs Data:%d\r\n", ltr.getUVI(), ltr.readUVS());
		}
	}
	delay(500);
}
