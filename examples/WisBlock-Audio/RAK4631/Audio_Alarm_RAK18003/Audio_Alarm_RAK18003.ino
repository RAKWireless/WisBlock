/**
   @file Audio_Alarm_RAK18003.ino
   @author rakwireless.com
   @brief The microphone detects the noise threshold .
   When the ambient noise is greater than the set threshold, a warning will be generated.
   And the LED of WisBase will lights 1 seconds.
   @note This need use the RAK18003 module.
   @version 0.1
   @date 2022-06-10
   @copyright Copyright (c) 2020
*/
#include <Arduino.h>
#include "audio.h" // Click here to install the library: http://librarymanager/All#RAKwireless-Audio

TPT29555   Expander1(0x23);
TPT29555   Expander2(0x25);

int channels = 1;   //1 mmono , 2 stereo
// buffer to read samples into, each sample is 16-bits
short sampleBuffer[BUFFER_SIZE] = {0};
volatile uint8_t read_flag = 0;

//Alarm threshold
int audio_threshold = 1000; //You can modify this value to your desired noise trigger threshold.

int abs_int(short data);
void RAK18003Init(void);

void setup()
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  delay(500);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_GREEN, LOW);

  // Initialize Serial for debug output
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

  RAK18003Init();

  // start pdm
  PDM.setPins(PDM_DATA_PIN, PDM_CLK_PIN, PDM_PWR_PIN);
  // configure the data receive callback
  PDM.onReceive(onPDMdata);
  // optionally set the gain, defaults to 20
  PDM.setGain(20);
  // initialize PDM with:
  // - one channel (mono mode)
  // - a 16 kHz sample rate
  if (!PDM.begin(channels, PCM_16000)) {
    Serial.println("Failed to start PDM!");
    while (1) yield();
  }
  delay(500);
}

void loop()
{
  // wait for samples to be read
  if (read_flag == 1) {
    read_flag = 0;
    uint32_t sum = 0;
    // print samples to the serial monitor or plotter
    for (int i = 0; i < BUFFER_SIZE; i++) {
      sum = sum + abs(sampleBuffer[i]);
    }
    int aver = sum / BUFFER_SIZE;
    if (aver > audio_threshold)
    {
      Serial.println("Alarm");
      digitalWrite(LED_BLUE, HIGH);
      digitalWrite(LED_GREEN, HIGH);
      delay(1000);
      digitalWrite(LED_BLUE, LOW);
      digitalWrite(LED_GREEN, LOW);
    }
  }
}
int abs_int(short data)
{
  if (data > 0) return data;
  else return (0 - data);
}
void onPDMdata() {
  // query the number of bytes available
  // read into the sample buffer
  PDM.read((uint8_t *)sampleBuffer, BUFFER_SIZE * 2);
  read_flag = 1;
}
void RAK18003Init(void)
{
  while (!Expander1.begin())
  {
    Serial.println("Did not find IO RAK18003 Expander Chip1,please check!");
  }

  if (!Expander2.begin())
  {
    Serial.println("Did not find RAK18003 IO Expander Chip2,please check!");
  }
  Expander1.pinMode(0, INPUT);    //SD check
  Expander1.pinMode(1, INPUT);    //MIC check
  Expander1.pinMode(2, OUTPUT);   //MIC CTR1
  Expander1.pinMode(3, OUTPUT);   //MIC CTR2
  Expander1.pinMode(4, INPUT);    //AMP check
  Expander1.pinMode(5, OUTPUT);   //AMP CTR1
  Expander1.pinMode(6, OUTPUT);   //AMP CTR2
  Expander1.pinMode(7, OUTPUT);   //AMP CTR3
  Expander1.pinMode(8, INPUT);    //DSP check
  Expander1.pinMode(9, INPUT);    //DSP CTR1  DSP int
  Expander1.pinMode(10, INPUT);   //DSP CTR2  DSP ready
  Expander1.pinMode(11, OUTPUT);  //DSP CTR3  DSP reset
  Expander1.pinMode(12, OUTPUT);  //DSP CTR4  not use
  Expander1.pinMode(13, OUTPUT);  //DSP CTR5  not use
  Expander1.pinMode(14, OUTPUT);  //NOT USE
  Expander1.pinMode(15, OUTPUT);  //NOT USE

  Expander2.pinMode(0, OUTPUT);  //CORE  SPI CS1   DSP CS
  Expander2.pinMode(1, OUTPUT);  //CORE  SPI CS2
  Expander2.pinMode(2, OUTPUT);  //CORE  SPI CS3
  Expander2.pinMode(3, OUTPUT);  //PDM switch CTR    1 to dsp   0 to core
  Expander2.pinMode(4, OUTPUT);  //not use
  Expander2.pinMode(5, OUTPUT);  //not use
  Expander2.pinMode(6, OUTPUT);  //not use
  Expander2.pinMode(7, OUTPUT);  //not use
  Expander2.pinMode(8, OUTPUT);  //not use
  Expander2.pinMode(9, OUTPUT);  //not use
  Expander2.pinMode(10, OUTPUT); //not use
  Expander2.pinMode(11, OUTPUT); //not use
  Expander2.pinMode(12, OUTPUT); //not use
  Expander2.pinMode(13, OUTPUT); //not use
  Expander2.pinMode(14, OUTPUT); //not use
  Expander2.pinMode(15, OUTPUT); //not use

  Expander2.digitalWrite(3, 0);   //set the PDM data direction from MIC to WisCore

  while (Expander1.digitalRead(1) == 0) //Check if the microphone board is connected on the RAK18003
  {
    Serial.println("There is no microphone, please check !");
    delay(500);
  }
}
