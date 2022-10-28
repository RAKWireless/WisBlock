/**
   @file AnalogMicSerialPlotter.ino
   @author rakwireless.com
   @brief This example reads audio data from the PDM microphones, and prints
   out the samples to the Serial console. The Serial Plotter built into the
   Arduino IDE can be used to plot the audio data (Tools -> Serial Plotter)
   @note This example need use the RAK18040 analog microphone module.
   @version 0.1
   @date 2022-06-6
   @copyright Copyright (c) 2022
*/
#include "audio.h" // Click here to install the library: http://librarymanager/All#RAKwireless-Audio
#include <I2S.h>

// GPIO pin numbers
#define pDIN 21

NAU85L40B   MIC(0x1D);

// Create the I2S port using a PIO state machine
I2S i2s(INPUT);

const int sampleRate = 16000; // sample rate in Hz

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

  i2s.setDATA(pDIN);
  i2s.setBitsPerSample(16);
  i2s.setFrequency(sampleRate);
  i2s.begin();

  MIC.begin();
  MIC.config(sampleRate, (MIC_CHANNEL1), I2S_SAMPLE_16BIT); //  |MIC_CHANNEL2
}

void loop() {

  int32_t value;
  int16_t leftChannel;
  i2s.read(&value, true);
  leftChannel = (int16_t)(value >> 16) & 0xFFFF;
  Serial.println((int16_t)leftChannel);
}
