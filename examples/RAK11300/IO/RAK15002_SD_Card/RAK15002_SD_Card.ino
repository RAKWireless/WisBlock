/**
   @file RAK15002_SD_Card.ino
   @author rakwireless.com
   @brief Read, write, delete SD card file for RAK4631.
   @version 0.1
   @date 2021-3-11
   @copyright Copyright (c) 2021
**/

#include "SPI.h"
#include "SD.h"//http://librarymanager/All#SD

/**
   @brief  This function is used to read data from file. 
*/
void readFile(const char * path)
{
  Serial.printf("Reading file: %s\n", path);
  
  File file = SD.open(path, FILE_READ); // re-open the file for reading.
  if (file) 
  {
    Serial.println("Read from file: ");
    
    while (file.available()) 
    {
      Serial.write(file.read());  // read from the file until there's nothing else in it.
    }
    file.close(); // close the file.
  } 
  else 
  {
    Serial.println("Failed to open file for reading."); // if the file didn't open, print an error.
    return;
  }
}

/**
   @brief  This function is used to write data to file. 
*/
void writeFile(const char * path, const char * message)
{
  Serial.printf("Writing file: %s\n", path);

  File file = SD.open(path, FILE_WRITE);

  if (file) // if the file opened okay, write to it:
  {
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
  else 
  {
    Serial.println("Failed to open file for writing."); 
    return;
  }
}

/**
   @brief  This function is used to delete file. 
*/
void deleteFile(const char * path)
{
  Serial.printf("Deleting file: %s\n", path);
  
  if(SD.remove(path))
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
void testFileIO(const char * path)
{
  Serial.println("Test read and write file speed.");
  Serial.printf("Writing file: %s\n", path);
  
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;

  File file = SD.open(path, FILE_WRITE);

  if (file)// if the file opened okay, write to it.
  {
    size_t i;
    start = millis();
    for(i=0; i<2048; i++)
    {
      file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u KB written for %u ms\n", 2048 * 512/1024, end);
    file.close();
  }
  else 
  {
    Serial.println("Failed to open file for writing.");
    return;
  }
  delay(10);
  
  file = SD.open(path, FILE_READ);
  if (file)
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
  
  if (!SD.begin()) 
  {    
    Serial.println("Card Mount Failed! Please make sure the card is inserted!");
    return;
  }
  
  deleteFile("RAK15002.txt"); 

  writeFile("RAK15002.txt", "RAK15002 SD Card Test.\n");
  
  readFile("RAK15002.txt");
  
  testFileIO("RAK15002.txt");
}

void loop() 
{
}
