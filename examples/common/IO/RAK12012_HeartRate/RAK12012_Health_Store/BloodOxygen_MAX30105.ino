/**
   @file RAK12012_BloodOxygen_MAX30105.ino
   @author rakwireless.com
   @brief Get blood oxygen from a max30105 sensor
   @version 0.1
   @date 2021-7-28
   @copyright Copyright (c) 2020
**/

#include <Wire.h>
#include "MAX30105.h"// Click here to get the library: http://librarymanager/All#Sparkfun_3010x
#include "spo2_algorithm.h"

MAX30105 particleSensor;

#define MAX_BRIGHTNESS 255

uint32_t irBuffer[100]; //infrared LED sensor data
uint32_t redBuffer[100];  //red LED sensor data
int store_index = 0;
int32_t bufferLength; //data length
void max30105_init()
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  delay(300);
  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println(F("MAX30105 was not found. Please check wiring/power."));
    while (1);
  }
  particleSensor.setup(); //Configure sensor with default settings
}

void max30105_measure()
{
  bufferLength = 100; //buffer length of 100 stores 4 seconds of samples running at 25sps

  //read the first 100 samples, and determine the signal range
  for (byte i = 0 ; i < bufferLength ; i++)
  {
    while (particleSensor.available() == false) //do we have new data?
      particleSensor.check(); //Check the sensor for new data

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample(); //We're finished with this sample so move to next sample

//    Serial.print(F("red="));
//    Serial.print(redBuffer[i], DEC);
//    Serial.print(F(", ir="));
//    Serial.println(irBuffer[i], DEC);
  }

  //calculate heart rate and SpO2 after first 100 samples (first 4 seconds of samples)
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

  //Continuously taking samples from MAX30102.  Heart rate and SpO2 are calculated every 1 second
  while (1)
  {
    //dumping the first 25 sets of samples in the memory and shift the last 75 sets of samples to the top
    for (byte i = 25; i < 100; i++)
    {
      redBuffer[i - 25] = redBuffer[i];
      irBuffer[i - 25] = irBuffer[i];
    }

    //take 25 sets of samples before calculating the heart rate.
    for (byte i = 75; i < 100; i++)
    {
      while (particleSensor.available() == false) //do we have new data?
        particleSensor.check(); //Check the sensor for new data


      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
      particleSensor.nextSample(); //We're finished with this sample so move to next sample
      // one page is 256 bytes in flash. Store when it reaches
      // Data format:heartRate,spo2,heartRate,spo2,heartRate,spo2,heartRate,spo2,heartRate,spo2,heartRate,spo2,heartRate,spo2....
      if(validSPO2 == 1 && validHeartRate == 1)
      {
          flashBuffer[store_index]=heartRate;
          flashBuffer[store_index++]=spo2;   
      }
        //if reach 16 bytes then store one time
        if(store_index == 256)
        {
          Serial.println("Store now..."); 
          store_index = 0;
          g_flash.readBuffer(0, buf, sizeof(buf));    // Get content of the flash
          Serial.println("First 256 bytes before erasing Flash");
          for(int i=0;i<256;i++)
          {
             Serial.printf("%X ",buf[i]);
          }
          Serial.println("Erase Flash");
          g_flash.eraseChip();                // Erase the Flash content
          g_flash.waitUntilReady();

          g_flash.readBuffer(0, buf, sizeof(buf));    // Read the content to see if erasing worked
          Serial.println("First 256 bytes after erasing Flash");
          for(int i=0;i<256;i++)
          {
             Serial.printf("%X ",buf[i]);
          }
          
          Serial.println("Writing ...");        
          g_flash.writeBuffer(0, flashBuffer, 256);  // Write new data to the Flash
          g_flash.waitUntilReady();
          Serial.println("Write done");

          g_flash.readBuffer(0, buf, sizeof(buf));           // Read the content to see if write worked
          Serial.println("First 256 bytes after writing new content");
          for(int i=0;i<256;i++)
          {
             Serial.printf("%X ",buf[i]);
          }
        }
    }
    //After gathering 25 new samples recalculate HR and SP02
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
  }
}
