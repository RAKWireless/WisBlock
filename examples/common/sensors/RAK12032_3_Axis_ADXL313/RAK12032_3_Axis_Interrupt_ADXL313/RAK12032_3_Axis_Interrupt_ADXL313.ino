/**
   @file RAK12032_3_Axis_Interrupt_ADXL313.ino
   @author rakwireless.com
   @brief Setup auto sleep and then only print 3-axis acceleration data when it's awake.
   @version 0.1
   @date 2021-12-25
   @copyright Copyright (c) 2021
**/

#include <Wire.h>
#include <SparkFunADXL313.h> //Click here to get the library: http://librarymanager/All#SparkFun_ADXL313

#define INT_PIN  WB_IO1 // Slot A
//#define INT_PIN  WB_IO3 // Slot C
//#define INT_PIN  WB_IO5 // Slot D

ADXL313 myAdxl;

bool g_awake = true;           // Global variable to keep track.
bool g_interruptFlag = false;  // Global variabl to keep track of new interrupts. Only ever set true by ISR.

void setup()
{ 
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
  
  pinMode(INT_PIN, INPUT); // setup for interrupt
  
  Serial.println("Setup Autosleep and then only print values when it's awake.");

  Wire.begin();

  if (myAdxl.begin() == false) //Begin communication over I2C
  {
    Serial.println("The sensor did not respond. Please check wiring.");
    while(1)
    {
      delay(10);  
    }
  }
  Serial.println("Sensor is connected properly.");

  myAdxl.standby(); // Must be in standby before changing settings.
  // This is here just in case we already had sensor powered and/or
  // configured from a previous setup.


  myAdxl.setRange(ADXL313_RANGE_4_G);

  // setup activity sensing options
  myAdxl.setActivityX(false);         // Enable x-axis participation in detecting inactivity
  myAdxl.setActivityY(false);         // Disable y-axis participation in detecting inactivity
  myAdxl.setActivityZ(true);          // Disable z-axis participation in detecting inactivity
  myAdxl.setActivityThreshold(68);    // 0-255 (15.625 mg/LSB)

  //setup inactivity sensing options
  myAdxl.setInactivityX(false);       // Enable x-axis participation in detecting inactivity
  myAdxl.setInactivityY(false);       // Disable y-axis participation in detecting inactivity
  myAdxl.setInactivityZ(true);        // Disable z-axis participation in detecting inactivity
  myAdxl.setInactivityThreshold(68);  // 0-255 (15.625 mg/LSB)
  myAdxl.setTimeInactivity(5);        // 0-255 (1sec/LSB)

  myAdxl.setInterruptMapping(ADXL313_INT_ACTIVITY_BIT, ADXL313_INT1_PIN); // When activity is detected, it will effect the int1 pin on the sensor
  myAdxl.setInterruptMapping(ADXL313_INT_INACTIVITY_BIT, ADXL313_INT1_PIN);
  // myAdxl.setInterruptMapping(ADXL313_INT_DATA_READY_BIT, ADXL313_INT1_PIN);

  // Enable/disable interrupts
  // Note, we set them all here, just in case there were previous settings,
  // that need to be changed for this example to work properly.
  myAdxl.InactivityINT(true); // Ensable the inactivity interrupt
  myAdxl.ActivityINT(true);   // Enable the activity interrupt
  myAdxl.DataReadyINT(false); // Disable dataReady

  myAdxl.autosleepOn();

  myAdxl.measureModeOn();     // Wakes up the sensor from standby and puts it into measurement mode.

  // Print int enable statuses, to verify we're setup correctly.
  Serial.println();
  Serial.print("activity int enable: ");
  Serial.println(myAdxl.isInterruptEnabled(ADXL313_INT_ACTIVITY_BIT));
  Serial.print("inactivity int enable: ");
  Serial.println(myAdxl.isInterruptEnabled(ADXL313_INT_INACTIVITY_BIT));
  Serial.print("dataReady int enable: ");
  Serial.println(myAdxl.isInterruptEnabled(ADXL313_INT_DATA_READY_BIT));
  delay(1000);

  attachInterrupt(digitalPinToInterrupt(INT_PIN), int1_ISR, RISING); // note, the INT output on the ADXL313 is default active HIGH.
}

void loop()
{
  if (g_interruptFlag == true) // Sensor is awake (this variable is only ever set true in int1_ISR)
  {
    // Interrupt has fired
    // Check to see what type of detection it was

    myAdxl.updateIntSourceStatuses(); // This will update all class intSource.xxxxx variables by reading int source bits.
    g_interruptFlag = false;

    if (myAdxl.intSource.activity == true)
    {
      Serial.println("Activity detected.");
      g_awake = true;
    }
    if (myAdxl.intSource.inactivity == true)
    {
      Serial.println("Inactivity detected.");
      g_awake = false;
    }
  }

  if (g_awake == true)
  {
    myAdxl.updateIntSourceStatuses(); // This will update all class intSource.xxxxx variables by reading int source bits.
    if (myAdxl.intSource.dataReady == true)
    {
      float xAxis,yAxis,zAxis;
      myAdxl.readAccel(); // Read all 3 axis, they are stored in class variables: myAdxl.x, myAdxl.y and myAdxl.z
      /*
       * ±0.5 g : 1024 LSB/g 
       * ±1 g   : 512 LSB/g 
       * ±2 g   : 256 LSB/g 
       * ±4 g   : 128 LSB/g 
       */
      xAxis = (float)myAdxl.x / 128;
      yAxis = (float)myAdxl.y / 128;
      zAxis = (float)myAdxl.z / 128;
      
      Serial.print("  x: ");
      Serial.print(xAxis);
      Serial.print("[g]  y: ");
      Serial.print(yAxis);
      Serial.print("[g]  z: ");
      Serial.print(zAxis);
      Serial.println("[g]");
    }
  }
  delay(50);
}

void int1_ISR()
{
  g_interruptFlag = true;
}
