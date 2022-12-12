/**
   @file RAK15007_FRAM_Write_Protect_CY15B108QN.ino
   @author rakwireless.com
   @brief use FRAM write protection function
   @version 0.1
   @date 2022-06-15
   @copyright Copyright (c) 2022
**/
#include "RAK15007_CY15B108QN.h"  //http://librarymanager/All#RAK15007_CY15B108QN

#define FRAM_WP_PIN WB_IO1   //SlotA installation, please do not use it on SLOTB
//#define FRAM_WP_PIN WB_IO3 //SlotC installation.
//#define FRAM_WP_PIN WB_IO5 //SlotD installation.

#define FRAM_8M_SIZE  0x100000
uint8_t fram_cy15b108_CS = SS;
RAK_FRAM_CY15B108QN fram_cy15b108 = RAK_FRAM_CY15B108QN(fram_cy15b108_CS);

/*
  Writing protect block for WRITE command is configured by the value of BP0 and BP1 in the status register
  data={
        0x00 None
        0x04 C0000h to FFFFFh (upper 1/4)
        0x08 80000h to FFFFFh (upper 1/2)
        0x0C 00000h to FFFFFh (all)
  }
*/
void writeBlockProtect(uint8_t data)
{
  fram_cy15b108.writeEnable(true);
  fram_cy15b108.setStatusRegister(data);
  fram_cy15b108.writeEnable(false);
}

void setup(void) {
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH); // Enable power supply.
  delay(300);
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
  Serial.println("RAK15007 FRAM_CY15B108QN TEST");
  if (fram_cy15b108.begin()) {
    Serial.println("Found FRAM_CY15B108QN");
  } else {
    Serial.println("FRAM_CY15B108QN is not connected, Please check your connections\r\n");
    while (1)
    {
      delay(500);
    }
  }

  writeBlockProtect(0x00);
  Serial.printf("registerStatus=0x%x\r\n", fram_cy15b108.getStatusRegister());

  uint32_t fram_size = fram_cy15b108.getFramSize();
  if (fram_size != 0)
  {
    Serial.print("FRAM_CY15B108QN address size is ");
    Serial.print(fram_size);
    Serial.println(" bytes");
    Serial.println("The capacity of the FRAM is");
    Serial.print(fram_size); Serial.println(" bytes");
    Serial.print(fram_size / 1024); Serial.println(" kilobytes");
    Serial.print((fram_size * 8) / 1024); Serial.println(" kilobits");
    if (fram_size >= ((1024 * 1024) / 8)) {
      Serial.print((fram_size * 8) / (1024 * 1024));
      Serial.println(" megabits");
    }
  }
  else
  {
    Serial.println("Unable to identify the device,Please check your connections");
    while (1)
    {
      delay(100);
    }
  }
  Serial.println();
}

uint32_t fram_address = 0x00; //fram address, if set all blocks protected,fram_address(0x00000~0xFFFFFF)is protected and can not to write
void loop(void) {
//  writeBlockProtect(0x0C);//0x0c:all blocks protected   0x08:80000h to FFFFFh(upper 1/2 blocks protected)  C0000h to FFFFFh (upper 1/4 blocks protected) 0x00:None
  Serial.printf("registerStatus=0x%x\r\n", fram_cy15b108.getStatusRegister());
  fram_cy15b108.writeEnable(true);
  fram_cy15b108.writeByte(fram_address, 0x01);
  fram_cy15b108.writeEnable(false);
  if (fram_cy15b108.readByte(fram_address) == 0x01)
  {
    Serial.println("Write to successful");
  }
  else
  {
    Serial.println("Write failed,The block is protected and cannot be written,Please change the write protection status");
  }

  fram_cy15b108.writeEnable(true);
  fram_cy15b108.writeByte(fram_address, 0x00);
  fram_cy15b108.writeEnable(false);
  delay(1000);
}
