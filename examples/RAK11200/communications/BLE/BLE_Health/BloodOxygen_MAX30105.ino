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

#include "heartRate.h"

MAX30105 particleSensor;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;

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
  long irValue = particleSensor.getIR();

  if (checkForBeat(irValue) == true)
  {
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }
  if (irValue < 50000)
  {
    Serial.print(" No finger?");
  }
  else
  {
    Serial.print("IR=");
    Serial.print(irValue);
    Serial.print(", BPM=");
    Serial.print(beatsPerMinute);
    Serial.print(", Avg BPM=");
    Serial.println(beatAvg);
    if(beatAvg >= 30)
    {
      detect_flag=1;
      memset(tmp,0,5);
      data_packet();
    }
  }
  Serial.println();
}

void data_packet()
{
  if(beatAvg > 100)
  {
    tmp[0]=beatAvg/100 +'0';
    tmp[1]=(beatAvg%100)/10 + '0';
    tmp[2]=(beatAvg%100)%10 + '0';
    len = 3;
    return;
  }
  tmp[0]=beatAvg/10 +'0';
  tmp[1]=beatAvg%10 +'0';
  len = 2;
}
