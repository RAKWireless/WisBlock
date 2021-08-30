/**
   @file RAK15001_Flash_GD25Q16C.ino
   @author rakwireless.com
   @brief Read,write and erase test GD25Q16C flash.
   @version 0.1
   @date 2021-01-26
   @copyright Copyright (c) 2021
**/


#include <Arduino.h>
#include "SdFat.h"
#include "Adafruit_SPIFlash.h"  //Click here to get the library: http://librarymanager/All#Adafruit_SPIFlash          

Adafruit_FlashTransport_SPI g_flashTransport(SS, SPI);  //Adafruit_FlashTransport_SPI flashTransport;                        

Adafruit_SPIFlash g_flash(&g_flashTransport); //Adafruit SPI Flash
SPIFlash_Device_t g_RAK15001{    
  .total_size = (1UL << 21), 
  .start_up_time_us = 5000,
  .manufacturer_id = 0xc8,  
  .memory_type = 0x40, 
  .capacity = 0x15,  
  .max_clock_speed_mhz = 15,
  .quad_enable_bit_mask = 0x00, 
  .has_sector_protection = false, 
  .supports_fast_read = true, 
  .supports_qspi = false,             
  .supports_qspi_writes = false, 
  .write_status_register_split = false,  
  .single_status_byte = true,                       
};  //Flash definition structure for GD25Q16C Flash

/**
 * @brief Arduino setup function
 * @note Called once after power on or reset
 * 
 */
void setup(void)
{
  
  uint8_t flashBuffer[16] = {0}; // Buffer to write to Flash
  uint8_t buf[16] = {0};         //Buffer to read from Flash
 
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

  Serial.println("=====================================");
  Serial.println("RAK15001 GD25Q16C Flash test");
  Serial.println("=====================================");

  for (int i = 0; i < sizeof(flashBuffer); i++)
  {
    flashBuffer[i] = i;             // Fill the buffer with some values
  }

  if (!g_flash.begin(&g_RAK15001))  // Start access to the flash
  {
    Serial.println("Flash access failed, check the settings");
  }
  g_flash.waitUntilReady();         //Wait for Flash to be ready

  Serial.print("JEDEC ID: ");
  Serial.print(g_flash.getJEDECID(), HEX);    
  Serial.print(" Flash size: ");
  Serial.print(g_flash.size());
  Serial.print(" # pages: ");
  Serial.print(g_flash.numPages());
  Serial.print(" page size: ");
  Serial.println(g_flash.pageSize()); // Get Flash information

  g_flash.readBuffer(0, buf, sizeof(buf));    // Get content of the flash
  Serial.println("First 16 bytes before erasing Flash");
  Serial.printf("%X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X\n",
          buf[0], buf[1], buf[2], buf[3],
          buf[4], buf[5], buf[6], buf[7],
          buf[8], buf[9], buf[10], buf[11],
          buf[12], buf[13], buf[14], buf[15]);

  Serial.println("Erase Flash");
  g_flash.eraseChip();                // Erase the Flash content
  g_flash.waitUntilReady();

  g_flash.readBuffer(0, buf, sizeof(buf));    // Read the content to see if erasing worked
  Serial.println("First 16 bytes after erasing Flash");
  Serial.printf("%X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X\n",
          buf[0], buf[1], buf[2], buf[3],
          buf[4], buf[5], buf[6], buf[7],
          buf[8], buf[9], buf[10], buf[11],
          buf[12], buf[13], buf[14], buf[15]);
          
  Serial.println("Writing ..."); 
  g_flash.writeBuffer(0, flashBuffer, sizeof(flashBuffer));  // Write new data to the Flash
  g_flash.waitUntilReady();
  Serial.println("Write done");

  g_flash.readBuffer(0, buf, sizeof(buf));           // Read the content to see if write worked
  Serial.println("First 16 bytes after writing new content");
  Serial.printf("%X %X %X %X %X %X %X %X %X %X %X %X %X %X %X %X\n",
          buf[0], buf[1], buf[2], buf[3],
          buf[4], buf[5], buf[6], buf[7],
          buf[8], buf[9], buf[10], buf[11],
          buf[12], buf[13], buf[14], buf[15]);
}

/**
 * @brief Arduino loop function
 * @note We do nothing here in this example
 * 
 */
void loop(void)
{
}
