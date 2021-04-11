/**
   @file bme680.ino
   @author rakwireless.com
   @brief Setup and read values from bme680 sensor
   @version 0.1
   @date 2021-01-12
   @copyright Copyright (c) 2020
**/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h" //http://librarymanager/All#Adafruit_BME680

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME680 bme;

/**
   @brief set and init bme680
*/
void bme680_init()
{
  if (!bme.begin(0x76))
  {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
  }
  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms
}

/**
   @brief  get bme680 data
*/
uint32_t bme680_read_data(double& temperature, double& pressure, double& humidity, double& gas_resist)
{
  if (!bme.performReading())
  {
    Serial.println("Failed to perform reading :(");
    return -1;
  }

  temperature = bme.temperature;
  pressure = bme.pressure / 100.0;
  humidity = bme.humidity;
  gas_resist = bme.gas_resistance / 1000.0;

  return 0;
}
