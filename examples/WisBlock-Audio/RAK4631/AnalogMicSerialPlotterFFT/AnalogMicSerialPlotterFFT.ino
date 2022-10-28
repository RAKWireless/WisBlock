/**
   @file AnalogMicSerialPlotterFFT.ino
   @author rakwireless.com
   @brief This example reads PCM audio datas from the analog microphones by I2S, and prints
   out the FFT transfer samples to the Serial console. The Serial Plotter built into the
   Arduino IDE can be used to plot the audio data (Tools -> Serial Plotter)
   @note This example need use the RAK18040 analog microphone module.
   @version 0.1
   @date 2022-06-6
   @copyright Copyright (c) 2022
*/

#include <Arduino.h>
#ifdef NRF52_SERIES
#include <Adafruit_TinyUSB.h>
#endif

#include "audio.h" // Click here to install the library: http://librarymanager/All#RAKwireless-Audio

NAU85L40B   MIC(0x1D);

#define I2S_DATA_BLOCK_WORDS    512

i2s_channels_t channels =  Stereo ; //Right、   Left;//
int frequency = 16000;
int sampleBit = 16;

uint32_t readbuff[I2S_DATA_BLOCK_WORDS] = {0};
int16_t leftChannel[512] = {0};
int16_t rightChannel[512] = {0};

// Audio sample buffers used for analysis and display
int approxBuffer[2][256];   // ApproxFFT sample buffer
const double samplingFrequency = 16000;     // Frequency range plotted is 2.5kHz (sampling frequency/4)

int print_string[500] = {0};

volatile uint8_t rx_flag = 1;
static uint8_t first_flag = 0;

void i2s_config();
void rx_irq();

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
  MIC.config(frequency, (MIC_CHANNEL1 | MIC_CHANNEL2), I2S_SAMPLE_16BIT); //|MIC_CHANNEL2
  i2s_config();
  Serial.println("=====================FFT example =====================");
}

void loop() {

  // wait for samples to be read
  if (rx_flag == 1)
  {
    rx_flag = 0;
    I2S.read(&readbuff, sizeof(readbuff));
    // Fill the buffers with the samples
    memset(approxBuffer, 0, sizeof(approxBuffer));
    // print samples to the serial monitor or plotter
    for (int i = 0; i < I2S_DATA_BLOCK_WORDS; i++)
    {
      if (channels == Stereo)
      {
        uint32_t const * p_word = &readbuff[i];
        leftChannel[i] = ((uint16_t const *)p_word)[0];
        rightChannel[i] = ((uint16_t const *)p_word)[1];
      }
      else
      {
        uint32_t const * p_word = &readbuff[i];
        int16_t temp = ((uint8_t const *)p_word)[3];
        temp = (int16_t)((temp << 8) | ((uint8_t const *)p_word)[1]);

        leftChannel[i] = temp;

        temp = 0;
        temp = ((uint8_t const *)p_word)[2];
        temp = (int16_t)((temp << 8) | ((uint8_t const *)p_word)[0]);
        rightChannel[i] = temp;
      }
    }//for
    //      Serial.printf("readbuff[i]%08X \t L:%04X \t R:%04X\r\n",readbuff[i],(uint16_t)leftChannel[i],(uint16_t)rightChannel[i]);
    //      Serial.print("L:");
    //      Serial.print(leftChannel[i]);
    //      Serial.print(" R:");
    //      Serial.println(rightChannel[i]);
    //      Serial.printf("%08X\t\tL:%04X\tR:%04X\t\r\n",readbuff[i],leftChannel[i],rightChannel[i]);

    if (first_flag > 10)  //Discard the first 10 samples data
    {
      int n = 0;
      for (int i = 0; i < 256; i++)
      {
        if (channels == Left)
        {
          approxBuffer[0][i] = leftChannel[n];
        }
        else
        {
          approxBuffer[0][i] = rightChannel[n];
        }
        n++;
      }

      for (int i = 0; i < 256; i++)
      {
        if (channels == Left)
        {
          approxBuffer[1][i] = leftChannel[n];
        }
        else
        {
          approxBuffer[1][i] = rightChannel[n];
        }
        n++;
      }


      Approx_FFT(approxBuffer[0], 256, samplingFrequency);
      memset(print_string, 0, sizeof(print_string));
      memcpy(print_string, approxBuffer[0], sizeof(approxBuffer[0]));
      for (int j = 0; j < 500; j++)
      {
        Serial.println(print_string[j]);
      }

      delay(500);
      Approx_FFT(approxBuffer[1], 256, samplingFrequency);
      memset(print_string, 0, sizeof(print_string));
      memcpy(print_string, approxBuffer[1], sizeof(approxBuffer[1]));
      for (int j = 0; j < 500; j++)
      {
        Serial.println(print_string[j]);
      }
      delay(500);
    }
    else
    {
      first_flag += 1;
    }
  }
}

void rx_irq()  ///< Pointer to the buffer with data to be sent.
{
  rx_flag = 1;
  //  I2S.read(&readbuff, sizeof(readbuff));
}
void i2s_config()
{
  I2S.RxIRQCallBack(rx_irq);
  I2S.begin(channels, frequency, sampleBit);
  I2S.start();
}
