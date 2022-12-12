/**
    @file RAK13011_MK04-1A66B_MagneticSwitch.ino
    @author rakwireless.com
    @brief Get magnetic switch status
    @version 0.1
    @date 2022-06-29
    @copyright Copyright (c) 2022
**/
#include <Arduino.h>
#define SWITCH_PIN   WB_IO1 //SlotA installation, please do not use it on SLOTB
//#define SWITCH_PIN WB_IO3 //SlotC installation.
//#define SWITCH_PIN WB_IO5 //SlotD installation.


bool interrupt_flag = false;
void setup() {
  time_t timeout = millis();
  Serial.begin(115200);
  while (!Serial) {
    if ((millis() - timeout) < 5000) {
      delay(100);
    } else {
      break;
    }
  }
  Serial.println("RAK13011 Magnetic Switch Sensor Test");

  pinMode(SWITCH_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(SWITCH_PIN), switch_interrupt, CHANGE);
}

void loop() {
  if (interrupt_flag)
  {
    interrupt_flag = false;
    if (digitalRead(SWITCH_PIN) == HIGH)
      Serial.println("Magnetic switch off");
    else
      Serial.println("Magnetic switch on");
    Serial.println();
  }
}

/*
  An interrupt is triggered when the threshold is exceeded
*/
void switch_interrupt()
{
  interrupt_flag = true;
}
