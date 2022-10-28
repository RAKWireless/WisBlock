/*
   @file AnalogMicAudio_Alarm.ino
   @author rakwireless.com
   @brief The analog microphone detects the noise value and lights up the LED on the WisBase
    when it exceeds the set threshold.
   @note This example need use the RAK18040 analog microphone module.
   @version 0.1
   @date 2022-08-29
   @copyright Copyright (c) 2020
*/
#include "audio.h" // Click here to install the library: http://librarymanager/All#RAKwireless-Audio

NAU85L40B   MIC(0x1D);

int channels = 1;
// default PCM output frequency
static const int frequency = 16000;
int sampleBit = 16;
// buffer to read samples into, each sample is 16-bits
short sampleBuffer[BUFFER_SIZE];
int audio_threshold = 800;
int aver = 0;
int g_alarm = 0;
int abs_int(short data);

void setup() {

  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  delay(500);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, HIGH);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_BLUE, HIGH);

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

  MIC.begin();
  MIC.config(frequency, (MIC_CHANNEL1 | MIC_CHANNEL2 ), I2S_SAMPLE_16BIT); //

  I2S.setI2Sformat(I2S_COMM_FORMAT_STAND_I2S);
  I2S.begin(channels, frequency, sampleBit);
  delay(500);
  Serial.println("This is an noise alarm example.");
}

void loop()
{
  // Read data from analog microphone by I2S
  int sampleRead = I2S.read(sampleBuffer, sizeof(sampleBuffer));
  sampleRead = sampleRead >> 1;
  // wait for samples to be read
  if (sampleRead > 0)
  {
    uint32_t sum = 0;
    // print samples to the serial monitor or plotter
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
      sum = sum + abs(sampleBuffer[i]);
    }
    aver = sum / BUFFER_SIZE;
    sampleRead = 0;

    if (aver > audio_threshold)
    {
      g_alarm++;
      Serial.printf("Alarm %d\r\n", g_alarm);
      digitalWrite(LED_BLUE, HIGH);
      digitalWrite(LED_GREEN, HIGH);
      delay(1000);
      digitalWrite(LED_BLUE, LOW);
      digitalWrite(LED_GREEN, LOW);
    }
  }
}

int abs_int(short data)
{
  if (data > 0) return data;
  else return (0 - data);
}
