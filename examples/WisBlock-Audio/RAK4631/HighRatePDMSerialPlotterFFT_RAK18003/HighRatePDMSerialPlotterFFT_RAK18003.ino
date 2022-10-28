/**
   @file HighRatePDMSerialPlotterFFT_RAK18003.ino
   @author rakwireless.com
   @brief This example reads audio data from the PDM microphones, and prints
   out the FFT transfer samples to the Serial console. The Serial Plotter built into the
   Arduino IDE can be used to plot the audio data (Tools -> Serial Plotter)
   @note  This example need use the microphones that can supports higher sampling rate. Also need use the RAK18003 module
   @version 0.1
   @date 2022-06-6
   @copyright Copyright (c) 2022
*/
#include <Arduino.h>
#ifdef NRF52_SERIES
#include <Adafruit_TinyUSB.h>
#endif

#include "audio.h" // Click here to install the library: http://librarymanager/All#RAKwireless-Audio

TPT29555   Expander1(0x23);
TPT29555   Expander2(0x25);

// Audio sample buffers used for analysis and display
int approxBuffer[BUFFER_SIZE];   // ApproxFFT sample buffer
const double samplingFrequency = 48000;
int channels = 1;
// buffer to read samples into, each sample is 16-bits
short sampleBuffer[BUFFER_SIZE];

int print_string[500] = {0};

volatile uint8_t read_flag = 0;
void onPDMdata();
void RAK18003Init(void);

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
    if ((millis() - timeout) < 3000)
    {
      delay(100);
    }
    else
    {
      break;
    }
  }
  RAK18003Init();

  // configure the data receive callback
  PDM.onReceive(onPDMdata);
  PDM.setPins(PDM_DATA_PIN, PDM_CLK_PIN, PDM_PWR_PIN);
  // optionally set the gain, defaults to 20
  PDM.setGain(30);
  // initialize PDM with:
  // - one channel (mono mode)
  // - a 200 kHz sample rate
  if (!PDM.begin(channels, PCM_48000)) {
    Serial.println("Failed to start PDM!");
    while (1) yield();
  }
  delay(500);
  Serial.println("=====================FFT example =====================");
}

static int first_flag = 0;
void loop() {
  // wait for samples to be read
  if (read_flag == 1) {
    read_flag = 0;

    // Fill the buffers with the samples
    for (int i = 0; i < BUFFER_SIZE; i++) {
      approxBuffer[i] = sampleBuffer[i];
      //       Serial.println(approxBuffer[i]);
    }

    if (first_flag > 5) //because the all of first IRQ read data is 0
    {
      Approx_FFT(approxBuffer, BUFFER_SIZE, samplingFrequency);
      //      for (int j=0; j<BUFFER_SIZE; j++){
      //        Serial.println(approxBuffer[j]);
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
  }
}
void onPDMdata() {
  // read into the sample buffer
  PDM.read(sampleBuffer, BUFFER_SIZE * 2);
  read_flag = 1;
}
void RAK18003Init(void)
{
  while (!Expander1.begin())
  {
    Serial.println("Did not find RAK18003 IO Expander Chip1,please check!");
    delay(500);
  }

  while (!Expander2.begin())
  {
    Serial.println("Did not find RAK18003 IO Expander Chip2,please check!");
    delay(500);
  }
  Expander1.pinMode(0, INPUT);    //SD check
  Expander1.pinMode(1, INPUT);    //MIC check
  Expander1.pinMode(2, INPUT);    //MIC CTR1
  Expander1.pinMode(3, INPUT);    //MIC CTR2
  Expander1.pinMode(4, INPUT);    //AMP check
  Expander1.pinMode(5, INPUT);    //AMP CTR1
  Expander1.pinMode(6, INPUT);    //AMP CTR2
  Expander1.pinMode(7, INPUT);    //AMP CTR3
  Expander1.pinMode(8, INPUT);    //DSP check
  Expander1.pinMode(9, INPUT);    //DSP CTR1  DSP int
  Expander1.pinMode(10, INPUT);   //DSP CTR2  DSP ready
  Expander1.pinMode(11, OUTPUT);  //DSP CTR3  DSP reset
  Expander1.pinMode(12, INPUT);   //DSP CTR4  not use
  Expander1.pinMode(13, INPUT);   //DSP CTR5  not use
  Expander1.pinMode(14, INPUT);   //NOT USE
  Expander1.pinMode(15, INPUT);   //NOT USE

  //  Expander1.digitalWrite(14, 0);    //set chip 1 not use pin output low
  //  Expander1.digitalWrite(15, 0);    //set chip 1 not use pin output low

  Expander2.pinMode(0, OUTPUT);  //CORE  SPI CS1 for DSPG CS
  Expander2.pinMode(1, OUTPUT);  //CORE  SPI CS2
  Expander2.pinMode(2, OUTPUT);  //CORE  SPI CS3
  Expander2.pinMode(3, OUTPUT);  //PDM switch CTR    1 to dsp   0 to core
  Expander2.pinMode(4, INPUT);  //not use
  Expander2.pinMode(5, INPUT);  //not use
  Expander2.pinMode(6, INPUT);  //not use
  Expander2.pinMode(7, INPUT);  //not use
  Expander2.pinMode(8, INPUT);  //not use
  Expander2.pinMode(9, INPUT);  //not use
  Expander2.pinMode(10, INPUT); //not use
  Expander2.pinMode(11, INPUT); //not use
  Expander2.pinMode(12, INPUT); //not use
  Expander2.pinMode(13, INPUT); //not use
  Expander2.pinMode(14, INPUT); //not use
  Expander2.pinMode(15, INPUT); //not use

  Expander2.digitalWrite(0, 1);  //set SPI CS1 High
  Expander2.digitalWrite(1, 1);  //set SPI CS2 High
  Expander2.digitalWrite(2, 1);  //set SPI CS3 High

  Expander2.digitalWrite(3, 0);   //set the PDM data direction from MIC to WisCore

  // if(Expander1.digitalRead(0) == 1)  //Check SD card
  // {
  //   Serial.println("There is no SD card on the RAK18003 board, please check !");
  // }

  while (Expander1.digitalRead(1) == 0) //Check if the microphone board is connected on the RAK18003
  {
    Serial.println("There is no microphone board, please check !");
    delay(500);
  }

  // if(Expander1.digitalRead(4) == 0)  //Check if the RAK18060 AMP board is connected on the RAK18003
  // {
  //   Serial.println("There is no RAK18060 AMP board, please check !");
  // }

  // if(Expander1.digitalRead(8) == 0)  //Check if the RAK18080 DSPG board is connected on the RAK18003
  // {
  //   Serial.println("There is no RAK18080 DSPG board, please check !");
  // }

}
