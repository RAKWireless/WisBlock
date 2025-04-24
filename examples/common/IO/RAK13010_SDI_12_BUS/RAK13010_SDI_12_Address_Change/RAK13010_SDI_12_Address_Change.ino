/**
   @file RAK13010_SDI_12_Address_Change.ino
   @author rakwireless.com
   @brief  Changing the Address of your SDI-12 Sensor.
           It discovers the address of the attached sensor and allows you to change it.
   @version 0.1
   @date 2022-03-11
   @copyright Copyright (c) 2022
**/
#include "RAK13010_SDI12.h" // Click to install library: // Click to install library: http://librarymanager/All#RAK13010

#define TX_PIN WB_IO6 // The pin of the SDI-12 data bus.
#define RX_PIN WB_IO5 // The pin of the SDI-12 data bus.
#define OE WB_IO4	  // Output enable pin, active low.

RAK_SDI12 mySDI12(RX_PIN, TX_PIN, OE);

String myCommand  = "";   // Empty to start.
char   oldAddress = '!';  // Invalid address as placeholder.

/*
 * @brief this checks for activity at a particular address
 *        expects a char, '0'-'9', 'a'-'z', or 'A'-'Z'.
 */
boolean checkActive(byte i) 
{
  Serial.print("Checking address ");
  Serial.print((char)i);
  Serial.println("...");
  myCommand = "";
  myCommand += (char)i;  // Sends basic 'acknowledge' command [address][!].
  myCommand += "!";

  for (int j = 0; j < 3; j++) // Goes through three rapid contact attempts.
  {  
    mySDI12.sendCommand(myCommand);
    delay(100);
    int avail = mySDI12.available();
    if (avail > 0) // If we here anything, assume we have an active sensor.
    {  
      Serial.println(" Occupied");
      mySDI12.clearBuffer();
      return true;
    } 
    else 
    {
      Serial.println(" Vacant");  // Otherwise it is vacant.
      mySDI12.clearBuffer();
    }
  }
  return false;
}

void setup() 
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);  // Power the sensors.

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

  Serial.println("Opening SDI-12 bus...");
  mySDI12.begin();
  delay(500);
}

void loop() 
{
  boolean found = false;  // Have we identified the sensor yet?

  for (byte i = '0'; i <= '9'; i++) // Scan address space 0-9.
  {  
    if (found) 
      break;
    if (checkActive(i)) 
    {
      found = true;
      oldAddress = i;
    }
    delay(100);
  }

  for (byte i = 'a'; i <= 'z'; i++) // Scan address space a-z.
  {  
    if (found) 
      break;
    if (checkActive(i)) 
    {
      found = true;
      oldAddress = i;
    }
    delay(100);
  }

  for (byte i = 'A'; i <= 'Z'; i++) // scan address space A-Z.
  {  
    if (found) 
      break;
    if (checkActive(i)) 
    {
      found = true;
      oldAddress = i;
    }
    delay(100);
  }

  if (!found) 
  {
    Serial.println("No sensor detected. Check physical connections.");  // Couldn't find a sensor.
  } 
  else 
  {
    Serial.print("Sensor active at address ");  // Found a sensor!
    Serial.print(oldAddress);
    Serial.println(".");

    Serial.println("Enter new address:");  // Prompt for a new address.
    while (!Serial.available()) ;
    char newAdd = Serial.read();
    
    while (((newAdd < '0') || (newAdd > '9')) && ((newAdd < 'a') || (newAdd > 'z')) && ((newAdd < 'A') || (newAdd > 'Z'))) // Wait for valid response. 
    {
      if (!(newAdd == '\n') || (newAdd == '\r') || (newAdd == ' ')) 
      {
        Serial.println("Not a valid address. Please enter '0'-'9', 'a'-'A', or 'z'-'Z'.");
      }
      while (!Serial.available()) ;
      newAdd = Serial.read();
    }

    /* 
     *  The syntax of the change address command is:
     *  [currentAddress]A[newAddress]!
     */
    Serial.println("Readdressing sensor.");
    myCommand = "";
    myCommand += (char)oldAddress;
    myCommand += "A";
    myCommand += (char)newAdd;
    myCommand += "!";
    mySDI12.sendCommand(myCommand);

    delay(500);
    mySDI12.clearBuffer();
    Serial.println("Success. Rescanning for verification.");
  }
}
