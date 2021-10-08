/**
 * @file RAK13005_linbus_master.ino
 * @author rakwireless.com
 * @brief linbus master send example.
 * @version 0.1
 * @date 2021-05-01
 * @copyright Copyright (c) 2021
 */
#include <Wire.h>
#include  "lin_bus.h"    //library: http://librarymanager/All#RAKwireless_TLE7259_LIN_Bus_library

#if defined(_VARIANT_RAK4630_)  
  #define BOARD "RAK4631 "
  int lin_tx = 16;
#elif defined(_VARIANT_RAK11300_) 
  #define BOARD "RAK11300 "  
  int lin_tx = 0;   
#else  
  #define BOARD "RAK11200 "   //default board is RAK11200
  int lin_tx = 21;           
#endif

int lin_en = WB_IO6;  //internal pulldown, EN=0 is sleep mode, EN=1 is normal operation mode.
int lin_wk = WB_IO5;  //low active
// LIN Object
lin_bus lin1(Serial1,LIN_V1, lin_en, lin_wk, lin_tx);
unsigned long baute = 9600;
uint16_t sendCount = 0;
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
  uint8_t package[8] = {0,1,2,3,255,5,6,100}; // 
  lin1.write(0x01, package, 8); // Write data to LIN 
  Serial.println("Send data is:");
  for(byte i=0;i<8;i++) 
  {
   Serial.printf("  %d  ",package[i]); 
  }
  Serial.println();
  Serial.printf("The LIN bus Send count is %d\r\n",sendCount++);
  delay(500);
}
