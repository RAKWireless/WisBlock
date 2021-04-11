/**
   @file RAK1901_Temperature_Humidity_SHTC3.ino
   @author rakwireless.com
   @brief Setup and read values from a SHTC3 temperature and humidity sensor
   @version 0.1
   @date 2020-12-28
   @copyright Copyright (c) 2020
**/


#include "SparkFun_SHTC3.h" 		//Click here to get the library: http://librarymanager/All#SparkFun_SHTC3

SHTC3 g_shtc3;						      // Declare an instance of the SHTC3 class

void errorDecoder(SHTC3_Status_TypeDef message)   // The errorDecoder function prints "SHTC3_Status_TypeDef" resultsin a human-friendly way
{
  switch (message)
  {
    case SHTC3_Status_Nominal:
      Serial.print("Nominal");
      break;
    case SHTC3_Status_Error:
      Serial.print("Error");
      break;
    case SHTC3_Status_CRC_Fail:
      Serial.print("CRC Fail");
      break;
    default:
      Serial.print("Unknown return code");
      break;
  }
}

void shtc3_read_data(void)
{
	float Temperature = 0;
	float Humidity = 0;
	
	g_shtc3.update();
	if (g_shtc3.lastStatus == SHTC3_Status_Nominal) // You can also assess the status of the last command by checking the ".lastStatus" member of the object
	{

		Temperature = g_shtc3.toDegC();			          // Packing LoRa data
		Humidity = g_shtc3.toPercent();
		
		Serial.print("RH = ");
		Serial.print(g_shtc3.toPercent()); 			      // "toPercent" returns the percent humidity as a floating point number
		Serial.print("% (checksum: ");
		
		if (g_shtc3.passRHcrc) 						            // Like "passIDcrc" this is true when the RH value is valid from the sensor (but not necessarily up-to-date in terms of time)
		{
			Serial.print("pass");
		}
		else
		{
			Serial.print("fail");
		}
		
		Serial.print("), T = ");
		Serial.print(g_shtc3.toDegC()); 			        // "toDegF" and "toDegC" return the temperature as a flaoting point number in deg F and deg C respectively
		Serial.print(" deg C (checksum: ");
		
		if (g_shtc3.passTcrc) 						            // Like "passIDcrc" this is true when the T value is valid from the sensor (but not necessarily up-to-date in terms of time)
		{
			Serial.print("pass");
		}
		else
		{
			Serial.print("fail");
		}
		Serial.println(")");
	}
	else
	{
    Serial.print("Update failed, error: ");
		errorDecoder(g_shtc3.lastStatus);
		Serial.println();
	}
}

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

	Wire.begin();
	Serial.println("shtc3 init");
	Serial.print("Beginning sensor. Result = "); // Most SHTC3 functions return a variable of the type "SHTC3_Status_TypeDef" to indicate the status of their execution
	errorDecoder(g_shtc3.begin());              // To start the sensor you must call "begin()", the default settings use Wire (default Arduino I2C port)
	Wire.setClock(400000);						          // The sensor is listed to work up to 1 MHz I2C speed, but the I2C clock speed is global for all sensors on that bus so using 400kHz or 100kHz is recommended
	Serial.println();

	if (g_shtc3.passIDcrc)                      // Whenever data is received the associated checksum is calculated and verified so you can be sure the data is true
	{					   						                    // The checksum pass indicators are: passIDcrc, passRHcrc, and passTcrc for the ID, RH, and T readings respectively
		Serial.print("ID Passed Checksum. ");
		Serial.print("Device ID: 0b");
		Serial.println(g_shtc3.ID, BIN); 		      // The 16-bit device ID can be accessed as a member variable of the object
	}
	else
	{
		Serial.println("ID Checksum Failed. ");
	}
}

void loop()
{
	shtc3_read_data();
	delay(1000);
}
