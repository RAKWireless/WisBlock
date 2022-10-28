/**
   @file PDMSerialPlotterFFT.ino
   @author rakwireless.com
   @brief This example reads audio data from the PDM microphones, and prints
   out the FFT transfer samples to the Serial console. The Serial Plotter built into the
   Arduino IDE can be used to plot the audio data (Tools -> Serial Plotter)
   @note  This example need use the microphones that can supports higher sampling rate.
   @version 0.1
   @date 2022-06-6
   @copyright Copyright (c) 2022
*/

#include <arduino.h>
#include "audio.h" // Click here to install the library: http://librarymanager/All#RAKwireless-Audio

int channels = 1;
// default PCM output frequency
static const int frequency = 48000;
// buffer to read samples into, each sample is 16-bits
short sampleBuffer[BUFFER_SIZE];

// Audio sample buffers used for analysis and display
int approxBuffer[BUFFER_SIZE];   // ApproxFFT sample buffer
const double samplingFrequency = 48000;     //

int print_string[500] = {0};

void onPDMdata();

void setup() {

  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  delay(500);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, HIGH);

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

  // initialize PDM with:
  // - one channel (mono mode)
  // - a 200 kHz sample rate
  // default PCM output frequency
  if (!PDM.begin(channels, frequency)) {
    Serial.println("Failed to start PDM!");
    while (1) yield();
  }

  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_BLUE, HIGH);
}
static uint8_t first_flag = 0;
void loop() {

  // Read data from microphone
  int sampleRead = PDM.read(sampleBuffer, sizeof(sampleBuffer));

  sampleRead = sampleRead >> 1; //each sample data with two byte
  // wait for samples to be read
  if (sampleRead > 0) {
    // Fill the buffers with the samples
    memset(approxBuffer, 0, sizeof(approxBuffer));

    for (int i = 0; i < BUFFER_SIZE; i++) {
      approxBuffer[i] = sampleBuffer[i];
      //Serial.println(approxBuffer[i]);
    }

    if (first_flag > 10)  //Discard the first 10 samples of data
    {
      Approx_FFT(approxBuffer, BUFFER_SIZE, samplingFrequency);
      //      for (int j=0; j<BUFFER_SIZE; j++){
      //      Serial.println(approxBuffer[j]);
      //      }
      memset(print_string, 0, sizeof(print_string));
      memcpy(print_string, approxBuffer, sizeof(approxBuffer));

      for (int j = 0; j < 500; j++)
      {
        Serial.println(print_string[j]);
      }
      delay(1000);
    }
    else
    {
      first_flag++;
    }
    sampleRead = 0;
  }
}
