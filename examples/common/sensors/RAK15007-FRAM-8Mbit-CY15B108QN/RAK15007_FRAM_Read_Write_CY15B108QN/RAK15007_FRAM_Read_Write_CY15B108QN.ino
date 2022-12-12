/**
   @file RAK15007_FRAM_Read_Write_CY15B108QN.ino
   @author rakwireless.com
   @brief FRAM_8M Read And Write Test
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
   @brief Comparing whether the read and write content is consistent.
          Can be used to test the probability of FRAM read and write errors.
*/
void readWriteTest(void)
{
  char wBuf[32] = "<<<RAK15007 FRAM_8M Test!!!!>>>";
  char rBuf[32] = {0};
  uint32_t successCount = 0;
  uint32_t failCount = 0;

  float progress = 0;
  time_t interval = millis();

  for (uint32_t addr = 0; addr < FRAM_8M_SIZE; addr += sizeof(wBuf) / sizeof(char))
  {
    fram_cy15b108.writeEnable(true);
    fram_cy15b108.write(addr, (uint8_t*)wBuf , sizeof(wBuf) / sizeof(char));
    fram_cy15b108.read( addr, (uint8_t*)rBuf  , sizeof(rBuf) / sizeof(char));
    fram_cy15b108.writeEnable(false);

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
    progress = (float)(addr + sizeof(wBuf) / sizeof(char)) * 100 / FRAM_8M_SIZE;
    memset(rBuf , '0' , sizeof(rBuf) / sizeof(char));
    delay(1);
  }
  Serial.printf("Test progress: %5.2f%% , successCount: %ld , failCount:%ld \n", progress, successCount, failCount);
}

/*
   @brief Read the contents of the entire chip.
*/
void readEntireChip(void)
{
  char readBuf[32] = {0};
  Serial.println();

  for (uint32_t addr = 0; addr < FRAM_8M_SIZE; addr += sizeof(readBuf))
  {
    fram_cy15b108.read( addr, (uint8_t*)readBuf  , sizeof(readBuf));
    Serial.print("0x");
    Serial.print(addr, HEX);
    Serial.print("\t");
    for (uint32_t bufCount = 0; bufCount < sizeof(readBuf); bufCount++)
    {
      Serial.print("0x");
      Serial.print(readBuf[bufCount], HEX);
      Serial.print(' ');
    }
    Serial.println();
  }
}


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

/*
   @brief Comparing whether the read and write special sector content is consistent.
          Can be used to test the probability of FRAM read and write errors.
*/
void specialSectorWriteAndReadTest(void)
{
  char specialSector_wBuf[32] = "<Special Sector Write And Read>";
  char specialSector_rBuf[32] = {0};
  uint32_t successCount = 0;
  uint32_t failCount = 0;

  float progress = 0;

  for (uint32_t addr = 0; addr < 0xFF; addr += sizeof(specialSector_wBuf) / sizeof(char))
  {
    fram_cy15b108.writeEnable(true);
    fram_cy15b108.specialSectorWrite(addr, (uint8_t*)specialSector_wBuf , sizeof(specialSector_wBuf) / sizeof(char));
    fram_cy15b108.specialSectorRead( addr, (uint8_t*)specialSector_rBuf , sizeof(specialSector_rBuf) / sizeof(char));
    fram_cy15b108.writeEnable(false);

    if (memcmp(specialSector_wBuf , specialSector_rBuf , sizeof(specialSector_rBuf)) == 0)
    {
      successCount++;
    }
    else
    {
      failCount++;
    }
    Serial.printf("Special Sector Write And Read Test Progress: %5.2f%% , successCount: %ld , failCount:%ld \n", progress, successCount, failCount);
    progress = (float)(addr + sizeof(specialSector_wBuf) / sizeof(char)) * 100 / 0xff;
    memset(specialSector_rBuf , '0' , sizeof(specialSector_rBuf) / sizeof(char));
    delay(100);
  }
}

/**
   The CY15X108QN supports a FAST READ opcode (0Bh) that is provided for opcode compatibility with serial flash devices
*/
void fastReadOperationTest(void)
{
  char readBuf[32] = {0};
  Serial.println();

  for (uint32_t addr = 0; addr < FRAM_8M_SIZE; addr += sizeof(readBuf))
  {
    fram_cy15b108.fastReadOperation( addr, (uint8_t*)readBuf  , sizeof(readBuf));
    Serial.print("0x");
    Serial.print(addr, HEX);
    Serial.print("\t");
    for (uint32_t bufCount = 0; bufCount < sizeof(readBuf); bufCount++)
    {
      Serial.print("0x");
      Serial.print(readBuf[bufCount], HEX);
      Serial.print(' ');
    }
    Serial.println();
  }
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
  delay(1000);

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
  fram_cy15b108.writeEnable(true);
  fram_cy15b108.writeByte(0x00, 0x01);
  uint8_t wByte = fram_cy15b108.readByte(0x00);
  if (wByte == 0x01)
  {
    Serial.println("write byte successful ");
  }
  fram_cy15b108.writeEnable(false);
}


void loop(void) {

  uint8_t serialNumberBuf[8] = {0x00};
  fram_cy15b108.getSerialNumber(serialNumberBuf, sizeof(serialNumberBuf) / sizeof(uint8_t));
  Serial.print("serialNumberBuf=0x");
  for (uint8_t serialCount = 0; serialCount < 8; serialCount++)
  {
    Serial.printf("%02x", serialNumberBuf[serialCount]);  // The factory default value for the 8-byte Serial Number is ‘0000000000000000h’.
  }
  Serial.println();
  delay(1000);

  char wData[25] = "RAK15007 FRAM_8M TEST";
  char rDate[25] = {0};
  Serial.println("reading and writing test");
  fram_cy15b108.writeEnable(true);
  fram_cy15b108.write(0x0000, (uint8_t*)wData , sizeof(wData) / sizeof(char));
  delay(1);
  fram_cy15b108.read( 0x0000, (uint8_t*)rDate , sizeof(rDate) / sizeof(char));
  Serial.println(rDate);
  fram_cy15b108.writeEnable(false);
  delay(1);

  char wDataAgain[30] = "RAK15007 FRAM_8M TEST Again";
  char rDataAgain[30] = {0};
  fram_cy15b108.writeEnable(true);
  fram_cy15b108.write(0x0000, (uint8_t*)wDataAgain , sizeof(wDataAgain) / sizeof(char));
  fram_cy15b108.read( 0x0000, (uint8_t*)rDataAgain , sizeof(rDataAgain) / sizeof(char));
  Serial.println(rDataAgain);
  fram_cy15b108.writeEnable(false);
  Serial.println();

  Serial.println("Comparing whether the read and write special sector content is consistent.");
  specialSectorWriteAndReadTest();
  Serial.println();

  Serial.println("Comparing whether the read and write content is consistent.");
  readWriteTest();
  Serial.println();

  delay(2000);
  Serial.println("FAST Read the contents of the entire chip.");
  fastReadOperationTest();
  Serial.println();

  delay(2000);
  Serial.println("Read the contents of the entire chip.");
  readEntireChip();
  Serial.println("Read complete,if you want to test again,please reset the module");
  while (1)
  {
    delay(500);
  }
}
