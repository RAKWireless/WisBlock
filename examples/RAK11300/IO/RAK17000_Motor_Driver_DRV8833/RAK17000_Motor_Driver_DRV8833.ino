/**
   @file RAK17000_Motor_Driver_DRV8833.ino
   @author rakwireless.com
   @brief Control the two motors for forward and reverse rotation from slow to fast and then from fast to slow.
   @version 0.1
   @date 2021-5-14
   @copyright Copyright (c) 2020
**/

#include <Arduino.h>

#define MOTOR_AIN1  SCK        
#define MOTOR_AIN2  SS     
#define MOTOR_BIN1  WB_IO3  
#define MOTOR_BIN2  MOSI   
#define MOTOR_SLEEP WB_IO6

void setup()
{
  // Enable DRV8833 output.
  pinMode(MOTOR_SLEEP, OUTPUT); 
  digitalWrite(MOTOR_SLEEP, HIGH);

  pinMode(MOTOR_AIN1, OUTPUT);
  pinMode(MOTOR_AIN2, OUTPUT);
  pinMode(MOTOR_BIN1, OUTPUT);
  pinMode(MOTOR_BIN2, OUTPUT);
}

void loop()
{
  analogWrite(MOTOR_AIN2, 0);
  analogWrite(MOTOR_BIN2, 0); 
  for(uint8_t i = 0 ;i < 255 ;i++ ) 
  {
    analogWrite(MOTOR_AIN1, i);
    analogWrite(MOTOR_BIN1, i);
    delay(50);   
  }
  delay(100);
  for(uint8_t i = 255 ;i > 0 ;i-- )
  {
    analogWrite(MOTOR_AIN1, i);
    analogWrite(MOTOR_BIN1, i);
    delay(50);
  }
  
  analogWrite(MOTOR_AIN1, 0);
  analogWrite(MOTOR_BIN1, 0);
  for(uint8_t i = 0 ;i < 255 ;i++ )
  {
    analogWrite(MOTOR_AIN2, i);
    analogWrite(MOTOR_BIN2, i);
    delay(50);
  }
  delay(100);
  for(uint8_t i = 255 ;i > 0 ;i-- )
  {
    analogWrite(MOTOR_AIN2, i);
    analogWrite(MOTOR_BIN2, i);
    delay(50);
  }
  delay(100); 
}