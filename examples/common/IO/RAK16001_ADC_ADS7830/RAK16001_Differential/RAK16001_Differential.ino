/**
   @file ADS7830.ino
   @author rakwireless.com
   @brief This code is designed to config ADS7830 ADC device and handle the data
   @version 1.0
   @date 2021-08-23

   @copyright Copyright (c) 2021

*/

#include <Wire.h>
#include "ADS7830.h"  //http://librarymanager/All#RAK-ADS7830 By：RAKWireless

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

  //ads.setVoltageResolution(3.0);                //set reference voltage ,default 3.0V

  // The Device operating and Power-Down mode
  // can be changed via the following functions

  ads.setSDMode(SDMODE_DIFF);         // Differential Inputs
  // ads.setSDMode(SDMODE_SINGLE);    // Single-Ended Inputs


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
    float result01, result10, result23, result32, result45, result54, result67, result76;
    Serial.println("Getting Differential Readings");
    result01 = ads.Get_Differential_Data(01) ;
    Serial.printf("Analog input voltage values between channels 0 and 1: %0.2fV\r\n", result01);
    result10 = ads.Get_Differential_Data(10) ;
    Serial.printf("Analog input voltage values between Channels 1 and 0: %0.2fV\r\n", result10);
    result23 = ads.Get_Differential_Data(23);
    Serial.printf("Analog input voltage values between Channels 2 and 3: %0.2fV\r\n", result23);
    result32 = ads.Get_Differential_Data(32);
    Serial.printf("Analog input voltage values between Channels 3 and 2: %0.2fV\r\n", result32);
    result45 = ads.Get_Differential_Data(45);
    Serial.printf("Analog input voltage values between Channels 4 and 5: %0.2fV\r\n", result45);
    result54 = ads.Get_Differential_Data(54);
    Serial.printf("Analog input voltage values between Channels 5 and 4: %0.2fV\r\n", result54);
    result67 = ads.Get_Differential_Data(67);
    Serial.printf("Analog input voltage values between Channels 6 and 7: %0.2fV\r\n", result67);
    result76 = ads.Get_Differential_Data(76);
    Serial.printf("Analog input voltage values between Channels 7 and 6: %0.2fV\r\n", result76);
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
