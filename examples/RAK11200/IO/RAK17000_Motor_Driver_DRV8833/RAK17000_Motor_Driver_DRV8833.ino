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

// use first channel of 16 channels (started from zero)
#define LEDC_CHANNEL_0  0
#define LEDC_CHANNEL_1  1
#define LEDC_CHANNEL_2  2
#define LEDC_CHANNEL_3  3

// use 13 bit precission for LEDC timer
#define LEDC_TIMER_13_BIT  13

// use 5000 Hz as a LEDC base frequency
#define LEDC_BASE_FREQ     5000

/**
   @brief  Arduino like analogWrite. 
*/
void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255) 
{
  // calculate duty, 8191 from 2 ^ 13 - 1
  uint32_t duty = (8191 / valueMax) * min(value, valueMax);

  // write duty to LEDC
  ledcWrite(channel, duty);
}

void setup()
{
  // Enable DRV8833 output.
  pinMode(MOTOR_SLEEP, OUTPUT); 
  digitalWrite(MOTOR_SLEEP, HIGH);

  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT); // Set channel 0
  ledcSetup(LEDC_CHANNEL_1, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT); // Set channel 1
  ledcSetup(LEDC_CHANNEL_2, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT); // Set channel 2
  ledcSetup(LEDC_CHANNEL_3, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT); // Set channel 3
  
  ledcAttachPin(MOTOR_AIN1, LEDC_CHANNEL_0);
  ledcAttachPin(MOTOR_AIN2, LEDC_CHANNEL_1);
  ledcAttachPin(MOTOR_BIN1, LEDC_CHANNEL_2);
  ledcAttachPin(MOTOR_BIN2, LEDC_CHANNEL_3);
}
void loop()
{
  ledcAnalogWrite(LEDC_CHANNEL_1, 0);
  ledcAnalogWrite(LEDC_CHANNEL_3, 0); 
  for(uint8_t i = 0 ;i < 255 ;i++ ) 
  {
    ledcAnalogWrite(LEDC_CHANNEL_0, i);
    ledcAnalogWrite(LEDC_CHANNEL_2, i);
    delay(50);   
  }
  delay(100);
  for(uint8_t i = 255 ;i > 0 ;i-- )
  {
    ledcAnalogWrite(LEDC_CHANNEL_0, i);
    ledcAnalogWrite(LEDC_CHANNEL_2, i);
    delay(50);
  }
  
  ledcAnalogWrite(LEDC_CHANNEL_0, 0);
  ledcAnalogWrite(LEDC_CHANNEL_2, 0);
  for(uint8_t i = 0 ;i < 255 ;i++ )
  {
    ledcAnalogWrite(LEDC_CHANNEL_1, i);
    ledcAnalogWrite(LEDC_CHANNEL_3, i);
    delay(50);
  }
  delay(100);
  for(uint8_t i = 255 ;i > 0 ;i-- )
  {
    ledcAnalogWrite(LEDC_CHANNEL_1, i);
    ledcAnalogWrite(LEDC_CHANNEL_3, i);
    delay(50);
  }
  delay(100); 
}
