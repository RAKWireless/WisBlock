/**
   @file RAK12014_Distance_Detection.ino
   @author rakwireless.com
   @brief Distance detection by laser
   @version 0.1
   @date 2021-8-28
   @copyright Copyright (c) 2020
**/



#include <Wire.h>
#include <vl53l0x_class.h>      // Click to install library: http://librarymanager/All#stm32duino_vl53l0x
VL53L0X sensor_vl53l0x(&Wire, WB_IO2); 



void setup() {
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  int status;

  // Initialize serial for output.
  Serial.begin(115200);

  // Initialize I2C bus.
  Wire.begin();

  // Configure VL53L0X component.
  sensor_vl53l0x.begin();

  // Switch off VL53L0X component.
  sensor_vl53l0x.VL53L0X_Off();

  // Initialize VL53L0X component.
  status = sensor_vl53l0x.InitSensor(0x52);
  if(status)
  {
    Serial.println("Init sensor_vl53l0x failed...");
  }
}


void loop() {
  // Read Range.
  uint32_t distance;
  int status;
  status = sensor_vl53l0x.GetDistance(&distance);

  if (status == VL53L0X_ERROR_NONE)
  {
    // Output data.
    char report[64];
    snprintf(report, sizeof(report), "| Distance [mm]: %ld |", distance);
    Serial.println(report);
  }
  delay(300);
}
