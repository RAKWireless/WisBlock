/**
   @file RAK17001_HBridge_STSPIN250.ino
   @author rakwireless.com
   @brief Control the one motors for forward and reverse rotation from slow to fast and then from fast to slow.
   @version 0.1
   @date 2022-05-05
   @copyright Copyright (c) 2022
**/

#include <Arduino.h>

#define STSPIN250_PH           WB_IO4        //the PH pin determines the direction of the current
#define STSPIN250_PWM          WB_IO3        //the PWM pin can be used to regulate the speed of the rotation
#define STSPIN250_FAULT_CHECK  WB_IO5
#define STSPIN250_EN           WB_IO6

#define FAULT_CHECK_COUNT      4             // if four protection interrupts detected within 5 seconds, Sensor may be thermal shutdown or Overcurrent and short-circuit protections
volatile uint16_t  InterruptCount = 0;
volatile int old_time = 0;
volatile int new_time = 0;
hw_timer_t *tim1 = NULL;
bool TimerFlag = false;
// use first channel of 16 channels (started from zero)
#define LEDC_CHANNEL_0  0

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
  //Sensor power control
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  delay(300);

  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_BLUE, LOW);
  Serial.begin(115200);
  time_t timeout = millis();
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
  Serial.println("RAK17001 Test!");

  tim1 = timerBegin(0, 80, true);
  timerAttachInterrupt(tim1, tim1Interrupt, true);
  timerAlarmWrite(tim1, 500000ul, true);
  timerAlarmEnable(tim1);

  pinMode(STSPIN250_PH, OUTPUT);
  digitalWrite(STSPIN250_PH, HIGH);

  pinMode(STSPIN250_PWM, OUTPUT);
  digitalWrite(STSPIN250_PWM, LOW);

  pinMode(STSPIN250_EN, OUTPUT);
  digitalWrite(STSPIN250_EN, HIGH);//if set STSPIN250_EN High,sensor work normal.if set LOW, Sensors enter low power mode.

  pinMode(STSPIN250_FAULT_CHECK, INPUT);
  attachInterrupt(digitalPinToInterrupt(STSPIN250_FAULT_CHECK), motor_interrupt, FALLING);
  delay(300);

  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT); // Set channel 0

  ledcAttachPin(STSPIN250_PWM, LEDC_CHANNEL_0);
}
void loop()
{
  if (TimerFlag)//Thermal shutdown or Overcurrent and short-circuit protections
  {
    InterruptCount = 0;
    digitalWrite(LED_BLUE, HIGH);
    Serial.println("Sensor thermal shutdown or Overcurrent and short-circuit protections,Please check it！！！");
    delay(500);
    return;
  }
  else
  {
    if (digitalRead(STSPIN250_EN) == 1)
    {
      digitalWrite(STSPIN250_PH, HIGH);  //The motor rotates clockwise
      Serial.println("The motor turns clockwise");
      for (uint8_t i = 50 ; i < 255 ; i++ )
      {
        ledcAnalogWrite(LEDC_CHANNEL_0, i);
        delay(50);
      }
      delay(100);
      for (uint8_t i = 255 ; i > 0 ; i-- )
      {
        ledcAnalogWrite(LEDC_CHANNEL_0, i);
        delay(50);
      }

      digitalWrite(STSPIN250_PH, LOW);//Counterclockwise rotation of motor
      Serial.println("The motor turns Counterclockwise ");
      for (uint8_t i = 50 ; i < 255 ; i++ )
      {
        ledcAnalogWrite(LEDC_CHANNEL_0, i);
        delay(50);
      }
      delay(100);
      for (uint8_t i = 255 ; i > 0 ; i-- )
      {
        ledcAnalogWrite(LEDC_CHANNEL_0, i);
        delay(50);
      }
    }
    else
    {
      Serial.println("please exit the low-power mode ");
      delay(1000);
    }
  }
}

void tim1Interrupt()
{
  if (InterruptCount)
  {
    if (InterruptCount >= FAULT_CHECK_COUNT)  // if three protection interrupts detected within 5 seconds, Sensor may be thermal shutdown or Overcurrent and short-circuit protections
    {
      TimerFlag = true;
      //    Serial.print("timerAlarmEnabled=");
      //    Serial.println(timerAlarmEnabled(tim1));
    }
  }
}

void motor_interrupt() {
  old_time = new_time;
  new_time = millis();
  if (new_time - old_time >= 100)
    InterruptCount++;
}
