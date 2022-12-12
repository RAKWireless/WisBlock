/**
   @file RAK12047_SGP40_GetVOCIndex.ino
   @author rakwireless.com
   @brief Get the VOC index data from the sensor SGP40 and print it out through the serial port.
          The voc index can directly indicate the quality of the air. The higher the value, the worse the air quality.
   @version 0.1
   @date 2022-1-22
   @copyright Copyright (c) 2020
**/

#include <SensirionI2CSgp40.h> // Click here to get the library: http://librarymanager/All#SensirionI2CSgp40
#include <Wire.h>

SensirionI2CSgp40 sgp40;

void setup() 
{
  uint16_t error;
  char errorMessage[256];
  uint16_t serialNumber[3];
  uint8_t serialNumberSize = 3;
  
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);

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
  
  Serial.println("RAK12047 SGP40 example");

  Wire.begin();
  sgp40.begin(Wire);

  error = sgp40.getSerialNumber(serialNumber, serialNumberSize);
  if (error) 
  {
    Serial.print("Error trying to execute getSerialNumber(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  } 
  else 
  {
    Serial.print("Serial Number:");
    Serial.print("0x");
    for (size_t i = 0; i < serialNumberSize; i++) 
    {
      uint16_t value = serialNumber[i];
      Serial.print(value < 4096 ? "0" : "");
      Serial.print(value < 256 ? "0" : "");
      Serial.print(value < 16 ? "0" : "");
      Serial.print(value, HEX);
    }
    Serial.println();
  }

  uint16_t testResult;
  error = sgp40.executeSelfTest(testResult);
  if (error) 
  {
    Serial.print("Error trying to execute executeSelfTest(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  } 
  else if (testResult != 0xD400) 
  {
    Serial.print("executeSelfTest failed with error: ");
    Serial.println(testResult);
  }
}

void loop() 
{
  uint16_t  error;
  char      errorMessage[256];
  uint16_t  srawVoc   = 0;
  float     vocIndex  = 0;
  /* 
   * @brief Set the relative humidity and temperature in the current environment.
   *        Temperature and humidity calibration has been performed inside the sensor.
   *        RH/ticks=RH/%×65535/100
   *        T/ticks=(T/°C + 45)×65535/175
   */
  uint16_t  defaultRh = 0x8000;  // 50 %RH
  uint16_t  defaultT  = 0x6666;  // 25 ℃
  
  delay(1000);
  
  error = sgp40.measureRawSignal(defaultRh, defaultT, srawVoc);
  if (error) 
  {
    Serial.print("Error trying to execute measureRawSignal(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  } 
  else 
  {
    Serial.print("SRAW_VOC:");
    Serial.print(srawVoc);
    vocIndex = (float)srawVoc/131.07 ;

   /* VOC index.
    * The voc index can directly indicate the quality of the air. The higher the value, the worse the air quality.
    *   0-100，no need to ventilate,purify.
    *   100-200，no need to ventilate,purify.
    *   200-400，ventilate,purify.
    *   400-500，ventilate,purify intensely.
    */
    Serial.print("  VOC Index:");
    Serial.println(vocIndex);
  }
}
