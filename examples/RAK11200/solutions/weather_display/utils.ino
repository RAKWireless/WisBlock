/**
   @file utils.ino
   @author rakwireless.com
   @brief utility function
   @version 0.1
   @date 2021-01-12
   @copyright Copyright (c) 2020
**/

/**
   @brief  print current date and time like "Tuesday, January 12 2021 17:53:29"
*/
static void printLocalTime()
{
  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 10000))
  {
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  }
}
