/**
   @file RAK12031_T_Fork_SX1041.ino
   @author rakwireless.com
   @brief TBF example.
          Can detect presence of small objects.
          Can detect movement and rotation speed of a slot disk.
   @version 0.1
   @date 2021-10-18
   @copyright Copyright (c) 2020
**/
#ifdef NRF52_SERIES
#include <Adafruit_TinyUSB.h>
#endif
// It use WB_IO2 to power up and is conflicting with singal pin, so better use in SlotA/SlotC/SlotD.
#define SINGAL_PIN          WB_IO1  // Connect with TBF out put pin.  Mount in SLOT A.
//#define SINGAL_PIN          WB_IO3  // Connect with TBF out put pin.  Mount in SLOT C.
//#define SINGAL_PIN          WB_IO5  // Connect with TBF out put pin.  Mount in SLOT D.

#define ROTATIONL_COUNT     20      // Adjust according to the code wheel used.

volatile uint32_t g_pulse_count=0;

void setup() 
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH); // Enable power supply.
  
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
  Serial.println("TBF example.");

  pinMode(SINGAL_PIN,INPUT_PULLDOWN);  // Connect with TBF out put pin.
  attachInterrupt(digitalPinToInterrupt(SINGAL_PIN) ,pulse_INT_call_back ,FALLING);  // Enable interrupts.FALLING CHANGE
  
  Serial.println("Waiting for motion.");
}

void pulse_INT_call_back()
{
  g_pulse_count++;
}

void loop() 
{
  float rotationl_speed = 0;
  Serial.print("Pulse count:");
  Serial.print(g_pulse_count);
  rotationl_speed = (float)g_pulse_count / (float)ROTATIONL_COUNT;
  g_pulse_count = 0;
  Serial.print("   ");
  Serial.print("Rotating speed:");
  Serial.print(rotationl_speed);
  Serial.print("[r/s]");
  Serial.print("   ");
  if( digitalRead(SINGAL_PIN) == LOW )
  {
    Serial.println("Object exists.");
  }
  else
  {
    Serial.println("Object not exists.");
  }
  delay(1000);
}
