/**
 * @file RAK12001_FingerPrint.ino
 * @author rakwireless.com
 * @brief finger print example
 * @version 0.1
 * @date 2021-11-10
 * @copyright Copyright (c) 2021
 */
#ifdef NRF52_SERIES
#include <Adafruit_TinyUSB.h>
#endif

#include "R30X_FPS.h"     // Click to install library: http://librarymanager/All#R30X-Fingerprint-Sensor-Library

//defines
#define R307_TOUCH   WB_IO1   //Finger touch induction signal output, low level output when there are fingers
//add your fingerprint scanner's password and device address here
#define R307_PASSWORD  0x00000000    //default password is 0x00000000 
#define R307_ADDRESS   0xFFFFFFFF    //default address is 0xFFFFFFFF
//initialize the object with the correct password and address
//if you want to use the deafault values, pass nothing
R30X_FPS r307_fps = R30X_FPS (&Serial1, R307_PASSWORD, R307_ADDRESS); //custom password and address

uint8_t enrollFinger(uint16_t location);
uint8_t finger_detect(void);
void print_help(void);

void setup() 
{
  //not all boards support all baud rates
  //check your board's documentation for more info
  Serial.begin(115200);
  time_t timeout = millis();
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
  pinMode(WB_IO2,OUTPUT); 
  digitalWrite(WB_IO2,HIGH);  //power on for Finger detection power
  r307_fps.begin(57600);
  pinMode(R307_TOUCH,INPUT_PULLUP); 
  Serial.println();
  Serial.println("R30X Fingerprint Example Sketch");
  Serial.println("===============================");
  Serial.println("All commands and parameters must be separated by single whitespace.");
  Serial.println();

  //r307_fps.portControl(0);
  //you need to verify the password before you can do anything else
  Serial.print(F("Verifying password 0x"));
  Serial.println(R307_PASSWORD, HEX);
  uint8_t response = r307_fps.verifyPassword(R307_PASSWORD);
  if(response == 0) 
  {
    #if !defined(FPS_DEBUG)
      Serial.println(F("Successful\n"));
    #else
      Serial.println();
    #endif
  }
  else 
  {
    Serial.println(F("Failed. Check your password. Otherwise try with default one.\n"));
  }
   print_help();  
   
  //this is optional
  // Serial.println(F("Setting new address.."));
  // response = r307_fps.setAddress(0xFFFFFFFF);
  // Serial.println();
}
//infinite loop
void loop() 
{
  uint8_t response = 0;
  String inputString = "";
  String commandString = "";
  String firstParam = "";
  String secondParam = "";
  String thirdParam = "";
  
  //send commands and parameters for each operation
  //items are separated by single whitespace
  //you can send up to 3 parameters
  if(Serial.available()) //monitor the serial interface
  {  
    inputString = Serial.readString();  //read the contents of serial buffer as string
    Serial.print("input Command : ");
    Serial.println(inputString);
  
    //-------------------------------------------------------------------------//
    uint8_t posCount = 0;
    int indexOfSpace = 0;

    while(inputString.indexOf(" ") != -1) 
    { //loop until all whitespace chars are found
      indexOfSpace = inputString.indexOf(" ");  //get the position of first whitespace
      if(indexOfSpace != -1) 
      {  //if a whitespace is found
        if(posCount == 0) //the first one will be command string
          commandString = inputString.substring(0, indexOfSpace); //end char is exclusive
        if(posCount == 1) //second will be second param
          firstParam = inputString.substring(0, indexOfSpace);
        if(posCount == 2) //and so on
          secondParam = inputString.substring(0, indexOfSpace);
        else if(posCount == 3)
          thirdParam = inputString.substring(0, indexOfSpace);
        inputString = inputString.substring(indexOfSpace+1);  //trim the input string
        posCount++;
      }
    }

    //saves the last part of the string is no more whitespace is found
    if(posCount == 0) //if there's just the command
      commandString = inputString;
    if(posCount == 1)
      firstParam = inputString;
    if(posCount == 2)
      secondParam = inputString;
    if(posCount == 3)
      thirdParam = inputString;

    //-------------------------------------------------------------------------//
    //separate and print the received command and parameters
    Serial.print("Command string = ");
    Serial.println(commandString);
    
    if(firstParam != "") 
    {
      Serial.print(F("First param = "));
      Serial.println(firstParam);
    }
    if(secondParam != "") 
    {
      Serial.print(F("Second param = "));
      Serial.println(secondParam);
    }
    if(thirdParam != "") 
    {
      Serial.print(F("Third param = "));
      Serial.println(thirdParam);
    }
    
    Serial.println();
    //-------------------------------------------------------------------------//
    //deletes all the templates in the library
    //this command has no parameters
    //eg. clrlib
    if(commandString == "clrlib") 
    {
      response = r307_fps.clearLibrary();
    }
    //-------------------------------------------------------------------------//
    //get templates count
    //eg. tmpcnt
    else if(commandString == "tmpcnt") 
    {
      Serial.println(F("Reading templates count.."));
      response = r307_fps.getTemplateCount();
    }
    //-------------------------------------------------------------------------//
    //read system parameters
    //eg. readsys
    else if(commandString == "readsys") 
    {
      response = r307_fps.readSysPara();
    }
    //-------------------------------------------------------------------------//
    //set data length
    //this command has a single parameter
    //value should be 32, 64, 128 or 256
    //eg. setdatlen 256
    else if(commandString == "setdatlen") 
    {
      uint16_t length = firstParam.toInt();
      response = r307_fps.setDataLength(length);
    }
    //-------------------------------------------------------------------------//
    //capture and range search library
    //this command has three parameters
    //eg. capranser 3000 1 10
    else if(commandString == "capranser") 
    {
      uint16_t timeOut = firstParam.toInt();  //first parameter in milliseconds
      uint16_t startLocation = secondParam.toInt(); //second parameter
      uint16_t count = thirdParam.toInt();  //third parameter
      Serial.println(F("Capture and range search fingerprint.."));
      delay(1000);
      Serial.println(F("Put your finger on the sensor.."));
      delay(3000);
      response = r307_fps.captureAndRangeSearch(timeOut, startLocation, count);
    }
    //-------------------------------------------------------------------------//
    //capture and full search library
    //eg. capfulser
    else if(commandString == "capfulser") 
    {
      Serial.println(F("Capture and full search fingerprint.."));
      delay(1000);
      Serial.println(F("Put your finger on the sensor.."));
      delay(3000);
      response = r307_fps.captureAndFullSearch();
    }
    //-------------------------------------------------------------------------//
    //enroll a new fingerprint
    //you need to scan the finger twice
    //follow the on-screen instructions
    //eg. enroll
    else if(commandString == "enroll") 
    {
      uint16_t location = firstParam.toInt(); //converts String object to int
      enrollFinger(location);
    }
    //-------------------------------------------------------------------------//
    //verify 4 byte password
    //password should be sent as hex string
    //eg. verpwd FF16FF16
    else if(commandString == "verpwd") 
    {
      const char* hexString = firstParam.c_str(); //convert String object to C-style string
      uint32_t password = strtoul(hexString, NULL, 16);  //convert hex formatted C-style string to int value
      response = r307_fps.verifyPassword(password);
    }
    //-------------------------------------------------------------------------//
    //set 4 byte password sent in hex format
    //password should be sent as hex string
    //eg. setpwd FF16FF16
    else if(commandString == "setpwd") 
    {
      const char* hexString = firstParam.c_str(); //convert String object to C-style string
      uint32_t password = strtoul(hexString, NULL, 16);  //convert hex formatted C-style string to int value
      response = r307_fps.setPassword(password);
    }
    //-------------------------------------------------------------------------//
    //set 4 byte address sent in hex format
    //address should be sent as hex string
    //eg. setaddr FF16FF16
    else if(commandString == "setaddr") 
    {
      const char *hexString = firstParam.c_str(); //convert String object to C-style string
      uint32_t address = strtoul(hexString, NULL, 16);  //convert hex formatted C-style string to int value
      response = r307_fps.setAddress(address);
    }
    //-------------------------------------------------------------------------//
    //set baudrate
    //baudrate must be integer multiple of 96000. max is 115200
    //eg. setbaud 115200
    else if(commandString == "setbaud") 
    {
      uint32_t baudrate = firstParam.toInt();
      response = r307_fps.setBaudrate(baudrate);
    }
    //-------------------------------------------------------------------------//
    //set baudrate
    //baudrate must be integer multiple of 96000. max is 115200
    //eg. setbaud 115200
    else if(commandString == "reinitprt") 
    {
      uint32_t baudrate = firstParam.toInt();
      r307_fps.reinitializePort(baudrate);
      Serial.println(F("No change in device configuration."));
    }
    //-------------------------------------------------------------------------//
    //set security level
    //security level value must be 1-5
    //deafault is usually 2
    //eg. setseclvl 4
    else if(commandString == "setseclvl") 
    {
      uint8_t level = firstParam.toInt();
      response = r307_fps.setSecurityLevel(level);
    }
    //-------------------------------------------------------------------------//
    //scan finger image and save to image buffer
    //eg. genimg
    else if(commandString == "genimg") 
    {      
      time_t timeout = millis();
      while(((millis() - timeout) < 5000)&&(digitalRead(R307_TOUCH) != 0))
      {
        Serial.println("Please put your finger on the sensor.");
        delay(200);
      }
      response = r307_fps.generateImage();           
    }
    //-------------------------------------------------------------------------//
    //generate character file from image
    //buffer Id should be 1 or 2
    //eg. genchar 1
    else if(commandString == "genchar") 
    {
      uint8_t bufferId = firstParam.toInt();
      response = r307_fps.generateCharacter(bufferId);
    }
    //-------------------------------------------------------------------------//
    //generate template from char buffers
    //template is the digital format of a fingerprint
    //generated template will be available on both buffers 1 and 2
    //eg. gentmp
    else if(commandString == "gentmp") 
    {
      response = r307_fps.generateTemplate();
    }
    //-------------------------------------------------------------------------//
    //save template on buffer to library
    //buffer ID should be 1 or 2
    //location should be #1 - #10000 (don't send the "#" with command)
    //eg. savtmp 1 32
    else if(commandString == "savtmp") 
    {
      uint8_t bufferId = firstParam.toInt();
      uint16_t location = secondParam.toInt();
      response = r307_fps.saveTemplate(bufferId, location);
    }
    //-------------------------------------------------------------------------//
    //load template from library to buffer 1 or 2
    //buffer ID should be 1 or 2
    //location should be #1 - #10000 (don't send the "#" with command)
    //eg. lodtmp 1 32
    else if(commandString == "lodtmp") 
    {
      uint8_t bufferId = firstParam.toInt();
      uint16_t location = secondParam.toInt();
      response = r307_fps.loadTemplate(bufferId, location);
    }
    //-------------------------------------------------------------------------//
    //delete one or more templates from library
    //to delete a single template, simply send 1 as quantity or count
    //eg. deltmp 5 1
    else if(commandString == "deltmp") 
    {
      uint16_t startLocation = firstParam.toInt();  //start location in library
      uint16_t count = secondParam.toInt(); //quantity to delete
      response = r307_fps.deleteTemplate(startLocation, count);
    }
    //-------------------------------------------------------------------------//
    //precisely match templates on the buffers 1 and 2
    //returns match score (matchScore)
    //eg. mattmp
    else if(commandString == "mattmp") 
    {
      response = r307_fps.matchTemplates();
    }
    //-------------------------------------------------------------------------//
    //search the library for content on the buffer
    //buffer ID should be 1 or 2
    //start location cane be #1 to #1000 (don't send the "#" with command)
    //eg. serlib 1 10 50
    else if(commandString == "serlib") 
    {
      uint8_t bufferId = firstParam.toInt();
      uint16_t startLocation = secondParam.toInt();
      uint16_t count = thirdParam.toInt();
      response = r307_fps.searchLibrary(bufferId, startLocation, count);
    }
    //-------------------------------------------------------------------------//
    //print command help information
    else if(commandString == "help")
    {
       print_help();  
    }
    //-------------------------------------------------------------------------//
    //unknown command
    else 
    {
      Serial.print(F("Invalid command : "));
      Serial.println(commandString);
    }

    if (response)  
    {
      Serial.print("response == ");
      Serial.println(response);
    }   
    Serial.println("\n.......END OF OPERATION.......\n");
    delay(2000);
  }
 
  finger_detect();
}
uint8_t finger_detect(void)
{
  //if power on 3.3V, when sensor dected Finger the Touch pin output LOW
  if(digitalRead(R307_TOUCH)==LOW)
  {
    delay(5);
    if(digitalRead(R307_TOUCH)==LOW)
    {
      Serial.println("Finger detected");
      return 1;
    }  
  }
  return 0;
}
//========================================================================//
//this implements the fingerprint enrolling process
//simply send the location of where you want to save the new fingerprint.
//the location can be from #1 to #1000
//the library location actually starts at 0, but I have made it to 1 to avoid confusion
//therefore a 1 will be substracted from your location automatically
//The finger needs to be scanend twice at steps #1 and #2
uint8_t enrollFinger(uint16_t location) 
{
  //enroll new fingerprint
  Serial.println("=========================");
  Serial.println("Enrolling New Fingerprint");
  Serial.println("=========================");

  if((location > 1000) || (location < 1)) //if not in range (1-1000)
  { 
    Serial.println();
    Serial.println("Enrolling failed.");
    Serial.println("Bad location.");
    Serial.print("location = #");
    Serial.println(location);
    Serial.println("Please try again.");
    return 1;
  }
  delay(4000);
  Serial.println();
  Serial.println("Scan #1: Please put your finger on the sensor.");
  Serial.println();
  delay(5000);

  uint8_t response = r307_fps.generateImage(); //scan the finger

  if(response != 0) 
  {
    Serial.println("Scan #1: ERROR - Scanning failed. Please try again.");
  }
  else 
  {
    Serial.println("Scan #1: Scanning success.");
    Serial.println();
    delay(2000);
    response = r307_fps.generateCharacter(1);  //generate the character file from image and save to buffer 1

    if(response != 0) 
    {
      Serial.println("Scan #1: ERROR - Template generation failed. Please try again.");
    }
    else 
    {
      Serial.println();
      Serial.println("Scan #1: Template generation success.");
      delay(2000);

      Serial.println("Scan #2: Please put your finger on the sensor.");
      delay(5000);

      Serial.println();
      response = r307_fps.generateImage(); //scan the finger for second time

      if(response != 0) 
      {
        Serial.println("Scan #2: ERROR - Scanning failed. Please try again.");
      }
      else 
      {
        Serial.println();
        Serial.println("Scan #2: Scanning success.");
        delay(2000);
        response = r307_fps.generateCharacter(2);  //generate the character file from image and save to buffer 2

        if(response != 0) 
        {
          Serial.println("Scan #2: Template generation failed. Please try again.");
        }
        else 
        {
          Serial.println();
          response = r307_fps.generateTemplate();  //combine the two buffers and generate a template
          delay(2000);
          if(response == 0) 
          {
            Serial.println();
            response = r307_fps.saveTemplate(1, location); //save the template to the specified location in library
            delay(2000);
            if(response == 0) 
            {
              Serial.print("-- Fingerprint enrolled at ID #");
              Serial.print(location);
              Serial.println(" successfully --");
            }
          }
          else if(response == FPS_RESP_ENROLLMISMATCH) 
          {
            Serial.println("ERROR : Fingerprints do not belong to same finger. Please try again.");
          }
        }
      }
    }
  }
  Serial.println();
  return 1;
}
void print_help(void)
{
  Serial.println(F("")); 
  Serial.println(F("----- COMMANDS -----"));
  Serial.println(F("help - command help information"));
  Serial.println(F("clrlib - clear library"));
  Serial.println(F("tmpcnt - get templates count"));
  Serial.println(F("readsys - read system parameters"));
  Serial.println(F("setdatlen <data length> - set data length"));
  Serial.println(F("capranser <timeout> <start location> <quantity> - capture and range search library for fingerprint"));
  Serial.println(F("capfulser - capture and full search the library for fingerprint"));
  Serial.println(F("enroll <location> - enroll new fingerprint"));
  Serial.println(F("verpwd <password> - verify 4 byte device password"));
  Serial.println(F("setpwd <password> - set new 4 byte device password"));
  Serial.println(F("setaddr <address> - set new 4 byte device address"));
  Serial.println(F("setbaud <baudrate> - set the baudrate"));
  Serial.println(F("reinitprt <baudrate> - reinitialize the port without changing device configuration"));
  Serial.println(F("setseclvl <level> - set security level"));
  Serial.println(F("genimg - generate image"));
  Serial.println(F("genchar <buffer id> - generate character file from image"));
  Serial.println(F("gentmp - generate template from character buffers"));
  Serial.println(F("savtmp <buffer id> <location> - save template to library from buffer"));
  Serial.println(F("lodtmp <buffer id> <location> - load template from library to buffer"));
  Serial.println(F("deltmp <start location> <quantity> - delete one or more templates from library"));
  Serial.println(F("mattmp - precisely match two templates available on buffers"));
  Serial.println(F("serlib <buffer id> <start location> <quantity> - search library for content on the buffer"));
  Serial.println(F(""));  
}
