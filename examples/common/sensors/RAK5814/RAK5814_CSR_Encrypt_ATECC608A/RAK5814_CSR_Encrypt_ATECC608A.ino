/**
   @file RAK5814_CSR_Encrypt_ATECC608A.ino
   @author rakwireless.com
   @brief This sketch can be used to generate a CSR for a private key generated in an ATECC608A crypto chip slot.
          If the ECC508/ECC608 is not configured and locked it prompts the user to configure and lock the chip with a default TLS configuration.
          The user is prompted for the following information that is contained in the generated CSR:
          - country
          - state or province
          - locality
          - organization
          - organizational unit
          - common name
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

  Serial.println("Hi there, in order to generate a new CSR for your board, we'll need the following information ...");
  Serial.println();

  String country            = promptAndReadLine("Country Name (2 letter code)", "");
  String stateOrProvince    = promptAndReadLine("State or Province Name (full name)", "");
  String locality           = promptAndReadLine("Locality Name (eg, city)", "");
  String organization       = promptAndReadLine("Organization Name (eg, company)", "");
  String organizationalUnit = promptAndReadLine("Organizational Unit Name (eg, section)", "");
  String common             = promptAndReadLine("Common Name (e.g. server FQDN or YOUR name)", serialNumber.c_str());
  String slot               = promptAndReadLine("What slot would you like to use? (0 - 4)", "0");
  String generateNewKey     = promptAndReadLine("Would you like to generate a new private key? (Y/n)", "Y");

  Serial.println();

  generateNewKey.toLowerCase();

  if (!ECCX08CSR.begin(slot.toInt(), generateNewKey.startsWith("y"))) 
  {
    Serial.println("Error starting CSR generation!");
    while (1)
    {
      delay(100);
    }
  }

  ECCX08CSR.setCountryName(country);
  ECCX08CSR.setStateProvinceName(stateOrProvince);
  ECCX08CSR.setLocalityName(locality);
  ECCX08CSR.setOrganizationName(organization);
  ECCX08CSR.setOrganizationalUnitName(organizationalUnit);
  ECCX08CSR.setCommonName(common);

  String csr = ECCX08CSR.end();

  if (!csr) 
  {
    Serial.println("Error generating CSR!");
    while (1)
    {
      delay(100);
    }
  }

  Serial.println("Here's your CSR, enjoy!");
  Serial.println();
  Serial.println(csr);
}

void loop() 
{
  // do nothing
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
