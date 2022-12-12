/**
   @file RAK5814_Verify_Encrypt_ATECC608A.ino
   @author rakwireless.com
   @brief  This example shows how to verify a digital signature of a message using an external public key.
   @version 0.1
   @date 2022-09-06
   @copyright Copyright (c) 2022
**/

#include <RAK5814_ATECC608A.h>  //Click to install library: http://librarymanager/All#RAK5814_ATECC608A

// Array to hold our 32 bytes of message. Note, it must be 32 bytes, no more or less.
uint8_t message[32] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
};

/*  
 * @note PublicKeyExternal, message, and signature come from RAK5814_Sign_Encrypt_ATECC608A.
 *       delete these, and then copy and paste your unique versions here.
 *       You will also need to change the name of the copied array "publicKey[64]" to "publicKeyExternal[64]"
 */
uint8_t publicKeyExternal[64] = {
  0xE8, 0xF1, 0x7C, 0x0C, 0x0C, 0x0B, 0xC1, 0x67, 0xE1, 0x17, 0xAA, 0x50, 0x40, 0x47, 0xCD, 0x31, 
  0x64, 0x85, 0xC1, 0x88, 0xF5, 0x80, 0x09, 0xB0, 0x72, 0x8D, 0xCE, 0xB2, 0x59, 0x64, 0x4D, 0x53, 
  0x1A, 0x50, 0x87, 0x4F, 0xF1, 0xF3, 0x63, 0x1C, 0xD4, 0xE4, 0x1A, 0xC8, 0xBC, 0x36, 0xCA, 0xAB, 
  0x9E, 0xCC, 0xE3, 0xBD, 0xC6, 0xF8, 0x88, 0x8B, 0x57, 0x82, 0xA1, 0xFE, 0x51, 0x50, 0xAD, 0xE8
};

uint8_t signature[64] = {
  0x44, 0x7A, 0x13, 0x66, 0xF0, 0x46, 0xE3, 0x7C, 0xAD, 0x9F, 0x57, 0x39, 0x6F, 0x5F, 0x07, 0x12, 
  0x51, 0x6B, 0x98, 0x41, 0x9F, 0xC1, 0xB8, 0xCA, 0xEB, 0x16, 0x0A, 0xE1, 0xC5, 0x3D, 0x4A, 0x35, 
  0x8B, 0xFD, 0xD3, 0x35, 0xCF, 0xBF, 0x4F, 0x34, 0xF3, 0x50, 0x67, 0x8C, 0x60, 0xB0, 0x8B, 0x11, 
  0xA1, 0x4A, 0x4A, 0xCB, 0x5A, 0x64, 0x69, 0x6E, 0x18, 0x79, 0xCC, 0xA5, 0x42, 0xAA, 0xDD, 0x24
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

  if (!ECC608A.ecdsaVerify(message, signature, publicKeyExternal))
  {
    Serial.println("Verify failed!");
  }
  else
  {
    printMessage();
    printSignature();
    printPublicKey();
    Serial.println();
    Serial.println("Verify OK!");
  }
}

void loop() 
{
  // Do nothing.
}

void printMessage()
{
  Serial.println("uint8_t message[32] = {");
  for (int i = 0; i < sizeof(message) ; i++)
  {
    Serial.print("0x");
    if ((message[i] >> 4) == 0) 
      Serial.print("0"); // print preceeding high nibble if it's zero
    Serial.print(message[i], HEX);
    if (i != 31) 
      Serial.print(", ");
    if ((31 - i) % 16 == 0) 
      Serial.println();
  }
  Serial.println("};");
}

void printSignature()
{
  Serial.println("uint8_t signature[64] = {");
  for (int i = 0; i < sizeof(signature) ; i++)
  {
    Serial.print("0x");
    if ((signature[i] >> 4) == 0) 
      Serial.print("0"); // print preceeding high nibble if it's zero
    Serial.print(signature[i], HEX);
    if (i != 63) 
      Serial.print(", ");
    if ((63 - i) % 16 == 0) 
      Serial.println();
  }
  Serial.println("};");
}

void printPublicKey()
{
  Serial.println("uint8_t publicKeyExternal[64] = {");
  for (int i = 0; i < sizeof(publicKeyExternal) ; i++)
  {
    Serial.print("0x");
    if ((publicKeyExternal[i] >> 4) == 0) 
      Serial.print("0"); // print preceeding high nibble if it's zero
    Serial.print(publicKeyExternal[i], HEX);
    if (i != 63) 
      Serial.print(", ");
    if ((63 - i) % 16 == 0) 
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
