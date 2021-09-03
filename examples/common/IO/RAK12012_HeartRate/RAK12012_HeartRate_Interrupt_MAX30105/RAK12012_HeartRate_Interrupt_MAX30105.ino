/**
   @file RAK12012_HeartRate_Interrupt_MAX30105.ino
   @author rakwireless.com
   @brief Get heart rate from a max30105 sensor and show the interrupt
   @version 0.1
   @date 2021-7-28
   @copyright Copyright (c) 2020
**/

#include <Wire.h>
#include "MAX30105.h"     // Click here to get the library: http://librarymanager/All#Sparkfun_3010x

#include "heartRate.h"

MAX30105 particleSensor;

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;

void setup()
{
  //define the INT
  pinMode(WB_IO4, INPUT);  
  delay(1000);  
  Serial.begin(115200);
  Serial.println("Initializing...");
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  particleSensor.setup();
  delay(1000);
  //clear the irq
  particleSensor.readRegister8(MAX30105_ADDRESS,0x00);
  particleSensor.writeRegister8(MAX30105_ADDRESS,0x02,0x40);
  Serial.println("Place your index finger on the sensor with steady pressure.");
  attachInterrupt(digitalPinToInterrupt(WB_IO4), blink, CHANGE);
}
void blink() {
  if(digitalRead(LED_BLUE)==HIGH)
    digitalWrite(LED_BLUE, LOW);
  else
    digitalWrite(LED_BLUE, HIGH);
}
void loop()
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
  }
  Serial.println();
  particleSensor.readRegister8(MAX30105_ADDRESS,0x00);
}
