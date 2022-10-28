/**
 * @file PlayBack48K.ino
 * @author rakwireless.com
 * @brief This example use RAK18060 Play mono audio file data with sampling rate of 48KHz 
 * and sampling depth of 16 bits mono. The test audio file in the library of "sound" folder.
 * The volume can be set from 0 to 21, and the appropriate volume can be set according to your speaker situation.
 * @How to convert WAV file to HEX format .h file can use the WAVconvert.py in the 'tool' folder.
 * @note This example need use the battery power for the WisBase.
 * @version 0.1
 * @date 2022-06-20
 * @copyright Copyright (c) 2022
 */
#include "audio.h" // Click here to install the library: http://librarymanager/All#RAKwireless-Audio
#include "sound.h"
#include <I2S.h>

Audio rak_audio;

TAS2560 AMP_Left;
TAS2560 AMP_Right;

#define AMP_LEFT_ADDRESS    0x4c    //amplifier i2c address
#define AMP_RIGTT_ADDRESS   0x4f    //amplifier i2c address

// Create the I2S port using a PIO state machine
I2S i2s(OUTPUT);

const int sampleRate = 48000; // sample rate in Hz
int audio_length = 0;
int wavHead = 44;   //WAV file head 44 Bytes

void setup()
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  delay(500);
  // Initialize Serial for debug output
  time_t timeout = millis();
  Serial.begin(115200);
  while (!Serial)
  {
    if ((millis() - timeout) < 2000)
    {
      delay(100);
    }
    else
    {
      break;
    }
  }

  Serial.println("=====================================");  
  if(!AMP_Left.begin(AMP_LEFT_ADDRESS))
  {
    Serial.printf("TAS2560 left init failed\r\n");
  }

  AMP_Left.set_pcm_channel(LeftMode);
  if(!AMP_Right.begin(AMP_RIGTT_ADDRESS))
  {
    Serial.printf("TAS2560 rigth init failed\r\n");
  } 

  AMP_Right.set_pcm_channel(RightMode);

  rak_audio.setVolume(6);  //The volume level can be set to 0-21
   
  i2s.setBitsPerSample(16);   //Set SampleBits 16
  // start I2S at the sample rate with 16-bits per sample
  if (!i2s.begin(sampleRate)) {
    Serial.println("Failed to initialize I2S!");
    while (1); // do nothing
  }
  audio_length = sizeof(sound_buff) / 2; //The sampling bit depth is 16 bits contains two bytes
  Serial.println("=====================================");
  Serial.println("start play");
  Serial.println("=====================================");
}

void loop()
{
 int16_t sample[2] = {0};
for (int i = 0; i < audio_length; i++)
  {
    sample[0] = sound_buff[i * 2 + 1];
    sample[0] = (sample[0] << 8) | sound_buff[i * 2];
    sample[1] = sample[0];  //copy left channel data to the right channel.
   
    int32_t s32 = rak_audio.Gain(sample); // vosample2lume;
    int16_t left_channel = (s32 >> 16) & 0xffff;
    int16_t right_channel = s32 & 0xffff;
    i2s.write(left_channel, right_channel);
    
//    uint32_t temp = left_channel;
//    temp = (temp<<16)|right_channel;
//    i2s.write(s32, true);; 
  }
  delay(1000);
//    while(1);   //If comment out this line can repeat play
}
