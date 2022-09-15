/**
   @file RAK13007_Relay_G5LE-14-DC3.ino
   @author rakwireless.com
   @brief Withstands impulse of up to 4,500 V
   @version 0.1
   @date 2021-8-28
   @copyright Copyright (c) 2020
**/
#include <Arduino.h>
#include <Wire.h>
#ifdef NRF52_SERIES
#include <Adafruit_TinyUSB.h>
#endif

void setup() {
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  pinMode(WB_IO4, OUTPUT);
  // Initialize serial for output.
  Serial.begin(115200);
}


void loop() {
  digitalWrite(WB_IO4, LOW);
  delay(5000);
  digitalWrite(WB_IO4, HIGH);
  delay(5000);
}
