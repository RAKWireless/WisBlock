/**
   @file Hologram_Tcp.ino
   @author rakwireless.com
   @brief BG77 tcp test with Hologram, send gps data to server
   @version 0.1
   @date 2020-12-28
   @copyright Copyright (c) 2020
**/


#define BG77_POWER_KEY WB_IO1
#define BG77_GPS_ENABLE WB_IO2

String bg77_rsp = "";

//Hologarm tcp message format is jason. Remember to replace the card key in type k. 
//About key details: https://support.hologram.io/hc/en-us/articles/360035212714-Device-keys
//About message details: https://www.hologram.io/references/embedded-apis#send-a-message-to-the-hologram-cloud 
String hologram_msg_pre = "{\"k\":\"+C7pOb8=\",\"d\":";
String hologram_msg_suff = ",\"t\":\"TOPIC1\"}";
String hologram_msg = "";
String send_data_length_at = "";
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
  Serial.println("RAK11200 Cellular TEST With Hologram sim card!");

  //BG77 init , Check if the modem is already awake
  time_t timeout = millis();
  bool moduleSleeps = true;
  Serial1.begin(115200);
  delay(1000);
  pinMode(BG77_GPS_ENABLE, OUTPUT);
  digitalWrite(BG77_GPS_ENABLE, 1);
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
    pinMode(BG77_POWER_KEY, OUTPUT);
    digitalWrite(BG77_POWER_KEY, 0);
    delay(1000);
    digitalWrite(BG77_POWER_KEY, 1);
    delay(2000);
    digitalWrite(BG77_POWER_KEY, 0);
    delay(1000);
  }
  Serial.println("BG77 power up!");

  //active and join to the net, this part may depend on some information of your operator.
  bg77_at("AT+CFUN=1,0", 500);
  delay(2000);
  bg77_at("AT+CPIN?", 500);
  delay(2000);
  bg77_at("AT+QNWINFO", 500);
  delay(2000);
  bg77_at("AT+QCSQ", 500);
  delay(2000);
  bg77_at("AT+CSQ", 500);
  delay(2000);
  bg77_at("AT+QIACT=1", 3000);
  delay(2000);

  //open tcp link with Hologram server
  bg77_at("AT+QIOPEN=1,0,\"TCP\",\"cloudsocket.hologram.io\",9999,0,1", 5000);
  delay(2000);
  
}
void parse_gps()
{
   int index1 = gps_data.indexOf(',');

   if(strstr(gps_data.c_str(),"E") != NULL)
   {
      int index2 = gps_data.indexOf('E'); 
      gps_data = gps_data.substring(index1+1,index2+1); 
   }
   if(strstr(gps_data.c_str(),"W") != NULL)
   {
      int index3 = gps_data.indexOf('W'); 
      gps_data = gps_data.substring(index1+1,index3+1);       
   }   
   
}


void get_gps()
{
  int gps_count = 300;
  int timeout = 1000;
  while(gps_count--)
  {
    Serial1.write("AT+QGPSLOC?\r");
    timeout = 1000;
    while (timeout--)
    {
      if (Serial1.available())
      {
         gps_data += char(Serial1.read());
      }
      delay(1);
    }
    if(strstr(gps_data.c_str(),"CME ERROR") != NULL)
    {
      gps_data = "";
      continue;
    }      
    if(strstr(gps_data.c_str(),"E") != NULL || strstr(gps_data.c_str(),"W") != NULL)
    {
      Serial.println(gps_data);
      parse_gps();
      break;
    }      
  }
}


//this function is suitable for most AT commands of bg96. e.g. bg96_at("ATI")
void bg77_at(char *at, uint16_t timeout)
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
      bg77_rsp += char(Serial1.read());
    }
    delay(1);
  }
  Serial.println(bg77_rsp);
  bg77_rsp = "";
}

void send_test_data()
{
  //open gps, gsm/nb will stop work now
  bg77_at("AT+QGPS=1,1", 20000);
  //get gps data, this will cost some time
  get_gps();
  //close gps
  bg77_at("AT+QGPSEND", 2000);  
  //combine the data packet
  hologram_msg = hologram_msg_pre+gps_data+hologram_msg_suff;
  Serial.println(hologram_msg);
  gps_data = "";
  //send data length command
  char tmp[256] = {0};
  send_data_length_at = "AT+QISEND=0," + hologram_msg.length();
  strncpy(tmp, send_data_length_at.c_str(), send_data_length_at.length());
  bg77_at(tmp, 3000);
  delay(2000);
  //send data
  memset(tmp,0,256);
  strncpy(tmp, hologram_msg.c_str(), hologram_msg.length());
  bg77_at(tmp, 1000);
  delay(1000);
}

void loop()
{
  Serial.println("Send test data to Hologram via TCP!");
  send_test_data();
  //consider the gps fix time, interval should be long
  delay(300000);
}
