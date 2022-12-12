/**
   @file RAK15006_FRAM_Information_MB85RS4MT.ino
   @author rakwireless.com
   @brief Example code to interrogate  SPI FRAM breakout for address size and storage capacity
   @version 0.1
   @date 2022-06-06
   @copyright Copyright (c) 2022
**/
#include <SPI.h>
#include "Adafruit_FRAM_SPI.h" //http://librarymanager/All#Adafruit_FRAM_SPI version:2.4.1        http://librarymanager/All#Adafruit_BusIO  version:1.11.6

#define FRAM_WP_PIN WB_IO1   //SlotA installation, please do not use it on SLOTB
//#define FRAM_WP_PIN WB_IO3 //SlotC installation.
//#define FRAM_WP_PIN WB_IO5 //SlotD installation.

/* NOTE: This sketch will overwrite data already on the FRAM breakout */
uint8_t FRAM_CS = SS;
Adafruit_FRAM_SPI fram = Adafruit_FRAM_SPI(FRAM_CS, &SPI, 10000000); // use hardware SPI

uint8_t FRAM_SCK = SCK;
uint8_t FRAM_MISO = MISO;
uint8_t FRAM_MOSI = MOSI;
//Or use software SPI, any pins!
//Adafruit_FRAM_SPI fram = Adafruit_FRAM_SPI(FRAM_SCK, FRAM_MISO, FRAM_MOSI, FRAM_CS);

uint8_t           addrSizeInBytes = 2; //Default to address size of two bytes
uint32_t          memSize;
uint8_t registerStatus = 0;

int32_t readBack(uint32_t addr, int32_t data) {
  int32_t check = !data;
  int32_t wrapCheck, backup;
  fram.read(addr, (uint8_t*)&backup, sizeof(int32_t));
  fram.writeEnable(true);
  fram.write(addr, (uint8_t*)&data, sizeof(int32_t));
  fram.writeEnable(false);
  fram.read(addr, (uint8_t*)&check, sizeof(int32_t));
  fram.read(0, (uint8_t*)&wrapCheck, sizeof(int32_t));
  fram.writeEnable(true);
  fram.write(addr, (uint8_t*)&backup, sizeof(int32_t));
  fram.writeEnable(false);
  // Check for warparound, address 0 will work anyway
  if (wrapCheck == check)
    check = 0;
  return check;
}

bool testAddrSize(uint8_t addrSize) {
  fram.setAddressSize(addrSize);
  if (readBack(4, 0xbeefbead) == 0xbeefbead)
    return true;
  return false;
}

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
  delay(300);
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

  Serial.println("RAK15006 FRAM_4M TEST");
  if (fram.begin(addrSizeInBytes)) {
    Serial.println("Found SPI FRAM");
  } else {
    Serial.println("No SPI FRAM found ... check your connections\r\n");
    while (1)
    {
      delay(500);
    }
  }
  writeBlockProtect(0x00);
  delay(1000);
  if (testAddrSize(2))
    addrSizeInBytes = 2;
  else if (testAddrSize(3))
    addrSizeInBytes = 3;  
  else if (testAddrSize(4))
    addrSizeInBytes = 4;
  else {
    Serial.println("SPI FRAM can not be read/written with any address size\r\n");
    while (1)
    {
      delay(500);
    }
  }

  memSize = 0;
  while (readBack(memSize, memSize) == memSize) {
    memSize += 256;
    //Serial.print("Block: #"); Serial.println(memSize/256);
  }

  Serial.print("SPI FRAM address size is ");
  Serial.print(addrSizeInBytes);
  Serial.println(" bytes.");
  Serial.println("SPI FRAM capacity appears to be..");
  Serial.print(memSize); Serial.println(" bytes");
  Serial.print(memSize / 0x400); Serial.println(" kilobytes");
  Serial.print((memSize * 8) / 0x400); Serial.println(" kilobits");
  if (memSize >= (0x100000 / 8)) {
    Serial.print((memSize * 8) / 0x100000); Serial.println(" megabits");
  }
}

uint8_t manufacturer_ID = 0;
uint16_t product_ID = 0;

void loop(void) {
  fram.getDeviceID(&manufacturer_ID, &product_ID);
  Serial.printf("registerStatus=0x%x,manufacturer_ID=0x%x,product_ID=0x%x\r\n", fram.getStatusRegister(), manufacturer_ID, product_ID);
  delay(2000);
}
