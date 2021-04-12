/**
 * @file RAK12003_FIR_MLX90632.ino
 * @author rakwireless
 * @brief This example shows how to read both the remote object temperature and the local sensor temperature. 
 * and prints out the temperature data to the Serial console. 
 * @version 0.1
 * @date 2021-01-20
 * 
 * @copyright Copyright (c) 2021
 */ 
 
#include <Wire.h>

#include "SparkFun_MLX90632_Arduino_Library.h"   // Click here to get the library: http://librarymanager/AllSparkFun_MLX90632_Arduino_Library
#define MLX90632_ADDRESS 0x3A
MLX90632 RAK_TempSensor;

void setup()
{
  TwoWire &wirePort = Wire;
  MLX90632::status returnError;  
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
  Serial.println("MLX90632 Read Example");

  Wire.begin(); //I2C init

  if (RAK_TempSensor.begin(MLX90632_ADDRESS, wirePort, returnError) == true) //MLX90632 init 
  {
     Serial.println("MLX90632 Init Succeed");
  }
  else
  {
     Serial.println("MLX90632 Init Failed");
  }
}

void loop()
{
  float object_temp = 0;
  object_temp = RAK_TempSensor.getObjectTempF(); //Get the temperature of the object we're looking at
  Serial.print("Object temperature: ");
  Serial.print(object_temp, 2);
  Serial.print(" F");

  float sensor_temp = 0;
  sensor_temp = RAK_TempSensor.getSensorTemp(); //Get the temperature of the sensor
  Serial.print(" Sensor temperature: ");
  Serial.print(sensor_temp, 2);
  Serial.print(" C");

  Serial.println();
}
