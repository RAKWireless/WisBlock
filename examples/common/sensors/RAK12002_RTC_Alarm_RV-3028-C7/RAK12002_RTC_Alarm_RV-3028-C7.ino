/**
   @file RAK12002_RTC_Alarm_RV-3028-C7.ino
   @author rakwireless.com
   @brief Trigger RTC interrupt at a fixed time every day.
   @version 0.1
   @date 2021-04-30
   @copyright Copyright (c) 2021
**/

#include "Melopero_RV3028.h" //http://librarymanager/All#Melopero_RV3028

Melopero_RV3028 rtc;

#define INTPIN WB_IO2 // Can modify the pins to fit your hardware connection

bool g_interruptOccurred = false; // Alarm interrupt trigger flag

/**
 * @brief Interrupt callback function
 * @note Alarm interrupt trigger callback function
 * 
 */
void interruptCallback()
{
  g_interruptOccurred = true;
}

/**
 * @brief Arduino setup function
 * @note Called once after power on or reset
 * 
 */
void setup() 
{
  // Initialize Serial for debug output
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
  
  Wire.begin();
  rtc.initI2C(); // First initialize and create the rtc device
  
  rtc.set24HourMode();  // Set the device to use the 24hour format (default) instead of the 12 hour format

  // Set the date and time
  // year, month, weekday, date, hour, minute, second
  // Note: time is always set in 24h format
  // Note: month value ranges from 1 (Jan) to 12 (Dec)
  // Note: date value ranges from 1 to 31
  rtc.setTime(2021, 4, 6, 30, 0, 0, 0);

  Serial.printf("%d:%d:%d %d/%d/%d \n",rtc.getHour(),rtc.getMinute(),rtc.getSecond(),rtc.getYear(),rtc.getMonth(),rtc.getDate());

  // Set the alarm time
  // weekdayOrDate not important, hour, minute, dateAlarm, hourAlarm, minuteAlarm, generateInterrupt
  rtc.setDateModeForAlarm(false);
  rtc.enableAlarm(0, rtc.getHour(), rtc.getMinute() + 1,false, true, true, true);

  pinMode(INTPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTPIN), interruptCallback, CHANGE);  // setup the pin to listen for the interrupt
}

/**
 * @brief Arduino loop function
 * @note Output date and time when the alarm is triggered
 * 
 */
void loop() 
{
  if (g_interruptOccurred) 
  {
    g_interruptOccurred = false;
    Serial.printf("%d:%d:%d %d/%d/%d \n",rtc.getHour(),rtc.getMinute(),rtc.getSecond(),rtc.getYear(),rtc.getMonth(),rtc.getDate());
  }
}
