/**
   @file GSM_TCP.ino
   @author rakwireless.com
   @brief Send GPS data via TCP with GSM
   @version 0.1
   @date 2021-6-28
   @copyright Copyright (c) 2020
**/
#include <Wire.h>

#define POWER_KEY WB_IO5
String at_rsp = "";
String gps_data = "";
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
	//BG77 init
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
  //register to GSM net
  mc20_at("AT+QGNSSC=1",2000);
  mc20_at("AT+COPS=?",20000);
  mc20_at("AT+COPS=1,0,\"CHINA MOBILE\"",5000);
  mc20_at("AT+QICSGP=1,\"CMCC\",\"\",\"\"",5000);  
  mc20_at("AT+COPS?",3000);
  mc20_at("AT+CREG?",3000);    
  //connect to tcp
  mc20_at("AT+QIOPEN=\"TCP\",\"42.193.113.207\",9999",10000);     
}

void loop()
{
  char cmd[128] = {0};
  String tmp = "";
  mc20_at("AT+QGNSSRD=\"NMEA/GGA\"",5000);  
  tmp = "AT+QISEND="+String(gps_data.length());
  tmp.toCharArray(cmd,128);
  mc20_at(cmd,1000);
  memset(cmd,0,128);
  gps_data.toCharArray(cmd,128); 
  mc20_at(cmd,1000); 
  gps_data = "";   
  memset(cmd,0,128);
  delay(2000);
}

void mc20_at(char *at, uint16_t timeout)
{
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
      at_rsp += char(Serial1.read());
    }
    delay(1);
  }
  if(at_rsp.indexOf("GNGGA")>1)
  {
    gps_data = at_rsp;
    gps_data = gps_data.substring(gps_data.indexOf("$"));
    parse_gps();    
  }
  Serial.println(at_rsp);
  at_rsp = "";
}

void parse_gps()
{
  //$GNGGA,092204.999,4250.5589,S,14718.5084,E,1,04,24.4,19.7,M,,,,0000*1F
  gps_data = gps_data.substring(gps_data.indexOf("GNGGA,")+6);
  if(gps_data.indexOf("E")>0)
    {
      gps_data = gps_data.substring(gps_data.indexOf(",")+1,gps_data.indexOf("E")+1);
    }
  else
    {
      gps_data = gps_data.substring(gps_data.indexOf(",")+1,gps_data.indexOf("W")+1);
    }  
}
