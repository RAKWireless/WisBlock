/**
   @file RAK15006_FRAM_Write_Protect_MB85RS4MT.ino
   @author rakwireless.com
   @brief use FRAM write protection function
   @version 0.1
   @date 2022-06-06
   @copyright Copyright (c) 2022
**/

#include <SPI.h>
#include "Adafruit_FRAM_SPI.h" //http://librarymanager/All#Adafruit_FRAM_SPI version:2.4.1        http://librarymanager/All#Adafruit_BusIO  version:1.11.6

#define FRAM_WP_PIN WB_IO1   //SlotA installation, please do not use it on SLOTB
//#define FRAM_WP_PIN WB_IO3 //SlotC installation.
//#define FRAM_WP_PIN WB_IO5 //SlotD installation.

uint8_t FRAM_CS = SS;
Adafruit_FRAM_SPI fram = Adafruit_FRAM_SPI(FRAM_CS, &SPI, 10000000); // use hardware SPI

uint8_t FRAM_SCK = SCK;
uint8_t FRAM_MISO = MISO;
uint8_t FRAM_MOSI = MOSI;
//Or use software SPI, any pins!
//Adafruit_FRAM_SPI fram = Adafruit_FRAM_SPI(FRAM_SCK, FRAM_MISO, FRAM_MOSI, FRAM_CS);

/*
  Writing protect block for WRITE command is configured by the value of BP0 and BP1 in the status register
  data={
        0x00 None
        0x04 60000H to 7FFFFH (upper 1/4)
        0x08 40000H to 7FFFFH (upper 1/2)
        0x0C 00000H to 7FFFFH (all)
  }
*/
void writeBlockProtect(uint8_t data)
{
  fram.writeEnable(true);
  fram.setStatusRegister(data);
  fram.writeEnable(false);

}

void setup(void) {
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH); // Enable power supply.
  pinMode(FRAM_WP_PIN, OUTPUT);
  digitalWrite(FRAM_WP_PIN, HIGH);
  Serial.begin(115200);

  time_t serial_timeout = millis();
  while (!Serial)
  {
    if ((millis() - serial_timeout) < 5000)
    {
      delay(100);
    }
    else
    {
      break;
    }
  }
  Serial.println("RAK15006 FRAM_4M Write Protect Test");
  if (fram.begin()) {
    Serial.println("Found SPI FRAM");
  } else {
    Serial.println("No SPI FRAM found ... check your connections\r\n");
    while (1)
    {
      delay(500);
    }
  }
  writeBlockProtect(0x00);
  delay(100);
}

uint32_t fram_address = 0x00; //fram address, if set all blocks protected,fram_address(0x00000~0xFFFFF)is protected and can not to  write
void loop(void) {

//  writeBlockProtect(0x0C);//0x0c:all blocks protected   0x08:40000H to 7FFFFH (upper 1/2 blocks protected)  0x04:60000H to 7FFFFH (upper 1/4 blocks protected) 0x00:None
  fram.writeEnable(true);
  fram.write8(fram_address, 0x01);
  delay(10);
  if (fram.read8(fram_address) == 0x01)
  {
    Serial.println("Write to successful");
  }
  else
  {
    Serial.println("Write failed,The block is protected and cannot be written,Please change the write protection status");
  }
  fram.write8(fram_address, 0x00);
  fram.writeEnable(false);
  delay(1000);
}
