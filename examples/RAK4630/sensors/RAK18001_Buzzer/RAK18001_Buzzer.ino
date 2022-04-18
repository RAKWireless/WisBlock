/**
   @file RAK18001_Buzzer.ino
   @author rakwireless.com
   @brief period buzzer test and play a funny song
   @version 0.1
   @date 2020-12-28
   @copyright Copyright (c) 2020
**/

#include <Arduino.h>
#include <Adafruit_TinyUSB.h>

#define BUZZER_CONTROL  WB_IO1

//This part is the note and rhythm of the song
#define NTC0 -1
#define NTC1 262
#define NTC2 294
#define NTC3 330
#define NTC4 350
#define NTC5 393
#define NTC6 441
#define NTC7 495

#define NTCL1 131
#define NTCL2 147
#define NTCL3 165
#define NTCL4 175
#define NTCL5 196
#define NTCL6 221
#define NTCL7 248

#define NTCH1 525
#define NTCH2 589
#define NTCH3 661
#define NTCH4 700
#define NTCH5 786
#define NTCH6 882
#define NTCH7 990

#define WHOLE 1
#define HALF 0.5
#define QUARTER 0.25
#define EIGHTH 0.125
#define SIXTEENTH 0.0625


int tune[] = //List the frequencies according to the spectrum
{
  NTC5, NTC5, NTC6,
  NTCH1, NTC6, NTC5, NTC6, NTCH1, NTC6, NTC5,
  NTC3, NTC3, NTC3, NTC5,
  NTC6, NTC6, NTC5, NTCH3, NTCH3, NTCH2, NTCH1,
  NTCH2, NTCH1, NTCH2,
  NTCH3, NTCH3, NTCH2, NTCH3, NTCH2, NTCH1, NTCH2, NTCH1, NTC6,

  NTCH2, NTCH2, NTCH2, NTCH1, NTC6, NTC5,
  NTC6, NTC5, NTC5, NTCH1, NTC6, NTC5, NTC1, NTC3,
  NTC2, NTC1, NTC2,
  NTC3, NTC5, NTC5, NTC3, NTCH1, NTC7,
  NTC6, NTC5, NTC6, NTCH1, NTCH2, NTCH3,

  NTCH3, NTCH2, NTCH1, NTC5, NTCH1, NTCH2, NTCH3,

  NTCH2, NTC0, NTCH3, NTCH2,
  NTCH1, NTC0, NTCH2, NTCH1, NTC6, NTC0,

  NTCH2, NTC6, NTCH1, NTCH1, NTCH1, NTC6, NTC5, NTC3,
  NTC5,
  NTC5, NTC6, NTCH1, NTC7, NTC6,
  NTCH3, NTCH3, NTCH3, NTCH3, NTCH2, NTCH2, NTCH1,
  NTC6, NTCH3, NTCH2, NTCH1, NTCH2, NTCH1, NTC6,
  NTCH1,
};

float durt[] = //List the beats according to the notation
{
  HALF, QUARTER, QUARTER,
  WHOLE + HALF, HALF, HALF, QUARTER, QUARTER, HALF, HALF,
  WHOLE + WHOLE + WHOLE, HALF, QUARTER, QUARTER,
  WHOLE + HALF, HALF, HALF, HALF, QUARTER, QUARTER, HALF,
  WHOLE + WHOLE + WHOLE, HALF, HALF,
  HALF, HALF, HALF, QUARTER, QUARTER, HALF, QUARTER, QUARTER, HALF,
  HALF, HALF, HALF, QUARTER, QUARTER, WHOLE + WHOLE,
  HALF, HALF, HALF, HALF, HALF, HALF, HALF, HALF,
  WHOLE + WHOLE + WHOLE, HALF, HALF,


  WHOLE + HALF, HALF, HALF, HALF, HALF, HALF,
  WHOLE + HALF, HALF, WHOLE, HALF, QUARTER, QUARTER,
  WHOLE + HALF, HALF, HALF, HALF, HALF, QUARTER, QUARTER,
  WHOLE + WHOLE + WHOLE, HALF, QUARTER, QUARTER,
  WHOLE, HALF, QUARTER, QUARTER, WHOLE, WHOLE,
  HALF, HALF, HALF, HALF, WHOLE, QUARTER, QUARTER, HALF,
  WHOLE + WHOLE + WHOLE + WHOLE,
  HALF, HALF, HALF, HALF, WHOLE + WHOLE,
  HALF, HALF, HALF, HALF, WHOLE + HALF, QUARTER, QUARTER,
  WHOLE + HALF, HALF, WHOLE, QUARTER, QUARTER, QUARTER, QUARTER, WHOLE + WHOLE + WHOLE + WHOLE,

};

int length = 0;

void setup()
{
  Serial.begin(115200);
  while (!Serial) {};
  pinMode(BUZZER_CONTROL, OUTPUT);
  length = sizeof(tune) / sizeof(tune[0]); //Calculation length
}
void loop()
{
  for (int x = 0; x < length; x++)
  {
    tone(BUZZER_CONTROL, tune[x]);
    delay(500 * durt[x]); //Here it is used to adjust the delay according to the beat. The 500 index can be adjusted by myself. In this music, I find that 500 is more suitable.
    noTone(BUZZER_CONTROL);
  }
  delay(2000);
} 
