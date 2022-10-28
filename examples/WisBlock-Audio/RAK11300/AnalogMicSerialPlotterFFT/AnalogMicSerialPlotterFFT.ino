/**
   @file AnalogMicSerialPlotterFFT.ino
   @author rakwireless.com
   @brief This example reads audio data from the analog microphones by I2S, and prints
   out the FFT transfer samples to the Serial console. The Serial Plotter built into the
   Arduino IDE can be used to plot the audio data (Tools -> Serial Plotter).
   @note This example need use the RAK18040 analog microphone module.
   @version 0.1
   @date 2022-08-29
   @copyright Copyright (c) 2022
*/

#include <arduino.h>
#include "audio.h" // Click here to install the library: http://librarymanager/All#RAKwireless-Audio

// GPIO pin numbers
#define pDIN 21
NAU85L40B   MIC(0x1D);
// Create the I2S port using a PIO state machine
I2S i2s(INPUT);

const int sampleRate = 16000; // sample rate in Hz
// Audio sample buffers used for analysis and display
int approxBuffer[256];   // ApproxFFT sample buffer
const double samplingFrequency = 16000;
// buffer to read samples into, each sample is 16-bits
short sampleBuffer[256];

int print_string[500] = {0};

void setup() {

  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  delay(500);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, HIGH);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_BLUE, HIGH);

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

  i2s.setDATA(pDIN);
  i2s.setBitsPerSample(16);
  i2s.setFrequency(sampleRate);
  i2s.begin();

  MIC.begin();
  MIC.config(sampleRate, (MIC_CHANNEL1), I2S_SAMPLE_16BIT); //  |MIC_CHANNEL2

  uint8_t discardCount = 10;
  while (discardCount--)
  {
    for (int i = 0; i < 256; i++)
    {
      int32_t value;
      i2s.read(&value, true);
      approxBuffer[i] = (short)((value >> 16) & 0xFFFF);
      Serial.println(approxBuffer[i]);
      //      Serial.printf("%08X\r\n",value);
    }
  }
}

void loop() {

  // Fill the buffers with the samples
  for (int i = 0; i < 256; i++)
  {
    int32_t value;
    i2s.read(&value, true);
    approxBuffer[i] = (short)((value >> 16) & 0xFFFF);
    //      Serial.println(approxBuffer[i]);
    //      Serial.printf("%08X\r\n",value);
  }
  Approx_FFT(approxBuffer, 256, samplingFrequency);

  memset(print_string, 0, sizeof(print_string));
  memcpy(print_string, approxBuffer, sizeof(approxBuffer));
  for (int j = 0; j < 500; j++)
  {
    Serial.println(print_string[j]);
  }
  delay(1000);
}
