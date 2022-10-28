/**
   @file AnalogMicSerialPlotter.ino
   @author rakwireless.com
   @brief This example reads PCM audio datas from the analog microphones by I2S, and prints
   out the samples to the Serial console. The Serial Plotter built into the
   Arduino IDE can be used to plot the audio data (Tools -> Serial Plotter)
   @note This example need use the RAK18040 analog microphone module.
   @version 0.1
   @date 2022-08-24
   @copyright Copyright (c) 2022
*/

#include "audio.h" // Click here to install the library: http://librarymanager/All#RAKwireless-Audio

NAU85L40B   MIC(0x1D);

int channels = 1;
// default PCM output frequency
static const int frequency = 16000;
int sampleBit = 16;
// buffer to read samples into, each sample is 16-bits
short sampleBuffer[BUFFER_SIZE];

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
  MIC.config(frequency, (MIC_CHANNEL1|MIC_CHANNEL2 ), I2S_SAMPLE_16BIT); //

  I2S.setI2Sformat(I2S_COMM_FORMAT_STAND_I2S);
  I2S.begin(channels, frequency, sampleBit);
}

void loop() {

  // Read data from microphone
  int sampleRead = I2S.read(sampleBuffer, sizeof(sampleBuffer));

  sampleRead = sampleRead >> 1;
  // wait for samples to be read
  if (sampleRead > 0)
  {
    // print samples to the serial monitor or plotter
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
      if (channels == 2)
      {
        Serial.print("L:");
        Serial.print(sampleBuffer[i]);
        Serial.print(" R:");
        i++;
      }
      Serial.println(sampleBuffer[i]);
    }
    // Clear the read count
    sampleRead = 0;
  }
}
