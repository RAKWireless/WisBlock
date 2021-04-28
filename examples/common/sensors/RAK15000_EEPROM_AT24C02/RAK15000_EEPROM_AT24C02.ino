/**
 * @file RAK15000_EEPROM_AT24C02.ino
 * @author rakwireless.com
 * @brief Example code for the AT24C02 I2C EEPROM 
 * @version 0.1
 * @date 2021-01-28
 * @copyright Copyright (c) 2020
 */
 
#include <Wire.h>
#include "Adafruit_EEPROM_I2C.h"  // Click here to get the library: http://librarymanager/All#Adafruit_EEPROM_I2C

#define EEPROM_ADDR 0x50  // the default address!
#define MAXADD 262143   // max address in byte

Adafruit_EEPROM_I2C i2ceeprom;

long g_random_addr = 0;
unsigned char g_write_data = 0;
unsigned char g_read_data = 0;
unsigned char g_test_count = 5;   //set read and write EEPROM test times

void setup(void) {
  
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
  
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);   // power on for AT24C02 device
   
  delay(300);
  Serial.println("Power on.............");

  if (i2ceeprom.begin(EEPROM_ADDR))  // you can stick the new i2c addr in here, e.g. begin(0x51);
    {  
      Serial.println("Found I2C EEPROM");
    }
   else 
    {      
      while (1)
      {
        Serial.println("I2C EEPROM not identified ... check your connections?\r\n");
        delay(10);
      }     
    }

   while(g_test_count--)
   {
     g_random_addr = random(0, MAXADD);
     g_write_data = random(1,255);        
     i2ceeprom.write8(g_random_addr,g_write_data);
  
     Serial.print("Random write address is: ");
     Serial.printf("%d\n",g_random_addr);
  
     Serial.print("Random write number is: ");
     Serial.printf("%d\n",g_write_data);
  
     g_read_data = i2ceeprom.read8(g_random_addr);
     Serial.print("Read data from address is: ");   
     Serial.printf("%d\n",g_read_data);    
     if(g_read_data == g_write_data)
     {
      Serial.println("EEPROM write and read data is OK!");   
     }
     else
     {
      while(1)
      {
        Serial.println("EEPROM write and read data error! please check device !!!");   
      }    
     }
     delay(1000);
    }           
}

void loop(void) {   
   
}
