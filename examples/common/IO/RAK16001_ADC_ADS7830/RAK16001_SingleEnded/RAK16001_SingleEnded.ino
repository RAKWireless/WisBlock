/**
   @file ADS7830.ino
   @author rakwireless.com
   @brief This code is designed to config ADS7830 ADC device and handle the data
   @version 1.0
   @date 2021-08-23

   @copyright Copyright (c) 2021

*/

#include <Wire.h>
#include "ADS7830.h"   //http://librarymanager/All#RAK-ADS7830 By：RAKWireless
ADS7830 ads;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  time_t timeout = millis();
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  delay(300);
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

  // The address can be changed making the option of connecting multiple devices
  ads.getAddr_ADS7830(ADS7830_DEFAULT_ADDRESS);   // 0x48, 1001 000 (ADDR = GND)
  // ads.getAddr_ADS7830(ADS7830_VDD_ADDRESS);    // 0x49, 1001 001 (ADDR = VDD)
  // ads.getAddr_ADS7830(ADS7830_SDA_ADDRESS);    // 0x4A, 1001 010 (ADDR = SDA)
  // ads.getAddr_ADS7830(ADS7830_SCL_ADDRESS);    // 0x4B, 1001 011 (ADDR = SCL)

  // The Device operating and Power-Down mode
  // can be changed via the following functions

  //ads.setSDMode(SDMODE_DIFF);         // Differential Inputs
  ads.setSDMode(SDMODE_SINGLE);        // Single-Ended Inputs

  ads.setPDMode(PDIROFF_ADON);        // Internal Reference OFF and A/D Converter ON
  // ads.setPDMode(PDADCONV);         // Power Down Between A/D Converter Conversions
  // ads.setPDMode(PDIRON_ADOFF);     // Internal Reference ON and A/D Converter OFF
  //ads.setPDMode(PDIRON_ADON);      // Internal Reference ON and A/D Converter ON

  ads.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  byte error;
  int8_t ADS7830_Address;

  // ADS7830 Address
  ADS7830_Address = ads.ads7830_Address;

  // The i2c_scanner uses the return value of
  // the Write.endTransmisstion to see if
  // a device did acknowledge to the address.
  ads.AdsBeginTransmission(ADS7830_Address);
  error = ads.AdsEndTransmission();
  if (error == 0)
  {
    float result[8] = {0};
    Serial.println("Getting SingleEnded Readings");

    for (uint8_t channelcount = 0; channelcount < 8; channelcount++)
    {
      result[channelcount] = ads.Get_SingleEnded_Data(channelcount);
      Serial.printf("Analog input voltage values between Channel %d: %0.2f\r\n", channelcount, result[channelcount]);
    }
    Serial.println(" ");
    Serial.println("        ***************************        ");
    Serial.println(" ");
  }
  else
  {
    Serial.println("ADS7830 Disconnected!");
    Serial.println(" ");
    Serial.println("        ************        ");
    Serial.println(" ");
  }

  delay(1000);
}
