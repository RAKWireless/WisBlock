/**
   @file RAK13010_SDI_12_Interface.ino
   @author rakwireless.com
   @brief  Arduino-based USB dongle translates serial comm from PC to SDI-12 (electrical and timing)
           1.Allows user to communicate to SDI-12 devices from a serial terminal emulator (e.g. PuTTY).
           2.Able to spy on an SDI-12 bus for troubleshooting comm between datalogger and sensors.
           3.Can also be used as a hardware middleman for interfacing software to an SDI-12 sensor. 
             For example, implementing an SDI-12 datalogger in Python on a PC.  Use verbatim mode 
             with feedback off in this case.
   @version 0.1
   @date 2022-03-11
   @copyright Copyright (c) 2022
**/
#ifdef NRF52_SERIES
#include <Adafruit_TinyUSB.h>
#endif

#include "RAK13010_SDI12.h"  // Click to install library: http://librarymanager/All#RAK12033-SDI12

#define HELPTEXT                                                                          \
        "OPTIONS:\r\n"                                                                    \
        "help   : Print this message\r\n"                                                 \
        "mode s : SDI-12 command mode (uppercase and ! automatically corrected) "         \
        "[default]\r\n"                                                                   \
        "mode v : verbatim mode (text will be sent verbatim)\r\n"                         \
        "fb on  : Enable feedback (characters visible while typing) [default]\r\n"        \
        "fb off : Disable feedback (characters not visible while typing; may be desired " \
        "for developers)\r\n"                                                             \
        "(else) : send command to SDI-12 bus"

#define DATA_PIN    WB_IO6   // The pin of the SDI-12 data bus.

RAK_SDI12 mySDI12(DATA_PIN);

void setup() 
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);  // Power the sensors.

  // Initialize Serial for debug output.
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

  mySDI12.begin();  // Initiate serial connection to SDI-12 bus.
  delay(500);
  
  mySDI12.forceListen();
  
  Serial.println(HELPTEXT); // Print help text (may wish to comment out if used for communicating to software).
}

void loop() 
{
  static String  serialMsgStr;
  static boolean serialMsgReady = false;

  static String  sdiMsgStr;
  static boolean sdiMsgReady = false;

  static boolean verbatim = false;
  static boolean feedback = true;


  // READ SERIAL (PC COMMS) DATA
  // If serial data is available, read in a single byte and add it to a String on each iteration.
  if (Serial.available()) 
  {
    char inByte1 = Serial.read();
    if (feedback) 
    { 
      Serial.print(inByte1); 
    }
    if (inByte1 == '\r' || inByte1 == '\n') 
    {
      serialMsgReady = true;
    } 
    else 
    {
      serialMsgStr += inByte1;
    }
  }

  /*
   * @brief READ SDI-12 DATA
   *        If SDI-12 data is available, keep reading until full message consumed
   *        (Normally I would prefer to allow the loop() to keep executing while the string
   *        is being read in--as the serial example above--but SDI-12 depends on very precise
   *        timing, so it is probably best to let it hold up loop() until the string is complete)
   */
  int avail = mySDI12.available();
  if (avail < 0) 
  {
    mySDI12.clearBuffer();  // Buffer is full,clear.
  }  
  else if (avail > 0) 
  {
    for (int a = 0; a < avail; a++) 
    {
      char inByte2 = mySDI12.read();
      if (inByte2 == '\n') 
      {
        sdiMsgReady = true;
      } 
      else if (inByte2 == '!') 
      {
        sdiMsgStr += "!";
        sdiMsgReady = true;
      } 
      else 
      {
        sdiMsgStr += String(inByte2);
      }
    }
  }
  
  if (sdiMsgReady)  // Report completed SDI-12 messages back to serial interface.
  {
    Serial.println(sdiMsgStr);
    sdiMsgReady = false;  // Reset String for next SDI-12 message.
    sdiMsgStr   = "";
  }

  
  if (serialMsgReady) // Send completed Serial message as SDI-12 command.
  {
    Serial.println();
    
    String lowerMsgStr = serialMsgStr;  // Check if the serial message is a known command to the SDI-12 interface program.
    lowerMsgStr.toLowerCase();
    if (lowerMsgStr == "mode v") 
    {
      verbatim = true;
      Serial.println("Verbatim mode; exact text will be sent.  Enter \"mode s\" for "
                     "SDI-12 command mode.");
    } 
    else if (lowerMsgStr == "mode s") 
    {
      verbatim = false;
      Serial.println("SDI-12 command mode; uppercase and ! suffix optional.  Enter "
                     "\"mode v\" for verbatim mode.");
    } 
    else if (lowerMsgStr == "help") 
    {
      Serial.println(HELPTEXT);
    } 
    else if (lowerMsgStr == "fb off") 
    {
      feedback = false;
      Serial.println("Feedback off; typed commands will not be visible.  Enter \"fb "
                     "on\" to enable feedback.");
    } 
    else if (lowerMsgStr == "fb on") 
    {
      feedback = true;
      Serial.println("Feedback on; typed commands will be visible.  Enter \"fb off\" "
                     "to disable feedback.");
    }
    else  // If not a known command to the SDI-12 interface program, send out on SDI-12 data.
    {
      if (verbatim) 
      {
        mySDI12.sendCommand(serialMsgStr);
      } 
      else 
      {
        serialMsgStr.toUpperCase();
        mySDI12.sendCommand(serialMsgStr + "!");
      }
    }
    serialMsgReady = false; // Reset String for next serial message.
    serialMsgStr   = "";
  }
}
