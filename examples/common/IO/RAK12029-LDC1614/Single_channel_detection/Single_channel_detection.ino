/**
   @file muti_channel_demo.ino
   @author rakwireless.com
   @brief  Metallic object detected
   @version 1.0
   @date 2021-11-17
   @copyright Copyright (c) 2021
**/
#include "RAK12029_LDC1614.h"

#define INDUCTANCE   13.000
#define CAPATANCE    100.0
#define GETCHANNEL   LDC1614_CHANNEL_0 //LDC1614_CHANNEL_(0~3)
RAK12029_LDC1614_Inductive ldc;

void setup()
{
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_BLUE, LOW);

  //Sensor power switch
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  delay(300);

  //control chip switch
  pinMode(WB_IO5, OUTPUT);
  digitalWrite(WB_IO5, LOW);
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
  Serial.println("RAK12029 TEST");
  delay(1000);
  ldc.LDC1614_init();
  /*single channel use case configuration.*/
  if (ldc.LDC1614_single_channel_config(GETCHANNEL, INDUCTANCE, CAPATANCE))
  {
    Serial.println("can't detect sensor!");
    while (1) delay(100);
  }
}

void loop()
{
  u16 value = 0;
  ldc.IIC_read_16bit(LDC1614_READ_DEVICE_ID, &value);
  if (value == 0x3055)
  {
    u32 ChannelData = 0;
    /*Get channel 0 result and parse it.*/
    delay(100);
    if (ldc.LDC1614_get_channel_result(GETCHANNEL, &ChannelData) == 0)
    {
      /*sensor result value.you can make a lot of application according to its changes.*/
      if (0 != ChannelData)
      {
        Serial.printf("The result channel%d is:%d\r\n", 4 - GETCHANNEL, ChannelData);
        digitalWrite(LED_BLUE, HIGH);
      }
      delay(1000);
      digitalWrite(LED_BLUE, LOW);
    }
  }
  else
  {
    Serial.println("The IIC communication fails,please reset the sensor!");
    delay(500);
  }
}
