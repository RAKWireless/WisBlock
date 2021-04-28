/**
   @file RAK1910_GPS_UBLOX7.ino
   @author rakwireless.com
   @brief get and parse gps data
   @version 0.1
   @date 2020-12-28
   @copyright Copyright (c) 2020
**/

#include <TinyGPS.h>    //http://librarymanager/All#TinyGPS

TinyGPS gps;
String tmp_data = "";
int direction_S_N = 0;  //0--S, 1--N
int direction_E_W = 0;  //0--E, 1--W


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


  //gps init
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, 0);
  delay(1000);
  digitalWrite(WB_IO2, 1);
  delay(1000);

  Serial1.begin(9600);
  while (!Serial1)
    ;
  Serial.println("GPS uart init ok!");
}

void direction_parse(String tmp)
{
    if (tmp.indexOf(",E,") != -1)
    {
        direction_E_W = 0;
    }
    else
    {
        direction_E_W = 1;
    }
    
    if (tmp.indexOf(",S,") != -1)
    {
        direction_S_N = 0;
    }
    else
    {
        direction_S_N = 1;
    }
}


void loop()
{
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (Serial1.available())
    {
      char c = Serial1.read();
      tmp_data += c;
      if (gps.encode(c))
        newData = true;
    }
  }
  direction_parse(tmp_data);
  tmp_data = "";
  
  if (newData)
  {
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    if(direction_S_N == 0)
    {
      Serial.print("(S):");
    }
    else
    {
      Serial.print("(N):");
    }
    Serial.print("LAT=");
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    if(direction_E_W == 0)
    {
      Serial.print(" (E):");
    }
    else
    {
      Serial.print(" (W):");
    }
    Serial.print("LON=");
    Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    Serial.print(" SAT=");
    Serial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    Serial.print(" PREC=");
    Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
  }
  
  gps.stats(&chars, &sentences, &failed);
  Serial.print(" CHARS=");
  Serial.print(chars);
  Serial.print(" SENTENCES=");
  Serial.print(sentences);
  Serial.print(" CSUM ERR=");
  Serial.println(failed);
  if (chars == 0)
    Serial.println("** No characters received from GPS: check wiring **");
}
