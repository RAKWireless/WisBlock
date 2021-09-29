/**
   @file RAK12002_EEPROM_RV-3028-C7.ino
   @author rakwireless.com
   @brief Read and write RV-3028-C7 user EEPROM (43 Bytes,00h-2Ah).
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
 
  rtc.useEEPROM();  // Setup the device to use the EEPROM memory

  uint8_t eepromRegAddress = 0x11;  // User eeprom address space : [0x00 - 0x2A]
  uint8_t value = 0x42; // example
  
  rtc.writeEEPROMRegister(eepromRegAddress, value);

  Serial.printf("Writing value: 0X%x to eeprom register at address: 0X%x \n",value,eepromRegAddress);

  delay(10);
  
  uint8_t readValue = rtc.readEEPROMRegister(eepromRegAddress);
  Serial.printf("Reading eeprom register at address: 0X%x Value: 0X%x \n",eepromRegAddress,readValue);
}

/**
 * @brief Arduino loop function
 * @note We do nothing here, just love you
 * 
 */
void loop() 
{
  
}
