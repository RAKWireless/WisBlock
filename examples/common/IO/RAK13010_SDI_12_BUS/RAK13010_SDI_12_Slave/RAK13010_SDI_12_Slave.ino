/**
   @file RAK13010_SDI_12_Slave.ino
   @author rakwireless.com
   @brief  Example sketch demonstrating how to implement an arduino as a slave on an SDI-12 bus.
           This may be used, for example, as a middleman between an I2C sensor and an SDI-12 datalogger.
   @version 0.1
   @date 2022-03-11
   @copyright Copyright (c) 2022
**/
#ifdef NRF52_SERIES
#include <Adafruit_TinyUSB.h>
#endif
#include "RAK13010_SDI12.h" // Click to install library: http://librarymanager/All#RAK12033-SDI12

#define DATA_PIN    WB_IO6   // The pin of the SDI-12 data bus.

char sensorAddress = 'A';    // A character between '0'-'9', 'a'-'z', or 'A'-'Z'.
int  state         = 0;

#define WAIT                  0
#define INITIATE_CONCURRENT   1
#define INITIATE_MEASUREMENT  2

RAK_SDI12 slaveSDI12(DATA_PIN);

void pollSensor(float* measurementValues)
{
  measurementValues[0] =  1.111111;
  measurementValues[1] = -2.222222;
  measurementValues[2] =  3.333333;
  measurementValues[3] = -4.444444;
  measurementValues[4] =  5.555555;
  measurementValues[5] = -6.666666;
  measurementValues[6] =  7.777777;
  measurementValues[7] = -8.888888;
  measurementValues[8] = -9.999999;
}

void parseSdi12Cmd(String command, String* dValues) 
{
  if (command.charAt(0) != sensorAddress && command.charAt(0) != '?') 
  { 
    return; 
  }

  /*
   * If execution reaches this point, the slave should respond with something in
   * the form:   <address><responseStr><Carriage Return><Line Feed>
   * The following if-switch-case block determines what to put into <responseStr>,
   * and the full response will be constructed afterward. For '?!' (address query)
   * or 'a!' (acknowledge active) commands, responseStr is blank so section is skipped
   */
  String responseStr = "";
  if (command.length() > 1) 
  {
    switch (command.charAt(1)) 
    {
      case 'I':
        /*
         * Identify command
         * Slave should respond with ID message: 2-char SDI-12 version + 8-char
         * company name + 6-char sensor model + 3-char sensor version + 0-13 char S/N
         */
        responseStr = "13RAK130100000011.0001";  // Substitute proper ID String here
        break;
      case 'C':
        /*
         * Initiate concurrent measurement command
         * Slave should immediately respond with: "tttnn":
         *    3-digit (seconds until measurement is available) +
         *    2-digit (number of values that will be available)
         * Slave should also start a measurment and relinquish control of the data line   
         */
        responseStr ="02109";  
        /*
         * 9 values ready in 21 sec; Substitue sensor-specific values here
         * It is not preferred for the actual measurement to occur in this subfunction,
         * because doing to would hold the main program hostage until the measurement
         * is complete.  Instead, we'll just set a flag and handle the measurement
         * elsewhere.
         */
        state = INITIATE_CONCURRENT;
        break;
        /*
         * NOTE: "aC1...9!" commands may be added by duplicating this case and adding
         *       additional states to the state flag
         */
      case 'M':
        /*
         * Initiate measurement command
         * Slave should immediately respond with: "tttnn":
         *    3-digit (seconds until measurement is available) +
         *    1-digit (number of values that will be available)
         * Slave should also start a measurment but may keep control of the data line
         * until advertised time elapsed OR measurement is complete and service request sent.
         */
        responseStr = "0219"; 
        /* 
         * 9 values ready in 21 sec; Substitue sensor-specific values here 
         * It is not preferred for the actual measurement to occur in this subfunction,
         * because doing to would hold the main program hostage until the measurement is
         * complete.  Instead, we'll just set a flag and handle the measurement
         * elsewhere. It is preferred though not required that the slave send a service
         * request upon completion of the measurement.  This should be handled in the
         * main loop().
         */
        state = INITIATE_MEASUREMENT;
        break;
        /* 
         * NOTE: "aM1...9!" commands may be added by duplicating this case and adding
         *       additional states to the state flag.
         */
      case 'D':
        /* 
         * Send data command.
         * Slave should respond with a String of values
         * Values to be returned must be split into Strings of 35 characters or fewer
         * (75 or fewer for concurrent).  The number following "D" in the SDI-12 command
         * specifies which String to send
         */
        responseStr = dValues[(int)command.charAt(2) - 48];
        break;
      case 'A':
        /* 
         * Change address command.
         * Slave should respond with blank message (just the [new] address + <CR> + <LF>).
         */
        sensorAddress = command.charAt(2);
        break;
      default:
        responseStr = "UNKN"; // Mostly for debugging; send back UNKN if unexpected command received.
        break;
    }
  }
  // Issue the response speficied in the switch-case structure above.
  slaveSDI12.sendResponse(String(sensorAddress) + responseStr + "\r\n");  
  Serial.println("T: "+ String(sensorAddress) + responseStr);
}

void formatOutputSDI(float* measurementValues, String* dValues, unsigned int maxChar) 
{
  dValues[0] = "";
  int j      = 0;

  // Upper limit on i should be number of elements in measurementValues.
  for (int i = 0; i < 9; i++) 
  {
    /*
     * Read float value "i" as a String with 6 deceimal digits
     * NOTE: SDI-12 specifies max of 7 digits per value; we can only use 6
     *       decimal place precision if integer part is one digit.
     */
    String valStr = String(measurementValues[i], 6);
    // Explictly add implied + sign if non-negative.
    if (valStr.charAt(0) != '-')
    { 
      valStr = '+' + valStr; 
    }
    // Append dValues[j] if it will not exceed 35 (aM!) or 75 (aC!) characters.
    if (dValues[j].length() + valStr.length() < maxChar) 
    {
      dValues[j] += valStr;
    }
    // Start a new dValues "line" if appending would exceed 35/75 characters.
    else 
    {
      dValues[++j] = valStr;
    }
  }

  while (j < 9) 
  { 
    dValues[++j] = "";  // Fill rest of dValues with blank strings.
  }  
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
  
  slaveSDI12.begin();
  delay(500);
  slaveSDI12.forceListen();  // Sets SDIPIN as input to prepare for incoming message.
}

void loop() 
{
  static float measurementValues[9];    // 9 floats to hold simulated sensor data.
  static String dValues[10];            // 10 String objects to hold the responses to aD0!-aD9! commands.
  static String commandReceived = "";   // String object to hold the incoming command.

  /*
   * If a byte is available, an SDI message is queued up. Read in the entire message
   * iteration to a static char buffer; however, the SDI-12 spec requires a precise
   * response time, and this method is invariant to the remaining loop() contents.
   */
  int avail = slaveSDI12.available();
  if (avail < 0) 
  {
    slaveSDI12.clearBuffer(); // Buffer is full; clear.
  }  
  else if (avail > 0) 
  {
    for (int a = 0; a < avail; a++) 
    {
      char charReceived = slaveSDI12.read();
      /*
       * Character '!' indicates the end of an SDI-12 command; if the current
       * character is '!', stop listening and respond to the command
       */
      if (charReceived == '!') 
      {
        Serial.println("R: "+ commandReceived);
        parseSdi12Cmd(commandReceived, dValues);  // Command string is completed; do something with it.
        commandReceived = ""; // Clear command string to reset for next command.
        /*
         * '!' should be the last available character anyway, but exit the "for" loop
         * just in case there are any stray characters.
         */
        slaveSDI12.clearBuffer();
        break;
      }
      /*
       * If the current character is anything but '!', it is part of the command 
       * string.  Append the commandReceived String object.
       */
      else 
      {
        commandReceived += String(charReceived);  // Append command string with new character.
      }
    }
  }

  /*
   *  For aM! and aC! commands, parseSdi12Cmd will modify "state" to indicate that 
   *  a measurement should be taken.
   */
  switch (state) 
  {
    case WAIT: break;
    case INITIATE_CONCURRENT:
      /*
       * Do whatever the sensor is supposed to do here.
       * For this example, we will just create arbitrary "simulated" sensor data.
       * NOTE: Your application might have a different data type (e.g. int) and
       *       number of values to report!
       */
      pollSensor(measurementValues);
      formatOutputSDI(measurementValues, dValues, 75);  // Populate the "dValues" String array with the values in SDI-12 format.
      state = WAIT;
      slaveSDI12.forceListen();  // Sets SDI-12 pin as input to prepare for incoming message AGAIN.
      break;
    case INITIATE_MEASUREMENT:
      /*
       * Do whatever the sensor is supposed to do here. 
       * For this example, we will just create arbitrary "simulated" sensor data.
       * NOTE: Your application might have a different data type (e.g. int) and
       *       number of values to report!
       */
      pollSensor(measurementValues);
      formatOutputSDI(measurementValues, dValues, 35);  // Populate the "dValues" String array with the values in SDI-12 format.
      slaveSDI12.sendResponse(String(sensorAddress) + "\r\n");  // For aM!, Send "service request" (<address><CR><LF>) when data is ready.
      state = WAIT;
      slaveSDI12.forceListen();  // Sets SDI-12 pin as input to prepare for incoming message AGAIN.
      break;
  }
}
