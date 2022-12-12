/**
   @file RAK5814_Random_Encrypt_ATECC608A.ino
   @author rakwireless.com
   @brief This example shows how to use the Random Number Generator on the ATECC608A.
          It will print random numbers once a second on the serial terminal at 115200.
   @version 0.1
   @date 2022-09-06
   @copyright Copyright (c) 2022
**/

#include <RAK5814_ATECC608A.h>  //Click to install library: http://librarymanager/All#RAK5814_ATECC608A

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

  if (!ECC608A.locked()) 
  {
    Serial.println("The ECC508/ECC608 is not locked!");
    while (1)
    {
      delay(100);
    }
  }
}

void loop() 
{
  Serial.print("Random number = ");
  Serial.println(ECC608A.random(65535));

  delay(1000);
}
