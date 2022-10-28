/**
   @file AnalogMicAudio_Alarm.ino
   @author rakwireless.com
   @brief The analog microphone detects the noise threshold .
   When the ambient noise is greater than the set threshold, a warning will be generated.
   And the LED of WisBase will lights 1 seconds.
   @note This example need use the RAK18040 analog microphone module.
   @version 0.1
   @date 2022-06-10
   @copyright Copyright (c) 2022
*/
#include <Arduino.h>
#ifdef NRF52_SERIES
#include <Adafruit_TinyUSB.h>
#endif
#include "audio.h" // Click here to install the library: http://librarymanager/All#RAKwireless-Audio

NAU85L40B   MIC(0x1D);

#define I2S_DATA_BLOCK_WORDS    512

i2s_channels_t channels =  Stereo ; //Right、   Left;//
int frequency = 16000;
int sampleBit = 16;

uint32_t readbuff[I2S_DATA_BLOCK_WORDS] = {0};
int16_t leftChannel[512] = {0};
int16_t rightChannel[512] = {0};

//Alarm threshold
int audio_threshold = 800; //You can modify this value to your desired noise trigger threshold.
int alarm_count = 0;

volatile uint8_t rx_flag = 1;

int abs_int(short data);
void i2s_config();
void rx_irq();

void setup()
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  delay(500);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_GREEN, LOW);

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
  MIC.begin();
  MIC.config(frequency, (MIC_CHANNEL1 | MIC_CHANNEL2), I2S_SAMPLE_16BIT); //
  i2s_config();
}

void loop()
{
  // wait for samples to be read
  if (rx_flag == 1)
  {
    rx_flag = 0;
    int sumLeft = 0;
    int sumRight = 0;
    I2S.read(&readbuff, sizeof(readbuff));
    for (int i = 0; i < I2S_DATA_BLOCK_WORDS; i++)
    {
      if(channels == Stereo)
      {
        uint32_t const * p_word = &readbuff[i];
        leftChannel[i] = ((uint16_t const *)p_word)[0];
        rightChannel[i] = ((uint16_t const *)p_word)[1]; 
      }
      else
      {
        uint32_t const * p_word = &readbuff[i];        
        int16_t temp = ((uint8_t const *)p_word)[3];
        temp = (int16_t)((temp<<8)|((uint8_t const *)p_word)[1]);
        leftChannel[i] = temp;
        temp = 0;
        temp = ((uint8_t const *)p_word)[2];
        temp = (int16_t)((temp<<8)|((uint8_t const *)p_word)[0]);
        rightChannel[i] = temp;   
      }       

      sumLeft = sumLeft+abs(leftChannel[i]);
      sumRight = sumRight+abs(rightChannel[i]);
//      Serial.print("L:");
//      Serial.print(leftChannel[i]);
//      Serial.print(" R:");
//      Serial.println(rightChannel[i]);
    }      
    int averLeft = sumLeft / I2S_DATA_BLOCK_WORDS;
    int averRight = sumRight / I2S_DATA_BLOCK_WORDS;    
    if ((averLeft > audio_threshold)||(averRight > audio_threshold))
    {      
      alarm_count++;        
    }
  }

  if (alarm_count > 2)
  {
    alarm_count = 0;
    Serial.println("Alarm");
    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    delay(1000);
    /*You can add your alarm processing tasks here*/
  }
  else
  {
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_GREEN, LOW);
  }
}
int abs_int(short data)
{
  if (data > 0) return data;
  else return (0 - data);
}

void rx_irq()
{
  rx_flag = 1;
  //  I2S.read(&readbuff,sizeof(readbuff));
}
void i2s_config()
{
  I2S.RxIRQCallBack(rx_irq);
  I2S.begin(channels, frequency, sampleBit);
  I2S.start();
}
