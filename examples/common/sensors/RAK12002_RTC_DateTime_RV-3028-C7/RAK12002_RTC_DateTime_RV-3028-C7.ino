/**
   @file RAK12002_RTC_DateTime_RV-3028-C7.ino
   @author rakwireless.com
   @brief Set and read RTC time.
   @version 0.1
   @date 2021-04-30
   @copyright Copyright (c) 2021
**/

#include "Melopero_RV3028.h" //http://librarymanager/All#Melopero_RV3028

Melopero_RV3028 rtc;

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
 

  rtc.writeToRegister(0x35,0x00);  
  rtc.writeToRegister(0x37,0xB4); //Direct Switching Mode (DSM): when VDD < VBACKUP, switchover occurs from VDD to VBACKUP
  
  rtc.set24HourMode();  // Set the device to use the 24hour format (default) instead of the 12 hour format

  // Set the date and time
  // year, month, weekday, date, hour, minute, second
  // Note: time is always set in 24h format
  // Note: month value ranges from 1 (Jan) to 12 (Dec)
  // Note: date value ranges from 1 to 31
  rtc.setTime(2021, 4, 6, 30, 0, 0, 0);
}

/**
 * @brief Arduino loop function
 * @note Output date and time every second
 * 
 */
void loop() 
{
  Serial.printf("%d:%d:%d %d/%d/%d \n",rtc.getHour(),rtc.getMinute(),rtc.getSecond(),rtc.getYear(),rtc.getMonth(),rtc.getDate());
  delay(1000);
}
