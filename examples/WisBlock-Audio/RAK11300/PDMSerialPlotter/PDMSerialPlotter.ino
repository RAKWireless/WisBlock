/**
   @file PDMSerialPlotter.ino
   @author rakwireless.com
   @brief This example reads audio data from the PDM microphones, and prints
   out the samples to the Serial console. The Serial Plotter built into the
   Arduino IDE can be used to plot the audio data (Tools -> Serial Plotter)
   @version 0.1
   @date 2022-06-6
   @copyright Copyright (c) 2022
*/

#include "audio.h" // Click here to install the library: http://librarymanager/All#RAKwireless-Audio
Channel_Mode channels = left; // stereo or left or right
int frequency = 16000;
// buffer to read samples into, each sample is 16-bits
short sampleBuffer[BUFFER_SIZE];

volatile uint8_t read_flag = 0;
void onPDMdata();

void setup() {

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

  // configure the data receive callback
  PDM.onReceive(onPDMdata);
  // initialize PDM with:
  // - one channel (mono mode)
  // - a 16 kHz sample rate
  if (!PDM.begin(channels, frequency)) {
    Serial.println("Failed to start PDM!");
    while (1);
  }
  delay(500);
  Serial.println("=====================================");
  Serial.println("Audio test");
  Serial.println("=====================================");
}

void loop() {
  // wait for samples to be read
  if (read_flag == 1) {
    read_flag = 0;
    // print samples to the serial monitor or plotter
    for (int i = 0; i < BUFFER_SIZE; i++) {

      Serial.println(sampleBuffer[i]);
    }
  }
}
void onPDMdata() {
  // read into the sample buffer
  PDM.read(sampleBuffer, sizeof(sampleBuffer));
  read_flag = 1;
}
