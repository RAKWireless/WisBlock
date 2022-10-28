/**
   @file RAK18003_example.ino
   @author rakwireless.com
   @brief This example shows how to configure the IO signal output of the RAK18003 module
   and use the RAK18003 to read the IO status, and how to detect other modules.
   @version 0.1
   @date 2022-06-10
   @copyright Copyright (c) 2020

*/
#include <Arduino.h>
#include "audio.h" // Click here to install the library: http://librarymanager/All#RAKwireless-Audio

#define    EXPANDER_INT_PIN   WB_IO1

TPT29555   Expander1(0x23);
TPT29555   Expander2(0x25);

volatile uint8_t int_flag = 0;

void RAK18003Init(void);
void RAK18003_IRQ(void);

void setup()
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  delay(500);
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
  Serial.println("This is an RAK18003 example");
  RAK18003Init();
  pinMode(EXPANDER_INT_PIN, INPUT_PULLUP);
  attachInterrupt(EXPANDER_INT_PIN, RAK18003_IRQ, CHANGE);
}

void loop()
{
  if (int_flag == 1)
  {
    uint16_t chip1_io_state = Expander1.readAllPin();
    uint16_t chip2_io_state = Expander2.readAllPin();

    Serial.printf("Expander1 IO state:%04X  Expander2 IO state:%04X\r\n", chip1_io_state, chip2_io_state);
    int_flag = 0;
  }
}
void RAK18003_IRQ(void)
{
  int_flag = 1;
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
  Expander1.pinMode(2, OUTPUT);   //MIC CTR1
  Expander1.pinMode(3, OUTPUT);   //MIC CTR2
  Expander1.pinMode(4, INPUT);    //AMP check
  Expander1.pinMode(5, INPUT);    //AMP CTR1  AMP INT  check
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

  Expander1.digitalWrite(14, 0);    //set chip 1 not use pin output low
  Expander1.digitalWrite(15, 0);    //set chip 1 not use pin output low

  Expander2.pinMode(0, OUTPUT);  //CORE  SPI CS1 for DSPG CS
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

  Expander2.digitalWrite(0, 1);  //set SPI CS1 High
  Expander2.digitalWrite(1, 1);  //set SPI CS2 High
  Expander2.digitalWrite(2, 1);  //set SPI CS3 High

  Expander2.digitalWrite(3, 0);   //set the PDM data direction from MIC to WisCore

  if (Expander1.digitalRead(0) == 1) //Check SD card
  {
    Serial.println("There is no SD card on the RAK18003 board, please check !");
    delay(200);
  }

  if (Expander1.digitalRead(1) == 0) //Check if the microphone board is connected on the RAK18003
  {
    Serial.println("There is no microphone board, please check !");
    delay(200);
  }

  if (Expander1.digitalRead(4) == 0) //Check if the RAK18060 AMP board is connected on the RAK18003
  {
    Serial.println("There is no RAK18060 AMP board, please check !");
    delay(200);
  }

  if (Expander1.digitalRead(8) == 0) //Check if the RAK18080 DSPG board is connected on the RAK18003
  {
    Serial.println("There is no RAK18080 DSPG board, please check !");
    delay(200);
  }

}
