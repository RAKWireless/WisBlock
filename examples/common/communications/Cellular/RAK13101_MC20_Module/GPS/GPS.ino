/**
   @file GPS.ino
   @author rakwireless.com
   @brief Get GPS data from MC20
   @version 0.1
   @date 2021-6-28
   @copyright Copyright (c) 2020
**/
#include <TinyGPS.h>    //http://librarymanager/All#TinyGPS
#include <Wire.h>

TinyGPS gps;
String tmp_data = "";
int direction_S_N = 0;  //0--S, 1--N
int direction_E_W = 0;  //0--E, 1--W

#define POWER_KEY WB_IO5
String at_rsp = "";

void setup()
{
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
	
	Serial.println("AT CMD TEST!");
	// Check if the modem is already awake
	time_t timeout = millis();
	bool moduleSleeps = true;
	Serial1.begin(9600);
	delay(1000);
	Serial1.println("ATI");
	//init
	while ((millis() - timeout) < 6000)
	{
		if (Serial1.available())
		{
			String result = Serial1.readString();
			Serial.println("Modem response after start:");
			Serial.println(result);
			moduleSleeps = false;
		}
	}
	if (moduleSleeps)
	{
		// Module slept, wake it up
		pinMode(POWER_KEY, OUTPUT);
		digitalWrite(POWER_KEY, 0);
		delay(1000);
		digitalWrite(POWER_KEY, 1);
		delay(2000);
		digitalWrite(POWER_KEY, 0);
		delay(1000);
	}
	Serial.println("MC20 power up!");
  mc20_at("AT+QGNSSC=1",500);
  delay(2000);
}


void loop()
{
	mc20_at("AT+QGNSSRD=\"NMEA/GGA\"",1000);
}

void mc20_at(char *at, uint16_t timeout)
{
  bool newData = false;
  char tmp[256] = {0};
  int len = strlen(at);
  strncpy(tmp, at, len);
  uint16_t t = timeout;
  tmp[len] = '\r';
  Serial1.write(tmp);
  delay(10);
  while (t--)
  {
    if (Serial1.available())
    {
      char c = Serial1.read();
      at_rsp += c;
      if (gps.encode(c))
        newData = true;
    }
    delay(1);
  }
  direction_parse(at_rsp);
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
  //Serial.println(at_rsp);
  at_rsp = "";
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
