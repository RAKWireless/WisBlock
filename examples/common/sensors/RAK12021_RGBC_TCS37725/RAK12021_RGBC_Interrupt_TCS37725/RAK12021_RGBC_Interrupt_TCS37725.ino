/**
   @file RAK12021_RGBC_Interrupt_TCS37725.ino
   @author rakwireless.com
   @brief  If the light intensity exceeds a certain threshold, an interrupt is triggered.
   @version 0.1
   @date 2021-11-08
   @copyright Copyright (c) 2020
**/
#include <Wire.h>
#include "TCS3772.h"  // Click here to get the library: http://librarymanager/All#TCS37725

// It use WB_IO2 to power up and is conflicting with INT1, so better use in SlotA/SlotC/SlotD.

// Slot A, INT1 WB_IO1
// Slot C, INT1 WB_IO3
// Slot D, INT1 WB_IO5

#define INT    WB_IO1

TCS3772 tcs3772;

volatile boolean g_threshold_exceeded = false;

void setup() 
{
  //Sensor power switch
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, LOW); // Indicates that the light intensity sensor is interrupted and triggered

#if !defined (_VARIANT_RAK11300_) 
  Wire.begin();
#endif
  
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
  Serial.println("RAK12021 RGBC interrupt example.");
  
  if(tcs3772.begin() == true)
  {
    Serial.println("Found sensor.");
  }
  else
  {
    Serial.println("TCS37725 not found ... check your connections.");
    while(1)
    {
      delay(1000);  
    }
  }
  delay(1000);
  
  tcs3772.setClearLowThreshold(5);
  tcs3772.setClearHighThreshold(600);
  tcs3772.enableClearINT();
  tcs3772.clearAllInterrupt();

  pinMode(INT, INPUT_PULLUP);  // Connect with TCS37725 INT1.
  attachInterrupt( digitalPinToInterrupt(INT), INTCallBack, FALLING);
}

void loop() 
{  
  TCS3772_DataScaled tcs3772_data = {0};

  tcs3772_data = tcs3772.getMeasurement();
   
  Serial.print("  R: ");
  Serial.print(tcs3772_data.red);
  Serial.print("  G: ");
  Serial.print(tcs3772_data.green);
  Serial.print("  B: ");
  Serial.print(tcs3772_data.blue);
  Serial.print("  C: ");
  Serial.print(tcs3772_data.clear);
  Serial.print("  P: ");
  Serial.println(tcs3772_data.blue);
  if(g_threshold_exceeded == true)
  {
    g_threshold_exceeded = false;
    printEventSrc();
    digitalWrite(LED_GREEN, LOW);
  }
  delay(1000);
}

void INTCallBack()
{
  g_threshold_exceeded = true;
}

void printEventSrc(void)
{
  uint8_t eventSrc = tcs3772.getInterruptSrc();
  if( eventSrc & TCS3772_AINT)
  {
    Serial.println("Clear channel Interrupt event.");
    digitalWrite(LED_GREEN, HIGH);
  }
  tcs3772.clearAllInterrupt();
}
