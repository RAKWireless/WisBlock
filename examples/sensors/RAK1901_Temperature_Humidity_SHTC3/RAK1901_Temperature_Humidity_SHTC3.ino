/**
   @file RAK1901_Temperature_Humidity_SHTC3.ino
   @author rakwireless.com
   @brief Setup and read values from a SHTC3 temperature and humidity sensor
   @version 0.1
   @date 2020-07-28

   @copyright Copyright (c) 2020

   @note RAK5005-O GPIO mapping to RAK4631 GPIO ports
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

#include "SparkFun_SHTC3.h" //Click here to get the library: http://librarymanager/All#SparkFun_SHTC3
SHTC3 mySHTC3;				// Declare an instance of the SHTC3 class

void setup()
{
	// Setup usb
	Serial.begin(115200);
	while (!Serial)
		;

	/* shtc3 init */
	Wire.begin();
	Serial.println("shtc3 init");
	Serial.print("Beginning sensor. Result = "); // Most SHTC3 functions return a variable of the type "SHTC3_Status_TypeDef" to indicate the status of their execution
	errorDecoder(mySHTC3.begin());				 // To start the sensor you must call "begin()", the default settings use Wire (default Arduino I2C port)
	Wire.setClock(400000);						 // The sensor is listed to work up to 1 MHz I2C speed, but the I2C clock speed is global for all sensors on that bus so using 400kHz or 100kHz is recommended
	Serial.println();

	if (mySHTC3.passIDcrc) // Whenever data is received the associated checksum is calculated and verified so you can be sure the data is true
	{					   // The checksum pass indicators are: passIDcrc, passRHcrc, and passTcrc for the ID, RH, and T readings respectively
		Serial.print("ID Passed Checksum. ");
		Serial.print("Device ID: 0b");
		Serial.println(mySHTC3.ID, BIN); // The 16-bit device ID can be accessed as a member variable of the object
	}
	else
	{
		Serial.println("ID Checksum Failed. ");
	}
}

void loop()
{
	shtc3_get();
	delay(1000);
}

void shtc3_get(void)
{
	float Temperature, Humidity;
	mySHTC3.update();
	if (mySHTC3.lastStatus == SHTC3_Status_Nominal) // You can also assess the status of the last command by checking the ".lastStatus" member of the object
	{
		/* Packing LoRa data */
		Temperature = mySHTC3.toDegC();
		Humidity = mySHTC3.toPercent();

		Serial.print("RH = ");
		Serial.print(mySHTC3.toPercent()); // "toPercent" returns the percent humidity as a floating point number
		Serial.print("% (checksum: ");
		if (mySHTC3.passRHcrc) // Like "passIDcrc" this is true when the RH value is valid from the sensor (but not necessarily up-to-date in terms of time)
		{
			Serial.print("pass");
		}
		else
		{
			Serial.print("fail");
		}
		Serial.print("), T = ");
		Serial.print(mySHTC3.toDegC()); // "toDegF" and "toDegC" return the temperature as a flaoting point number in deg F and deg C respectively
		Serial.print(" deg C (checksum: ");
		if (mySHTC3.passTcrc) // Like "passIDcrc" this is true when the T value is valid from the sensor (but not necessarily up-to-date in terms of time)
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
		errorDecoder(mySHTC3.lastStatus);
		Serial.println();
	}
}

void errorDecoder(SHTC3_Status_TypeDef message) // The errorDecoder function prints "SHTC3_Status_TypeDef" resultsin a human-friendly way
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
