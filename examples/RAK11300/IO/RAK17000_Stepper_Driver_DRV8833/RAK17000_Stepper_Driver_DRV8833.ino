/**
   @file RAK17000_Stepper_Driver_DRV8833.ino
   @author rakwireless.com
   @brief Control the rotation of the stepper motor.
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

#define SPEED_DELAY 7

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
  for (int i = 0; i < 600; i++) 
  {
    PhaseA();
    delay(SPEED_DELAY);

    PhaseB();
    delay(SPEED_DELAY);

    PhaseC();
    delay(SPEED_DELAY);

    PhaseD();
    delay(SPEED_DELAY);
  }
  delay(200);
  for (int i = 0; i < 600; i++) 
  {
    PhaseD();
    delay(SPEED_DELAY);

    PhaseC();
    delay(SPEED_DELAY);

    PhaseB();
    delay(SPEED_DELAY);

    PhaseA();
    delay(SPEED_DELAY);
  }
  delay(200);
}

/**
   @brief  Move to phase A. 
*/
void PhaseA() 
{
  digitalWrite(MOTOR_AIN1, HIGH);
  digitalWrite(MOTOR_AIN2, LOW);
  digitalWrite(MOTOR_BIN1, LOW);
  digitalWrite(MOTOR_BIN2, LOW);
}

/**
   @brief  Move to phase B. 
*/
void PhaseB() 
{
  digitalWrite(MOTOR_AIN1, LOW);
  digitalWrite(MOTOR_AIN2, HIGH);
  digitalWrite(MOTOR_BIN1, LOW);
  digitalWrite(MOTOR_BIN2, LOW);
}

/**
   @brief  Move to phase C. 
*/
void PhaseC() 
{
  digitalWrite(MOTOR_AIN1, LOW);
  digitalWrite(MOTOR_AIN2, LOW);
  digitalWrite(MOTOR_BIN1, HIGH);
  digitalWrite(MOTOR_BIN2, LOW);
}

/**
   @brief  Move to phase D. 
*/
void PhaseD() 
{
  digitalWrite(MOTOR_AIN1, LOW);
  digitalWrite(MOTOR_AIN2, LOW);
  digitalWrite(MOTOR_BIN1, LOW);
  digitalWrite(MOTOR_BIN2, HIGH);
}
