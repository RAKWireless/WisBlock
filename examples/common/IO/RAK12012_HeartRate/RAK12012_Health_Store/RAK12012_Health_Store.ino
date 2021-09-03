/**
   @file RAK12012_Health_Store.ino
   @author rakwireless.com
   @brief Store health data to flash.
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

uint8_t flashBuffer[256] = {0}; // Buffer to write to Flash
uint8_t buf[256] = {0};         //Buffer to read from Flash
//Heart rate define

int32_t spo2; //SPO2 value
int8_t validSPO2; //indicator to show if the SPO2 calculation is valid
int32_t heartRate; //heart rate value
int8_t validHeartRate; //indicator to show if the heart rate calculation is valid

/**
 * @brief Arduino setup function
 * @note Called once after power on or reset
 * 
 */
void setup(void)
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
  max30105_init();

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
  Serial.println("Please put your finger on the sensor and wait for several seconds!"); // Get Flash information
}

/**
 * @brief Arduino loop function
 * @note We do nothing here in this example
 * 
 */
void loop(void)
{
  max30105_measure();
}
