/**
   @file RAK12500_GPS_ZOE-M8Q_UART.ino
   @author rakwireless.com
   @brief use UART get and parse gps data
   @version 0.1
   @date 2021-1-28
   @copyright Copyright (c) 2021
**/

#include <SparkFun_u-blox_GNSS_Arduino_Library.h> //http://librarymanager/All#SparkFun_u-blox_GNSS

SFE_UBLOX_GNSS g_myGNSS;
 
long g_lastTime = 0; //Simple local timer. Limits amount of I2C traffic to u-blox module.

void setup()
{
  int baud[7]={9600,14400,19200,38400,56000,57600,115200};

  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, 0);
  delay(1000);
  digitalWrite(WB_IO2, 1);
  delay(1000);
  
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
  
  Serial.println("GPS ZOE-M8Q Example(UART)");

  for(int i=0 ; i < sizeof(baud)/sizeof(int) ; i++)
  {
    Serial1.begin(baud[i]);
    while (!Serial1);      // Wait for user to open terminal
    if(g_myGNSS.begin(Serial1) == true)
    {
      Serial.printf("GNSS baund rate: %d \n",baud[i]);//GNSS baund rate
      break;
    }
    Serial1.end();
    delay(200);
  }
  Serial.println("GNSS serial connected");  

  g_myGNSS.setUART1Output(COM_TYPE_UBX);      //Set the UART port to output UBX only
  g_myGNSS.setI2COutput(COM_TYPE_UBX);        //Set the I2C port to output UBX only (turn off NMEA noise)
  g_myGNSS.saveConfiguration();               //Save the current settings to flash and BBR
}

void loop()
{
  if (millis() - g_lastTime > 100)
  {
    g_lastTime = millis(); //Update the timer
  
    long latitude = g_myGNSS.getLatitude();
    Serial.print(F(" Lat: "));
    Serial.print(latitude);

    long longitude = g_myGNSS.getLongitude();
    Serial.print(F(" Long: "));
    Serial.print(longitude);
    Serial.print(F(" (degrees * 10^-7)"));

    long altitude = g_myGNSS.getAltitude();
    Serial.print(F(" Alt: "));
    Serial.print(altitude);
    Serial.print(F(" (mm)"));

    long speed = g_myGNSS.getGroundSpeed();
    Serial.print(F(" Speed: "));
    Serial.print(speed);
    Serial.print(F(" (mm/s)"));

    long heading = g_myGNSS.getHeading();
    Serial.print(F(" Heading: "));
    Serial.print(heading);
    Serial.print(F(" (degrees * 10^-5)"));
    
    byte SIV = g_myGNSS.getSIV();
    Serial.print(F(" SIV: "));
    Serial.println(SIV);
  }
}
