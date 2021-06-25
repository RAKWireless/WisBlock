/**
 * @file RAK18000_Stereo.ino
 * @author rakwireless
 * @brief  This example reads audio data from the on-board PDM microphones, and prints
 * out the samples to the Serial console. The Serial Plotter built into the
 * Arduino IDE can be used to plot the audio data (Tools -> Serial Plotter)
 * @version 0.1
 * @date 2021-03-07
 *
 * @copyright Copyright (c) 2020
 *
 * @note RAK4631 GPIO mapping to nRF52840 GPIO ports
   RAK4631    <->  nRF52840
   WB_IO1     <->  P0.17 (GPIO 17)
   WB_IO2     <->  P1.02 (GPIO 34)
   WB_IO3     <->  P0.21 (GPIO 21)
   WB_IO4     <->  P0.04 (GPIO 4)
   WB_IO5     <->  P0.09 (GPIO 9)
   WB_IO6     <->  P0.10 (GPIO 10)
   WB_SW1     <->  P0.01 (GPIO 1)
   WB_A0      <->  P0.04/AIN2 (AnalogIn A2)
   WB_A1      <->  P0.31/AIN7 (AnalogIn A7)
 */

#include <PDM.h>

#define PDM_DATA_PIN  21
#define PDM_CLK_PIN   4
#define PDM_PWR_PIN   -1

// buffer to read samples into, each sample is 16-bits
short sampleBuffer[256] = {0};

// number of samples read
volatile int samplesRead = 0;

void setup() {

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

  PDM.setPins(PDM_DATA_PIN, PDM_CLK_PIN, PDM_PWR_PIN);
  PDM.setBufferSize(256);
  // configure the data receive callback
  PDM.onReceive(onPDMdata);
  // optionally set the gain, defaults to 20
  PDM.setGain(20);
  
  // initialize PDM with:
  // - 2 means stereo, 1 means single
  // - a 16 kHz sample rate
  if (!PDM.begin(2, 16000)) 
  {
    Serial.println("Failed to start PDM!");
    while (1);
  }
}

void loop() 
{
    for (int i = 0; i < samplesRead; i++) 
    {
        //Simple filtering, if less than 200, it will be regarded as noise
        if(sampleBuffer[i] <= 200 && sampleBuffer[i] >= -200)
        {
            sampleBuffer[i] = 0;
        } 
        Serial.println(sampleBuffer[i]);
    }
    // clear the read count
	  samplesRead = 0;
}

void onPDMdata() {
  // query the number of bytes available
  int bytesAvailable = PDM.available();

  // read into the sample buffer
  PDM.read((uint8_t *)sampleBuffer, bytesAvailable);
  // 16-bit, 2 bytes per sample
  samplesRead = bytesAvailable/2;
}
