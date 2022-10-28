/*
   @file Audio_Alarm_RAK18003.ino
   @author rakwireless.com
   @brief When the microphone detects that the PDM value of the sound exceeds the set threshold,
          the serial port will print out an alarm message, and the LED will light up.
   @note This example need use the RAK18003 module.
   @version 0.1
   @date 2021-01-12
   @copyright Copyright (c) 2020
*/
#include "audio.h" // Click here to install the library: http://librarymanager/All#RAKwireless-Audio
#include <WiFi.h>

TPT29555   Expander1(0x23);
TPT29555   Expander2(0x25);

int channels = 1;
// default PCM output frequency
static const int frequency = 16000;

// buffer to read samples into, each sample is 16-bits
short sampleBuffer[BUFFER_SIZE];
int audio_threshold = 800; //This value determines the threshold for triggering the warning.
int g_alarm = 0;
int aver = 0;
void RAK18003Init(void);

void setup()
{
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
  // initialize PDM with:
  // - one channel (mono mode)
  // - a 16 kHz sample rate
  // default PCM output frequency
  if (!PDM.begin(channels, frequency)) {
    Serial.println("Failed to start PDM!");
    while (1) yield();
  }
  delay(500);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, LOW);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_BLUE, LOW);
  Serial.println("noise alarm example");
}

int abs_int(short data)
{
  if (data > 0) return data;
  else return (0 - data);
}

void loop()
{
  // Read data from microphone
  int sampleRead = PDM.read(sampleBuffer, sizeof(sampleBuffer));
  sampleRead = sampleRead / 2;
  // wait for samples to be read
  if (sampleRead > 0) {
    uint32_t sum = 0;
    // print samples to the serial monitor or plotter
    for (int i = 0; i < BUFFER_SIZE; i++) {
      sum = sum + abs(sampleBuffer[i]);
    }
    aver = sum / BUFFER_SIZE;
  }

  if (aver > audio_threshold)
  {
    g_alarm++;
    Serial.printf("Alarm %d\r\n", g_alarm);
    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    delay(1000);
    /*You can add your warning handling tasks here.*/
  }
  else
  {
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_GREEN, LOW);
  }
}
void RAK18003Init(void)
{
  while (!Expander1.begin())
  {
    Serial.println("Did not find RAK18003 IO Expander Chip1, please check !");
    delay(200);
  }

  while (!Expander2.begin())
  {
    Serial.println("Did not find RAK18003 IO Expander Chip2, please check !");
    delay(200);
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
