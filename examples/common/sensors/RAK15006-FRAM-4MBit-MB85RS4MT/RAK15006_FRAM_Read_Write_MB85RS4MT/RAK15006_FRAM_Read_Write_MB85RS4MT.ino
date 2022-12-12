/**
   @file RAK15006_FRAM_Read_Write_MB85RS4MT.ino
   @author rakwireless.com
   @brief FRAM_4M Read And Write Test
   @version 0.1
   @date 2022-06-06
   @copyright Copyright (c) 2022
**/

#include <SPI.h>
#include "Adafruit_FRAM_SPI.h" //http://librarymanager/All#Adafruit_FRAM_SPI version:2.4.1        http://librarymanager/All#Adafruit_BusIO  version:1.11.6

#define FRAM_WP_PIN WB_IO1   //SlotA installation, please do not use it on SLOTB
//#define FRAM_WP_PIN WB_IO3 //SlotC installation.
//#define FRAM_WP_PIN WB_IO5 //SlotD installation.

#define FRAM_4M_SIZE  0x80000

/* Example code for the Adafruit SPI FRAM breakout */
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

/*
   @brief Comparing whether the read and write content is consistent.
          Can be used to test the probability of FRAM read and write errors.
*/
void readWriteTest(void)
{
  char wBuf[32] = "<<<RAK15006 FRAM_4M Test!!!!>>>";
  char rBuf[32] = {0};
  uint32_t successCount = 0;
  uint32_t failCount = 0;

  float progress = 0;
  time_t interval = millis();

  fram.writeEnable(true);
  for (uint32_t i = 0; i < FRAM_4M_SIZE; i += sizeof(wBuf) / sizeof(char))
  {
    fram.write(i, (uint8_t*)wBuf , sizeof(wBuf) / sizeof(char));
    fram.read( i, (uint8_t*)rBuf  , sizeof(rBuf) / sizeof(char));

    if (memcmp(wBuf , rBuf , sizeof(rBuf)) == 0)
    {
      successCount++;
    }
    else
    {
      failCount++;
    }
    if ((millis() - interval) > 100)
    {
      interval = millis();
      Serial.printf("Test progress: %5.2f%% , successCount: %ld , failCount:%ld \n", progress, successCount, failCount);
    }
    progress = (float)(i + sizeof(wBuf) / sizeof(char)) * 100 / FRAM_4M_SIZE;
    memset(rBuf , '0' , sizeof(rBuf) / sizeof(char));
    delay(1);
  }
  fram.writeEnable(false);
  Serial.printf("Test progress: %5.2f%% , successCount: %ld , failCount:%ld \n", progress, successCount, failCount);
}

/*
   @brief Read the contents of the entire chip.
*/
void readEntireChip(void)
{
  char readBuf[32] = {0};
  Serial.println();
  fram.writeEnable(true);
  for (uint32_t i = 0; i < FRAM_4M_SIZE; i += sizeof(readBuf))
  {
    fram.read( i, (uint8_t*)readBuf  , sizeof(readBuf));
    Serial.print("0x");
    Serial.print(i, HEX);
    Serial.print("\t");
    for (uint32_t j = 0; j < sizeof(readBuf); j++)
    {
      Serial.print("0x");
      Serial.print(readBuf[j], HEX);
      Serial.print(' ');
    }
    Serial.println();
  }
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
  Serial.println("RAK15006 FRAM_4M Read And Write Test ");
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
}

void loop(void) {

  char wData[25] = "RAK15006 FRAM_4M TEST";
  char rDate[25] = {0};
  Serial.println("reading and writing test");
  fram.writeEnable(true);
  fram.write(0x0000, (uint8_t*)wData , sizeof(wData) / sizeof(char));
  delay(1);
  fram.read( 0x0000, (uint8_t*)rDate , sizeof(rDate) / sizeof(char));
  Serial.println(rDate);
  
  delay(10);
  char wDataAgain[30] = "RAK15006 FRAM_4M TEST Again";
  char rDataAgain[30] = {0};
  fram.write(0x0000, (uint8_t*)wDataAgain , sizeof(wDataAgain) / sizeof(char));
  fram.read( 0x0000, (uint8_t*)rDataAgain , sizeof(rDataAgain) / sizeof(char));
  Serial.println(rDataAgain);
  fram.writeEnable(false);
  Serial.println();

  Serial.println("Comparing whether the read and write content is consistent.");
  readWriteTest();
  Serial.println();

  Serial.println("Read the contents of the entire chip.");
  readEntireChip();
  Serial.println("Read complete,if you want to test again,please reset the module");
  while (1)
  {
    delay(500);
  }
}
