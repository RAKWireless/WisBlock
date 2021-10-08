/**
 * @file RAK13005_linbus_master.ino
 * @author rakwireless.com
 * @brief linbus master send data to slaver and receive data from slaver
 * @version 0.1
 * @date 2021-05-01
 * @copyright Copyright (c) 2021
 */
#include <Wire.h>
#include  "lin_bus.h"   //library: http://librarymanager/All#RAKwireless_TLE7259_LIN_Bus_library

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
unsigned long baute = 9600;
uint16_t sendCount = 0;
uint16_t receiveCount = 0;
uint8_t ledON = 255;   //the value control led on
uint8_t ledOFF = 0;    //the value control led off
int dataSize = 2;
uint8_t readData[2];
uint8_t sendData[2] = {0,0}; //   
void setup() 
{  
  pinMode(lin_wk,OUTPUT);
  digitalWrite(lin_wk,HIGH);
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
  lin1.master(baute);
  Serial.println("------------------------------");
  Serial.println(BOARD);
  Serial.println("TEST RAK13005 master");
  Serial.println("------------------------------");
}

void loop() 
{ 
  lin1.write(0x01, sendData, dataSize); // Write data to LIN  
  time_t timeout = millis();
   while((millis() - timeout) < 500)
    {
      int ok = lin1.listen(0x01,readData,dataSize);   //read slaver
      if(ok==1)
      {
        if(readData[0] == ledON)
        {
          Serial.println("LED1 is on");
          sendData[0] = ledOFF;
          set_led_on(LED_GREEN);          
        }
        else
        {
          Serial.println("LED1 is off");
          sendData[0] = ledON;
          set_led_off(LED_GREEN);   
        }
            
        if(readData[1] == ledON)
        {
          Serial.println("LED2 is on");
          sendData[1] = ledOFF;
          set_led_on(LED_BLUE);  
        }
        else
        {
          Serial.println("LED2 is off");
          sendData[1] = ledON;
          set_led_off(LED_BLUE); 
        } 
       Serial.printf("The LIN bus receive count is %d\r\n",receiveCount++);      
      }     
     }
   Serial.printf("The LIN bus Send count is %d\r\n",sendCount++);   
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
