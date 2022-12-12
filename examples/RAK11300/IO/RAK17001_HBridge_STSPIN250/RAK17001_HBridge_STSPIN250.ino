/**
   @file RAK17001_HBridge_STSPIN250.ino
   @author rakwireless.com
   @brief Control the one motors for forward and reverse rotation from slow to fast and then from fast to slow.
   @version 0.1
   @date 2022-05-05
   @copyright Copyright (c) 2022
**/

#include <Arduino.h>
#if defined NRF52_SERIES
#include <LoRaWan-RAK4630.h>                //http://librarymanager/All#SX126x
#elif defined ARDUINO_ARCH_RP2040
#include <LoRaWan-Arduino.h>
#elif defined ESP32
#endif
#define STSPIN250_PH           WB_IO4        //the PH pin determines the direction of the current
#define STSPIN250_PWM          WB_IO3        //the PWM pin can be used to regulate the speed of the rotation
#define STSPIN250_FAULT_CHECK  WB_IO5
#define STSPIN250_EN           WB_IO6
#define FAULT_CHECK_COUNT      4             // if four protection interrupts detected within 5 seconds, Sensor may be thermal shutdown or Overcurrent and short-circuit protections
#define FAULT_CHECK_INTERVAL   5000          //value in [ms] ,


TimerEvent_t appTimer;
static uint32_t timers_init(void);
volatile uint16_t InterruptCount = 0;
volatile int old_time = 0;
volatile int new_time = 0;
bool TimerFlag = false;

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
  Serial.println("RAK17001 Hbridege Test!");

  //creat a user timer
  uint32_t err_code;
  err_code = timers_init();
  if (err_code != 0)
  {
    Serial.printf("timers_init failed - %d\n", err_code);
    return;
  }

  pinMode(STSPIN250_PH, OUTPUT);
  digitalWrite(STSPIN250_PH, HIGH);

  pinMode(STSPIN250_PWM, OUTPUT);
  digitalWrite(STSPIN250_PWM, LOW);

  pinMode(STSPIN250_EN, OUTPUT);
  digitalWrite(STSPIN250_EN, HIGH);//if set STSPIN250_EN High,sensor work normal.if set LOW, Sensors enter low power mode.
  //   digitalWrite(STSPIN250_EN, LOW);

  pinMode(STSPIN250_FAULT_CHECK, INPUT);
  attachInterrupt(digitalPinToInterrupt(STSPIN250_FAULT_CHECK), motor_interrupt, FALLING);

  TimerSetValue(&appTimer, FAULT_CHECK_INTERVAL);
  TimerStart(&appTimer);
  delay(100);
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
        analogWrite(STSPIN250_PWM, i);
        delay(35);
      }
      delay(100);
      for (uint8_t i = 255 ; i > 0 ; i-- )
      {
        analogWrite(STSPIN250_PWM, i);
        delay(35);
      }

      digitalWrite(STSPIN250_PH, LOW);//Counterclockwise rotation of motor
      Serial.println("The motor turns Counterclockwise ");
      for (uint8_t i = 50 ; i < 255 ; i++ )
      {
        analogWrite(STSPIN250_PWM, i);
        delay(35);
      }
      delay(100);
      for (uint8_t i = 255 ; i > 0 ; i-- )
      {
        analogWrite(STSPIN250_PWM, i);
        delay(35);
      }
    }
    else
    {
      Serial.println("please exit the low-power mode ");
      delay(1000);
    }
  }
}


void motor_interrupt() {
  old_time = new_time;
  new_time = millis();
  if (new_time - old_time >= 100)
    InterruptCount++;
}


/**@brief Function for handling user timerout event.
*/
void motor_check_periodic_handler(void)
{
  TimerSetValue(&appTimer, FAULT_CHECK_INTERVAL);
  TimerStart(&appTimer);
  Serial.print("InterruptCount=");
  Serial.println(InterruptCount);
  if (InterruptCount >= FAULT_CHECK_COUNT)
  {
    Serial.print("InterruptCount1=");
    Serial.println(InterruptCount);
    TimerFlag = true;
  }
  InterruptCount = 0;
}

/**@brief Function for the Timer initialization.
   @details Initializes the timer module. This creates and starts application timers.
*/
uint32_t timers_init(void)
{
  TimerInit(&appTimer, motor_check_periodic_handler);
  return 0;
}
