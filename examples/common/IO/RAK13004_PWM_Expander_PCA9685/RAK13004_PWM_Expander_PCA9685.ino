/**
   @file RAK13004_PWM_Expander_PCA9685.ino
   @author rakwireless.com
   @brief Use IIC to expand 16 PWM. 
          Control PCA9685 channels 0~15 to output PWM with different duty cycles and time delays. 
          The frequency can be adjusted supports 24HZ to 1526HZ.
   @version 0.1
   @date 2021-3-2
   @copyright Copyright (c) 2021
**/

#include <Arduino.h>
// Install PCA9685 library by Peter Polidoro
#include "PCA9685.h"  //http://librarymanager/All#PCA9685

PCA9685 PCA9685;

void setup()
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, 1);
  
  PCA9685.setupSingleDevice(Wire,0x40);// Library using 0x40(0100 0000) i2c address, and default Wire @400kHz.

  PCA9685.setupOutputEnablePin(WB_IO6);// WB_IO6 active LOW output enable.
  PCA9685.enableOutputs(WB_IO6);

  PCA9685.setOutputsNotInverted();

  PCA9685.setToFrequency(500); // Set PWM freq to 500Hz (supports 24Hz to 1526Hz)
  
  // Set the PWM output of channel 0 .
  // PWM duty cycle = 50 % (LED on time = 50 %; LED off time = 50 %).
  // Delay time = 0 % .
  PCA9685.setChannelDutyCycle(0,  50, 0); 
  PCA9685.setChannelDutyCycle(1,  50, 25);
  PCA9685.setChannelDutyCycle(2,  50, 50);
  PCA9685.setChannelDutyCycle(3,  10, 10);
  PCA9685.setChannelDutyCycle(4,  20, 20);
  PCA9685.setChannelDutyCycle(5,  30, 30);
  PCA9685.setChannelDutyCycle(6,  40, 40);
  PCA9685.setChannelDutyCycle(7,  50, 50);
  PCA9685.setChannelDutyCycle(8,  60, 60);
  PCA9685.setChannelDutyCycle(9,  70, 70);
  PCA9685.setChannelDutyCycle(10, 80, 80);
  PCA9685.setChannelDutyCycle(11, 90, 90);
  PCA9685.setChannelDutyCycle(12, 100, 0);              
}

void loop()
{
  // Use channels 13, 14, 15 to make a simple breathing light example.
  for(uint8_t i=0 ;i < 100; i+=1)
  {
    PCA9685.setChannelDutyCycle(13, i,0);
    PCA9685.setChannelDutyCycle(14, 100-i,0);
    PCA9685.setChannelDutyCycle(15, i,0);
    delay(20);
  }
  for(uint8_t i=100 ;i >0; i-=1)
  {
    PCA9685.setChannelDutyCycle(13, i,0);
    PCA9685.setChannelDutyCycle(14, 100-i,0);
    PCA9685.setChannelDutyCycle(15, i,0);
    delay(20);
  }
}
