/**
   @file RAK15002_SD_Card.ino
   @author rakwireless.com
   @brief Read, write, delete SD card file for RAK11200.
   @version 0.1
   @date 2021-3-11
   @copyright Copyright (c) 2021
**/

#include "FS.h"
#include "SD.h"
#include "SPI.h"

/**
   @brief  This function is used to read data from file. 
*/
void readFile(fs::FS &fs, const char * path)
{
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if(!file)
  {
    Serial.println("Failed to open file for reading.");
    return;
  }

  Serial.println("Read from file: ");
  while(file.available())
  {
    Serial.write(file.read());
  }
  file.close();
}

/**
   @brief  This function is used to write data to file. 
*/
void writeFile(fs::FS &fs, const char * path, const char * message)
{
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file)
  {
    Serial.println("Failed to open file for writing.");
    return;
  }
  if(file.print(message))
  {
    Serial.println("File written.");
  } 
  else 
  {
    Serial.println("Write failed.");
  }
  file.close();
}

/**
   @brief  This function is used to append date to file. 
*/
void appendFile(fs::FS &fs, const char * path, const char * message)
{
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file)
  {
    Serial.println("Failed to open file for appending.");
    return;
  }
  if(file.print(message))
  {
    Serial.println("Message appended.");
  } 
  else 
  {
    Serial.println("Append failed.");
  }
  file.close();
}

/**
   @brief  This function is used to delete file. 
*/
void deleteFile(fs::FS &fs, const char * path)
{
  Serial.printf("Deleting file: %s\n", path);
  if(fs.remove(path))
  {
    Serial.println("File deleted.");
  } 
  else 
  {
    Serial.println("Delete failed.");
  }
}

/**
   @brief  This function is used to test read and write file speed. 
*/
void testFileIO(fs::FS &fs, const char * path)
{
  Serial.println("Test read and write file speed.");
  Serial.printf("Writing file: %s\n", path);
    
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;

  File file = fs.open(path, FILE_WRITE);
  if(file)// if the file opened okay, write to it.
  {
    size_t i;
    start = millis();
    for(i=0; i<2048; i++)
    {
      file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u KB written for %u ms\n", 2048 * 512/104, end);
    file.close();  
  }
  else 
  {
    Serial.println("Failed to open file for writing.");
    return;
  }

  file = fs.open(path, FILE_READ);
  if(file)
  {
    len = file.size();
    size_t flen = len;
    start = millis();
    while(len)
    {
      size_t toRead = len;
      if(toRead > 512)
      {
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    Serial.printf("%u KB read for %u ms\n", flen/1024, end);
    file.close();
  } 
  else 
  {
    Serial.println("Failed to open file for reading.");
    return;
  }
}

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
  Serial.println("=====================================");
  Serial.println("RAK15002 SD Card Test.");
  Serial.println("=====================================");

  if(!SD.begin(SS,SPI,80000000,"/sd",5))// Start access to the SD.
  {
    Serial.println("Card Mount Failed! Please make sure the card is inserted!");
    return;
  }

  deleteFile(SD, "/RAK15002.txt"); 
  
  writeFile(SD, "/RAK15002.txt", "RAK15002 SD Card Test.\n");

  readFile(SD, "/RAK15002.txt");

  testFileIO(SD, "/RAK15002.txt");
}

void loop()
{
}
