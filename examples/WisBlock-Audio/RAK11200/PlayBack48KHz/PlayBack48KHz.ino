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

Audio rak_audio;

TAS2560 AMP_Left;
TAS2560 AMP_Right;

#define AMP_LEFT_ADDRESS    0x4c    //amplifier i2c address
#define AMP_RIGTT_ADDRESS   0x4f    //amplifier i2c address

int channels = 1;   //mono
static const int frequency = 48000;   //samplerate 48KHz

void setup() {
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
  if (!AMP_Left.begin(AMP_LEFT_ADDRESS))
  {
    Serial.printf("TAS2560 left init failed\r\n");
  }

  AMP_Left.set_pcm_channel(LeftMode);
  if (!AMP_Right.begin(AMP_RIGTT_ADDRESS))
  {
    Serial.printf("TAS2560 rigth init failed\r\n");
  }

  AMP_Right.set_pcm_channel(RightMode);
  rak_audio.setVolume(6);    //The volume level can be set to 0-21

  I2S.setSampleBit(16);
  I2S.begin(channels, frequency);
  Serial.println("start play");
  Serial.println("=====================================");
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, HIGH);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_BLUE, HIGH);
}

void loop()
{
  int16_t sample[2] = {0};
  int16_t *p_word = NULL;

  uint32_t sound_size = sizeof(sound_buff) / 2;
  Serial.println("start play");
  for (int i = 0; i < sound_size; i++)
  {
    p_word = &sample[0];
    ((uint8_t *)p_word)[1] = sound_buff[i * 2 + 1];
    ((uint8_t *)p_word)[0] = sound_buff[i * 2];

    if (channels == 1)
    {
      i++;
      p_word = &sample[1];
      ((uint8_t *)p_word)[1] = sound_buff[i * 2 + 1];
      ((uint8_t *)p_word)[0] = sound_buff[i * 2];
    }
    else
    {
      sample[1] = sample[0];
    }
    uint32_t s32 = rak_audio.Gain(sample); //
    I2S.write((void *) &s32, sizeof(uint32_t));
  }
  delay(100);
  I2S.clearDMA();
  delay(500);
}
