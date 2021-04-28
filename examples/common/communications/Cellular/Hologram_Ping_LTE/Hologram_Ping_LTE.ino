/**
 * @file BG77_Hologram_Ping.ino
 * @author rakwireless.com
 * @brief RAK4631 + RAK5860 BG77 module product testing
 * @version 0.1
 * @date 2021-01-26
 * 
 * @copyright Copyright (c) 2021
 * 
 * @note RAK5005-O GPIO mapping to RAK4631 GPIO ports
   RAK5005-O <->  nRF52840
   IO1       <->  P0.17 (Arduino GPIO number 17)
   IO2       <->  P1.02 (Arduino GPIO number 34)
   IO3       <->  P0.21 (Arduino GPIO number 21)
   IO4       <->  P0.04 (Arduino GPIO number 4)
   IO5       <->  P0.09 (Arduino GPIO number 9)
   IO6       <->  P0.10 (Arduino GPIO number 10)
   SW1       <->  P0.01 (Arduino GPIO number 1)
   A0        <->  P0.04/AIN2 (Arduino Analog A2
   A1        <->  P0.31/AIN7 (Arduino Analog A7
   SPI_CS    <->  P0.26 (Arduino GPIO number 26) 
 */

#define BG77_POWER_KEY 17
char cArr[128] = {0};
String bg_rsp = "";
bool bg_timeout = false;

//this function is suitable for most AT commands of bg96. e.g. bg96_at("ATI")
boolean bg_at(char *at, uint16_t timeout)
{
  bg_rsp = "";
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
      bg_rsp += char(Serial1.read());
    }
    delay(1);
  }
  if( bg_rsp.length() > 0 )
    bg_timeout = false;
  else
    bg_timeout = true;
  return bg_timeout;
}

void deact(void)
{
  // deactivate PDP
  bg_at("AT+QIDEACT=1" ,1500);
  // Set minimal UE Functionality
  bg_at("AT+CFUN=0,0" ,1500); 
}

void setup()
{
  bool reg= false;
  bool pdp_active = false;
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial)
    delay(10);
  Serial.println("BG77 LTE ping test!");
  // Check if the modem is already awake
  time_t timeout = millis();
  bool moduleSleeps = true;
  Serial1.begin(115200);
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
    pinMode(BG77_POWER_KEY, OUTPUT);
    digitalWrite(BG77_POWER_KEY, 0);
    delay(1000);
    digitalWrite(BG77_POWER_KEY, 1);
    delay(2000);
    digitalWrite(BG77_POWER_KEY, 0);
    delay(1000);
  }
  Serial.println("BG77 power up!");
  
// Verbose Error Reporting to get understandable error reporting 
  bg_timeout = bg_at("AT+CMEE=2" ,1500);
  if ( bg_timeout == false )
  { 
    if ( bg_rsp.indexOf('OK') > 0 ) 
      Serial.println(bg_rsp); 

  }
  else
    return;
    
// Set full UE Functionality
  bg_timeout = bg_at("AT+CFUN=1,0" ,1400);
  if (bg_timeout == false) 
  {
    if ( bg_rsp.indexOf('OK') > 0 ) 
      Serial.println(bg_rsp);
  }
  else
    return;
    
// Set eMTC LTE mode
  bg_timeout = bg_at("AT+QCFG=\"iotopmode\",0,1",2000);
  if ( bg_timeout == false )
  {
    if ( bg_rsp.indexOf('OK') > 0 )   
      Serial.println(bg_rsp);
  }
  else
    return;

  // Set the APN to "hologram" with no username or password.
  // Configure parameters of a TCP/IP context
  bg_timeout = bg_at("AT+QICSGP=1,1,\"hologram\",\"\",\"\",1" ,2000);
  if (bg_timeout == false )
  { 
    if ( bg_rsp.indexOf('OK') > 0 )  
      Serial.println(bg_rsp);
  }  
  else
    return;
  
  Serial.println("Waiting for registration");
  do 
  {
    reg=false;
    bg_timeout = bg_at("AT+CREG?",2000);
    if ( bg_timeout == false )
    {
        int v =  bg_rsp.indexOf('0,');
        if ( v > 0 )
        {
          if ( bg_rsp[v+1] == '5' )
          {
            reg = true;
            Serial.println("Registered: Roaming");
          } 
          else if ( bg_rsp[v+1] == '1' )
          {
            reg = true;
            Serial.println("Registered: Home");
          }
      
        } 
    }
      
    delay(200);
     
  } while (!reg);

 // Get NW INFO
  Serial.println("Waiting for NW Info");
  do {
    reg=false;
    bg_timeout = bg_at("AT+QNWINFO",1000);
    if ( bg_timeout == false )
    {
      int v = bg_rsp.indexOf('eMTC');
      if ( v >0 )
      {
        reg = true;
        Serial.println(bg_rsp); 
      }
      else 
      { 
        v = bg_rsp.indexOf('ervice'); 
        if ( v >0 )
          Serial.println("No service"); 
      }
     
    }   
    delay(200);
     
  } while (!reg);

// Check if PDP IPV4 is activated
// Don't send data if PDP is deactivated

  pdp_active = false;
  bg_timeout = bg_at("AT+QIACT?",1500);
  if ( bg_timeout == false )
  {
    if ( bg_rsp.indexOf(',1,1') > 0 )
    {
      pdp_active = true;
      Serial.println("PDP is activated");
    }
  }
  else
    return;

// Don't send PDP activate command if PDP is already activated

  if (  pdp_active == false )
  {
 
    Serial.println("Activating PDP");
    do {
        reg=false;
        bg_timeout = bg_at("AT+QIACT=1",1500);
        if ( bg_timeout == false )
        {
          if ( bg_rsp.indexOf('OK') >0 )
          {
            reg = true;
            Serial.println(bg_rsp); 
          }
 
        }
      delay(200);
     
    } while (!reg);

  }

  
}

void loop()
{
  int timeout = 100;
  int retry = 3;
  String resp = "";
  String snd = "";
  int v;
 
//  char cArr[128] = {0};
  if (bg_timeout == true)
  {
    Serial.print("Timeout during initialization");
    while(1);
  }
  delay(100);

  while ( retry--)
  {
    Serial.println("Sending ping");
    bg_at( "AT+QPING=1,\"baidu.com\",15,1",5000 );
    if ( bg_timeout == false )
    {
      v = bg_rsp.indexOf('RROR');
      if ( v > 0)
      {
        Serial.println( "Ping error...retry" );
        continue;
      }
      // Result => 0 received the ping response from the server
      v = bg_rsp.indexOf('+QPING: 0,"');
      if ( v >0 )
        v = bg_rsp.indexOf('+QPING: 0,1,1');       
      if ( v >0 )
      {
        Serial.println( "1 ping sent 1 ping received" );
        break;
      }
    }
    delay(200);
  } // retry--
  
  Serial.println("Waiting AT command");
  while (1)
  {
    timeout = 100;
  
    while (timeout--)
    {
      if (Serial1.available() > 0)
      {
        resp += char(Serial1.read());
      }
      if (Serial.available() > 0)
      {
        snd += char(Serial.read());
      }
      delay(1);
    }
    if (resp.length() > 0)
    {
      Serial.print(resp);
    }
    if (snd.length() > 0)
    {
      memset(cArr, 0, 128);
      snd.toCharArray(cArr, snd.length() + 1);
      Serial1.write(cArr);
      delay(10);
    }
    resp = "";
    snd = "";
  }
}
