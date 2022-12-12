/**
   @file RAK1500x_FRAM_Read_Write_MB85RC.ino
   @author rakwireless.com
   @brief Test RAK1500x FRAM read and write functions. 
					Suitable for RAK15003, RAK15004, RAK15005.
   @version 0.1
   @date 2021-4-13
   @copyright Copyright (c) 2022
**/
#include "RAK1500x_MB85RC.h"  // Click here to get the library: http://librarymanager/All#RAK1500x_MB85RC

#define DEV_ADDR    MB85RC_DEV_ADDRESS

/*
 * @note If installed in SlotB.
 *       Since IO2 is the power supply enable pin level is high.
 *       Only read operation can be performed on this slot.
 */
#define WP_PING   WB_IO1 // SlotA installation.
//#define WP_PING   WB_IO3 // SlotC installation.
//#define WP_PING   WB_IO5 // SlotD installation.

RAK_MB85RC MB85RC;

void setup() 
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
	
  pinMode(WP_PING, OUTPUT);
  digitalWrite(WP_PING, LOW);
  
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
  while(MB85RC.begin(Wire , DEV_ADDR) == false) 
  {
		Serial.println("MB85RC is not connected.");
    while(1)
    {
      delay(1000);
    }
  }
  
  switch (MB85RC.getDeviceType()) 
  {
    case MB85RC256:
      Serial.print("RAK15003(MB85RC256) ");
      break;
    case MB85RC512:
      Serial.print("RAK15004(MB85RC512) ");
      break;
    case MB85RC1:
      Serial.print("RAK15005(MB85RC1M) ");
      break;
    default:
      break;
  }
  Serial.println("initialized successfully.");
}

void loop()
{
  char writeDate[20]="Hello RAK1500X.";
  char readDate[20] = {0};
  Serial.println("Simple reading and writing test.");
  MB85RC.write(0x0000, (uint8_t*)writeDate , strlen(writeDate));
  MB85RC.read( 0x0000, (uint8_t*)readDate  , strlen(writeDate));
  Serial.println(readDate);

  Serial.println("Comparing whether the read and write content is consistent.");
  readWriteTest();

  Serial.println("Read the contents of the entire chip.");
  readEntireChip();

  Serial.println();
  Serial.println("Do nothing here, just love you!");
  while(1)
  {
    delay(100);    
  }
}

/*
 * @brief Comparing whether the read and write content is consistent.
 *        Can be used to test the probability of FRAM read and write errors.
 */
void readWriteTest()
{
  char writeBuf[16] = ">>Test RAK1500X";
  char readBuf[16] = {0};
  uint32_t successCount = 0;
  uint32_t failCount = 0;
  uint32_t productSize = MB85RC.getDeviceCapacity();
  float progress = 0;
  time_t interval = millis();
  
  for(uint32_t i = 0; i < productSize; i+=sizeof(writeBuf))
  {
    MB85RC.write(i, (uint8_t*)writeBuf , sizeof(writeBuf));
    MB85RC.read( i, (uint8_t*)readBuf  , sizeof(readBuf));
    if(memcmp(writeBuf , readBuf , sizeof(readBuf)) == 0)
    {
      successCount++;
    }
    else
    {
      failCount++;  
    }
    if((millis() - interval) > 1000)
    {
      interval = millis();
      Serial.printf("Test progress: %5.2f%% , successCount: %ld , failCount:%ld \n",progress,successCount,failCount);    
    }
    progress = (float)(i+sizeof(writeBuf)) * 100 / productSize;
    memset(readBuf , '0' , sizeof(readBuf));
    delay(1);
  }
  Serial.printf("Test progress: %5.2f%% , successCount: %ld , failCount:%ld \n",progress,successCount,failCount); 
}

/*
 * @brief Read the contents of the entire chip.
 */
void readEntireChip()
{
  char readBuf[32] = {0};
  uint32_t productSize = MB85RC.getDeviceCapacity();
  Serial.println();
  for(uint32_t i = 0; i < productSize; i+=sizeof(readBuf))
  {
    MB85RC.read( i, (uint8_t*)readBuf  , sizeof(readBuf));
    Serial.print("0x"); 
    Serial.print(i,HEX);
    Serial.print("\t");
    for(uint32_t j = 0; j < sizeof(readBuf); j++)
    {
      Serial.print("0x"); 
      Serial.print(readBuf[j],HEX);
      Serial.print(' ');
    }
    Serial.println();
  }
}
