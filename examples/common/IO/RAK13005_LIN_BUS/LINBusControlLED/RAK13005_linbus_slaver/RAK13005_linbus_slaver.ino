/**
 * @file RAK13005_linbus_slaver.ino
 * @author rakwireless.com
 * @brief linbus slaver receive data from master and send data to master 
 * @version 0.1
 * @date 2021-05-01
 * @copyright Copyright (c) 2021
 */
#include <Wire.h>
#include  "lin_bus.h"  //library: http://librarymanager/All#RAKwireless_TLE7259_LIN_Bus_library

#if defined(_VARIANT_RAK4630_)
  #define BOARD "RAK4631 "
  #define LED_GREEN 35
  #define LED_BLUE  36
  int lin_tx =  16;
#elif defined(_VARIANT_RAK11300_) 
  #define BOARD "RAK11300"   
  #define LED_GREEN  23
  #define LED_BLUE   24
  int lin_tx =  0;  
#else 
  #define BOARD "RAK11200 "   
  #define LED_GREEN  12
  #define LED_BLUE   2
  int lin_tx =  21;                    
#endif

int lin_en = WB_IO6;  //internal pulldown, EN=0 is sleep mode, EN=1 is normal operation mode.
int lin_wk = WB_IO5;  //low active
// LIN Object
lin_bus lin1(Serial1,LIN_V1, lin_en, lin_wk, lin_tx);
uint8_t slaver_id = 1;
unsigned long baute = 9600;
uint16_t receiveCount = 0;
uint8_t ledON = 255;   //the value control led on
uint8_t ledOFF = 0;    //the value control led off
uint8_t sendData[2] = {0,0}; // 
void setup() 
{  
  pinMode(lin_wk,OUTPUT);
  digitalWrite(lin_wk,LOW);
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
  lin1.slave(baute,slaver_id);
  Serial.println("------------------------------");
  Serial.println(BOARD);
  Serial.println("TEST RAK13005 slaver");
  Serial.println("------------------------------");
}

void loop() 
{
  uint8_t dataSize = 2;
  uint8_t readData[dataSize];
  int ok = lin1.read(readData,dataSize);  //  int ok = lin1.listen(slaver_id,readData,dataSize);
  if(ok==1)
  {   
    Serial.printf("The LIN bus receive count is %d\r\n",receiveCount++);      
    Serial.println("TEST RAK13005 receive data");
    for(uint8_t i=0;i<dataSize;i++)
    {      
      Serial.printf("The data[%d] is %d \r\n",i,readData[i]);        
    }  
    if(readData[0] == ledON)
        { 
          sendData[0] = 0;               
          set_led_on(LED_BLUE);          
        }
        else
        {  
          sendData[0] = 255;                
          set_led_off(LED_BLUE);   
        }
            
        if(readData[1] == ledON)
        { 
          sendData[1] = 0;               
          set_led_on(LED_GREEN);  
        }
        else
        { 
          sendData[1] = 255;     
          set_led_off(LED_GREEN); 
        }
    delay(50);
    lin1.write(slaver_id,sendData,dataSize);   
  }
}
void set_led_on(int pin)
{
  pinMode(pin,OUTPUT);
  digitalWrite(pin,HIGH);
}
void set_led_off(int pin)
{
  pinMode(pin,OUTPUT);
  digitalWrite(pin,LOW);
}
