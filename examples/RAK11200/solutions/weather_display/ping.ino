/**
   @file ping.ino
   @author rakwireless.com
   @brief ping test function
   @version 0.1
   @date 2021-01-12
   @copyright Copyright (c) 2020
**/

//this library should install manually in IDE: Sketch->Include Library->Add .ZIP Library
#include <ESP32Ping.h> // https://github.com/marian-craciunescu/ESP32Ping

/**
   @brief  ping test function
   @param  addr: ip address or domain which can ping
*/
void ping_network_address(char *addr)
{
  bool ret = Ping.ping(addr, 5);
  if (ret == true)
  {
    Serial.println("The network is normal");
    float avg_time_ms = Ping.averageTime();
    Serial.printf("Average time of ping %s is %.1f ms.\n", addr, avg_time_ms);
  }
  else
  {
    Serial.println("The network is block.");
  }
}
