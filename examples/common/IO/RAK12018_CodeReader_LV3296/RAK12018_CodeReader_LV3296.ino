/**
   @file RAK12018_CodeReader_LV3296.ino
   @author rakwireless.com
   @brief Get 1D or 2D code
   @version 0.1
   @date 2021-8-28
   @copyright Copyright (c) 2020
**/
#include <Wire.h>

void setup()
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);

  time_t serial_timeout = millis();
  Serial.begin(115200);
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
  Serial.println("RAK12018 Barcodes Scanner!");
  Serial1.begin(9600);
}

void loop()
{
  int timeout = 1000;
  String rsp = "";
  while (timeout--)
  {
    if (Serial1.available() > 0)
    {
      rsp += char(Serial1.read());
    }
    delay(1);
  }
  if (rsp.length() > 0)
  {
    Serial.println(rsp);
  }
  rsp = "";
}
