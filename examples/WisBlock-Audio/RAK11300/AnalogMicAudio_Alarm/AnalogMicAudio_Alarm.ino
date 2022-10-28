/**
   @file AnalogMicAudio_Alarm.ino
   @author rakwireless.com
   @brief The analog microphone detects the noise threshold and when the ambient noise is greater than the set threshold,
   a warning will be generated. And the LED of WisBase will lights.
   @note This example need use the RAK18040 analog microphone module.
   @version 0.1
   @date 2022-08-29

   @copyright Copyright (c) 2022
*/
#include "audio.h" // Click here to install the library: http://librarymanager/All#RAKwireless-Audio
#include <Arduino.h>

// GPIO pin numbers
#define pDIN 21

NAU85L40B   MIC(0x1D);

// Create the I2S port using a PIO state machine
I2S i2s(INPUT);
const int sampleRate = 16000; // sample rate in Hz
// buffer to read samples into, each sample is 16-bits
short sampleBuffer[BUFFER_SIZE];
//Alarm threshold
int audio_threshold = 1000;   //The threshold value of the noise trigger can be modified according to the environmental conditions
int g_alarm = 0;

int abs_int(short data);

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
    if ((millis() - timeout) < 3000)
    {
      delay(100);
    }
    else
    {
      break;
    }
  }
  i2s.setDATA(pDIN);
  i2s.setBitsPerSample(16);
  i2s.setFrequency(sampleRate);
  i2s.begin();

  MIC.begin();
  MIC.config(sampleRate, (MIC_CHANNEL1), I2S_SAMPLE_16BIT); //  |MIC_CHANNEL2
  delay(500);
  Serial.println("=====================================");
}
void loop()
{
  uint32_t sum = 0;
  // print samples to the serial monitor or plotter
  for (int i = 0; i < BUFFER_SIZE; i++) {
    int16_t l = 0, r = 0;
    i2s.read16(&l, &r);
    sampleBuffer[i] = l;
    sum = sum + abs(sampleBuffer[i]);
  }
  int aver = sum / BUFFER_SIZE;
  if (aver > audio_threshold)
  {    
    /*You can add your alarm processing tasks here*/
    g_alarm++;
    Serial.printf("Alarm %d\r\n",g_alarm);     
    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    delay(1000);
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_GREEN, LOW);   
  }

}
int abs_int(short data)
{
  if (data > 0) return data;
  else return (0 - data);
}
