/**
   @file RAK5814_Hash_Encrypt_ATECC608A.ino
   @author rakwireless.com
   @brief  This example shows how to create a hash on larger amounts of data.
           If you have only 32 bytes of data, then you can simply sign the data,
           But if you have more than that, then you mush send your data into an SHA256 algorithm,
           And this will create a 32-byte hash of the data. 
           It's kind of like a signature, but it's created with a standard algorithm, and so anyone can make it. 
           The ATECC608A will make it for us easily in an embedded system.
   @version 0.1
   @date 2022-09-06
   @copyright Copyright (c) 2022
**/

#include <RAK5814_ATECC608A.h>  //Click to install library: http://librarymanager/All#RAK5814_ATECC608A

/*
 * Array to hold our message. Note, this can be greater than 32 bytes!!
 */
//uint8_t message[] = {
//  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
//  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
//};

uint8_t message[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
};

/*
 * Array to hold our 32-byte hash. 
 * Note, it must be 32 bytes, no more or less.
 * Also note, we will start with it blank, but the crypto co-processor will fill it in when we call atecc.sha256()
 */
uint8_t myHash[32] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void setup() 
{
  //Sensor power switch.
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);

  // Initialize Serial for debug output.
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
  if (!ECC608A.begin()) 
  {
    Serial.println("Failed to communicate with ECC508/ECC608!");
    while (1)
    {
      delay(100);
    }
  }

  String serialNumber = ECC608A.serialNumber();
  
  Serial.print("ECC608A Serial Number = ");
  Serial.println(serialNumber);
  Serial.println();

  if (!ECC608A.locked()) 
  {
    String lock = promptAndReadLine("The ECC608A on your board is not locked, would you like to PERMANENTLY configure and lock it now? (y/N)", "N");
    lock.toLowerCase();

    if (!lock.startsWith("y")) 
    {
      Serial.println("Unfortunately you can't proceed without locking it :(");
      while (1)
      {
        delay(100);
      }
    }

    if (!ECC608A.writeConfiguration(ECCX08_DEFAULT_TLS_CONFIG)) 
    {
      Serial.println("Writing ECC608A configuration failed!");
      while (1)
      {
        delay(100);
      }
    }

    if (!ECC608A.lock()) 
    {
      Serial.println("Locking ECC608A configuration failed!");
      while (1)
      {
        delay(100);
      }
    }

    Serial.println("ECC608A locked successfully");
    Serial.println();
  }

  ECC608A.beginSHA256();
  for (int i = 0; i < sizeof(message); i += 64) 
  {
    int chunkLength = sizeof(message) - i;
    if (chunkLength > 64) 
    {
      chunkLength = 64;
    }
    if (chunkLength == 64) 
    {
      if (!ECC608A.updateSHA256(&message[i])) 
      {
        Serial.println("SHA256 failed!");
        while (1)
        {
          delay(100);
        }
      }
    } 
    else 
    {
      if (!ECC608A.endSHA256(&message[i], chunkLength, myHash)) 
      {
        Serial.println("SHA256 failed!");
        while (1)
        {
          delay(100);
        }
      }
      else
      {
        printMessage(); // Nice debug to see what you're hashing. see function below.
        printMyHash(); // Nice debug to see your new hash. see function below.
      }
    }
  }
}

void loop() 
{
  // Do nothing.
}

void printMessage()
{
  Serial.println("uint8_t message[] = {");
  for (int i = 0; i < sizeof(message) ; i++)
  {
    Serial.print("0x");
    if ((message[i] >> 4) == 0) 
      Serial.print("0"); // Print preceeding high nibble if it's zero.
    Serial.print(message[i], HEX);
    if (i < (sizeof(message) - 1)) 
      Serial.print(", ");
    if (((sizeof(message) - 1) - i) % 16 == 0) 
      Serial.println();
  }
  Serial.println("};\n\r");
}

void printMyHash()
{
  Serial.println("uint8_t myHash[32] = {");
  for (int i = 0; i < sizeof(myHash) ; i++)
  {
    Serial.print("0x");
    if ((myHash[i] >> 4) == 0) 
      Serial.print("0"); // Print preceeding high nibble if it's zero.
    Serial.print(myHash[i], HEX);
    if (i != 31) 
      Serial.print(", ");
    if ((31 - i) % 16 == 0) 
      Serial.println();
  }
  Serial.println("};");
}

String promptAndReadLine(const char* prompt, const char* defaultValue) 
{
  Serial.print(prompt);
  Serial.print(" [");
  Serial.print(defaultValue);
  Serial.print("]: ");

  String s = readLine();

  if (s.length() == 0) 
  {
    s = defaultValue;
  }

  Serial.println(s);

  return s;
}

String readLine() 
{
  String line;

  while (1) 
  {
    if (Serial.available()) 
    {
      char c = Serial.read();

      if (c == '\r') 
      {
        // ignore
        continue;
      } 
      else if (c == '\n') 
      {
        break;
      }
      line += c;
    }
  }

  return line;
}
