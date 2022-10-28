/**
   @file RAK13010_SDI_12_Concurrent_Logger.ino
   @author rakwireless.com
   @brief  Concurrent Measurements.
           Finding all attached sensors and logging data from them.
           Example waits for each sensor to complete a measurement, 
           this asks all sensors to take measurements concurrently 
           and then waits until each is finished to query for results. 
           This can be much faster than waiting for each sensor when 
           you have multiple sensor attached.
   @version 0.1
   @date 2022-03-11
   @copyright Copyright (c) 2022
**/
#ifdef NRF52_SERIES
#include <Adafruit_TinyUSB.h>
#endif
#include "RAK13010_SDI12.h" // Click to install library: http://librarymanager/All#RAK12033-SDI12

#define DATA_PIN    WB_IO6   // The pin of the SDI-12 data bus.

RAK_SDI12 mySDI12(DATA_PIN);

bool isActive[64] = {0};  // Keeps track of active addresses.
uint8_t waitTime[64] = {0}; // Keeps track of the wait time for each active addresses.
uint32_t millisStarted[64] = {0}; // Keeps track of the time each sensor was started.
uint32_t millisReady[64] = {0}; // Keeps track of the time each sensor will be ready.
uint8_t returnedResults[64] = {0};  // Keeps track of the number of results expected.
uint8_t numSensors = 0;

/**
 * @brief converts allowable address characters ('0'-'9', 'a'-'z', 'A'-'Z') to a
 *        decimal number between 0 and 61 (inclusive) to cover the 62 possible
 *        addresses.
 */
byte charToDec(char i) 
{
  if ((i >= '0') && (i <= '9')) 
    return i - '0';
  if ((i >= 'a') && (i <= 'z')) 
    return i - 'a' + 10;
  if ((i >= 'A') && (i <= 'Z'))
    return i - 'A' + 36;
  else
    return i;
}

/**
 * @brief maps a decimal number between 0 and 61 (inclusive) to allowable
 *        address characters '0'-'9', 'a'-'z', 'A'-'Z',
 */
char decToChar(byte i) 
{
  if (i < 10) 
    return i + '0';
  if ((i >= 10) && (i < 36)) 
    return i + 'a' - 10;
  if ((i >= 36) && (i <= 62))
    return i + 'A' - 36;
  else
    return i;
}

/**
 * @brief Gets identification information from a sensor, and prints it to the serial port.
 * @param i A character between '0'-'9', 'a'-'z', or 'A'-'Z'.
 */
void printInfo(char i) 
{
  String command = "";
  command += (char)i;
  command += "I!";
  mySDI12.sendCommand(command);
  delay(100);

  String sdiResponse = mySDI12.readStringUntil('\n');
  sdiResponse.trim();
  // allccccccccmmmmmmvvvxxx...xx<CR><LF>
  Serial.print("Address:" + sdiResponse.substring(0, 1));  // Address.
  Serial.println();
  Serial.print("Protocol Version:" + String(sdiResponse.substring(1, 3).toFloat() / 10));  // SDI-12 version number.
  Serial.println();
  Serial.print("Sensor Vendor:" + sdiResponse.substring(3, 11));  // Vendor id.
  Serial.println();
  Serial.print("Sensor Model:" + sdiResponse.substring(11, 17));  // Sensor model.
  Serial.println();
  Serial.print("Sensor Version:" + sdiResponse.substring(17, 20));  // Sensor version.
  Serial.println();
  Serial.print("Sensor ID:" + sdiResponse.substring(20));  // Sensor id.
  Serial.println();
}

bool getResults(char i, int resultsExpected) 
{
  uint8_t resultsReceived = 0;
  uint8_t cmd_number      = 0;
  while (resultsReceived < resultsExpected && cmd_number <= 9) 
  {
    String command = "";
    // In this example we will only take the 'DO' measurement.
    command = "";
    command += i;
    command += "D";
    command += cmd_number;
    command += "!";  // SDI-12 command to get data [address][D][dataOption][!].
    mySDI12.sendCommand(command);

    uint32_t start = millis();
    while (mySDI12.available() < 3 && (millis() - start) < 1500);
    mySDI12.read();           // Ignore the repeated SDI12 address.
    char c = mySDI12.peek();  // Check if there's a '+' and toss if so.
    if (c == '+')
      mySDI12.read();

    while (mySDI12.available()) 
    {
      char c = mySDI12.peek();
      if (c == '-' || (c >= '0' && c <= '9') || c == '.') 
      {
        float result = mySDI12.parseFloat(SKIP_NONE);
        Serial.print(String(result, 10));
        if (result != -9999) { resultsReceived++; }
      } 
      else if (c == '+') 
      {
        mySDI12.read();
        Serial.print(", ");
      } 
      else 
      {
        mySDI12.read();
      }
      delay(10);  // 1 character ~ 7.5ms.
    }
    if (resultsReceived < resultsExpected) 
    { 
      Serial.print(", "); 
    }
    cmd_number++;
  }
  mySDI12.clearBuffer();

  return resultsReceived == resultsExpected;
}

int startConcurrentMeasurement(char i, String meas_type = "") 
{
  mySDI12.clearBuffer();
  String command = "";
  command += i;
  command += "C";
  command += meas_type;
  command += "!";  // SDI-12 concurrent measurement command format  [address]['C'][!].
  mySDI12.sendCommand(command);
  delay(30);

  String sdiResponse = mySDI12.readStringUntil('\n');
  sdiResponse.trim();

  uint8_t wait = sdiResponse.substring(1, 4).toInt(); // Find out how long we have to wait (in seconds).

  int numResults = sdiResponse.substring(4).toInt();  // Set up the number of results to expect.

  uint8_t sensorNum   = charToDec(i);  // e.g. convert '0' to 0, 'a' to 10, 'Z' to 61.
  waitTime[sensorNum] = wait;
  millisStarted[sensorNum] = millis();
  if (wait == 0) 
  {
    millisReady[sensorNum] = millis();
  } 
  else 
  {
    millisReady[sensorNum] = millis() + wait * 1000;
  }
  returnedResults[sensorNum] = numResults;

  return numResults;
}

/**
 * @brief This checks for activity at a particular address
 * @param expects a char, '0'-'9', 'a'-'z', or 'A'-'Z'.
 */
boolean checkActive(char i) 
{
  String myCommand = "";
  myCommand        = "";
  myCommand += (char)i;  // Sends basic 'acknowledge' command [address][!]
  myCommand += "!";

  for (int j = 0; j < 3; j++) 
  {
    mySDI12.sendCommand(myCommand);
    delay(100);
    if (mySDI12.available()) 
    {
      mySDI12.clearBuffer();
      return true;
    }
  }
  mySDI12.clearBuffer();
  return false;
}

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

  Serial.println("Opening SDI-12 bus.");
  mySDI12.begin();
  delay(500);

  // Quickly Scan the Address Space
  Serial.println("Scanning all addresses, please wait.");

  for (byte i = 0; i < 62; i++) 
  {
    char addr = decToChar(i);
    if (checkActive(addr)) 
    {
      numSensors++;
      isActive[i] = 1;
      printInfo(addr);
      Serial.println();
    }
  }
  Serial.print("Total number of sensors found:  ");
  Serial.println(numSensors);

  if (numSensors == 0) 
  {
    Serial.println( "No sensors found, please check connections and restart the Arduino.");
    while (true) 
    { 
      delay(10); // Do nothing forever.
    }  
  }
}

void loop() 
{
  for (byte i = 0; i < 62; i++) // Start all sensors measuring concurrently.
  {
    char addr = decToChar(i);
    if (isActive[i])
      startConcurrentMeasurement(addr);
  }

  uint8_t numReadingsRecorded = 0;
  while (numReadingsRecorded < numSensors) 
  {
    for (byte i = 0; i < 62; i++) 
    {
      char addr = decToChar(i);
      if (isActive[i]) 
      {
        if (millis() > millisReady[i]) 
        {
          if (returnedResults[i] > 0) 
          {
            Serial.println("Address:" + String(addr));
            Serial.println("Time Elapsed:" + String(millis() / 1000));
            Serial.print("Date:");
            getResults(addr, returnedResults[i]);
            Serial.println();
            Serial.println();
          }
          numReadingsRecorded++;
        }
      }
    }
  }

  delay(10000);  // Wait ten seconds between measurement attempts.
}
