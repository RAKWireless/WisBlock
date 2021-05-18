/**
   @file RAK1903_Optical_OPT3001.ino
   @author rakwireless.com
   @brief Setup and read values from a opt3001 sensor
   @version 0.1
   @date 2020-12-28
   @copyright Copyright (c) 2020
**/

#include <Wire.h>
#include <ClosedCube_OPT3001.h> // Click here to get the library: http://librarymanager/All#OPT3001

// Forward declarations for functions
void printError(String text, OPT3001_ErrorCode error);

ClosedCube_OPT3001 g_opt3001;
#define OPT3001_ADDRESS 0x44


void configureSensor()
{
  OPT3001_Config newConfig;

  newConfig.RangeNumber = B1100;
  newConfig.ConvertionTime = B0;
  newConfig.Latch = B1;
  newConfig.ModeOfConversionOperation = B11;

  OPT3001_ErrorCode errorConfig = g_opt3001.writeConfig(newConfig);
  if (errorConfig != NO_ERROR)
    printError("OPT3001 configuration", errorConfig);
  else
  {
    OPT3001_Config sensorConfig = g_opt3001.readConfig();
    Serial.println("OPT3001 Current Config:");
    Serial.println("------------------------------");

    Serial.print("Conversion ready (R):");
    Serial.println(sensorConfig.ConversionReady, HEX);

    Serial.print("Conversion time (R/W):");
    Serial.println(sensorConfig.ConvertionTime, HEX);

    Serial.print("Fault count field (R/W):");
    Serial.println(sensorConfig.FaultCount, HEX);

    Serial.print("Flag high field (R-only):");
    Serial.println(sensorConfig.FlagHigh, HEX);

    Serial.print("Flag low field (R-only):");
    Serial.println(sensorConfig.FlagLow, HEX);

    Serial.print("Latch field (R/W):");
    Serial.println(sensorConfig.Latch, HEX);

    Serial.print("Mask exponent field (R/W):");
    Serial.println(sensorConfig.MaskExponent, HEX);

    Serial.print("Mode of conversion operation (R/W):");
    Serial.println(sensorConfig.ModeOfConversionOperation, HEX);

    Serial.print("Polarity field (R/W):");
    Serial.println(sensorConfig.Polarity, HEX);

    Serial.print("Overflow flag (R-only):");
    Serial.println(sensorConfig.OverflowFlag, HEX);

    Serial.print("Range number (R/W):");
    Serial.println(sensorConfig.RangeNumber, HEX);

    Serial.println("------------------------------");
  }
}

void opt3001_read_data()
{
  OPT3001 result = g_opt3001.readResult();
  if (result.error == NO_ERROR)
  {
    Serial.print("OPT3001");
    Serial.print(": ");
    Serial.print(result.lux);
    Serial.println(" lux");

    uint16_t luminosity = result.lux;
  }
  else
  {
    printError("OPT3001", result.error);
  }
}

void printResult(String text, OPT3001 result)
{
  if (result.error == NO_ERROR)
  {
    Serial.print(text);
    Serial.print(": ");
    Serial.print(result.lux);
    Serial.println(" lux");
  }
  else
  {
    printError(text, result.error);
  }
}
void printError(String text, OPT3001_ErrorCode error)
{
  Serial.print(text);
  Serial.print(": [ERROR] Code #");
  Serial.println(error);
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

	/* opt3001 init */
	g_opt3001.begin(OPT3001_ADDRESS);
	Serial.print("OPT3001 Manufacturer ID");
	Serial.println(g_opt3001.readManufacturerID());
	Serial.print("OPT3001 Device ID");
	Serial.println(g_opt3001.readDeviceID());

	configureSensor();
	printResult("High-Limit", g_opt3001.readHighLimit());
	printResult("Low-Limit", g_opt3001.readLowLimit());
}

void loop()
{
	opt3001_read_data();
	delay(1000);
}
