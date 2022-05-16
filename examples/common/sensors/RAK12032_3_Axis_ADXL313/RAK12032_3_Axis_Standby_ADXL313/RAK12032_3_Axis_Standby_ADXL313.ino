/**
   @file RAK12032_3_Axis_Standby_ADXL313.ino
   @author rakwireless.com
   @brief Setup standby and only print 3-axis acceleration data when it's awake.
   @version 0.1
   @date 2021-12-25
   @copyright Copyright (c) 2021
**/

#include <Wire.h>
#include <SparkFunADXL313.h> // Click here to get the library: http://librarymanager/All#SparkFun_ADXL313

ADXL313 myAdxl;

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
  Serial.println("Standby mode and measure mode.");

  Wire.begin();

  if(myAdxl.begin() == false) // Begin communication over I2C
  {
    Serial.println("The sensor did not respond. Please check wiring.");
    while(1)
    {
      delay(10);  
    }
  }
  Serial.println("Sensor is connected properly.");
}

void loop()
{
  Serial.println("Entering Measure Mode."); // Enter measure mode
  myAdxl.measureModeOn();

  for (int i = 0 ; i < 100 ; i++)     // Print 100 readings to terminal (this should take about 500ms)
  {
    myAdxl.updateIntSourceStatuses(); // This will update all class intSource.xxxxx variables by reading int source bits.

    if (myAdxl.intSource.dataReady)   // Check data ready interrupt bit
    {
      float xAxis,yAxis,zAxis;
      myAdxl.readAccel();             // Read all 3 axis, they are stored in class variables: myAdxl.x, myAdxl.y and myAdxl.z
      /*
       * ±0.5 g : 1024 LSB/g 
       * ±1 g   : 512 LSB/g 
       * ±2 g   : 256 LSB/g 
       * ±4 g   : 128 LSB/g 
       */
      xAxis = (float)myAdxl.x / 1024;
      yAxis = (float)myAdxl.y / 1024;
      zAxis = (float)myAdxl.z / 1024;
      
      Serial.print("  x: ");
      Serial.print(xAxis);
      Serial.print("[g]  y: ");
      Serial.print(yAxis);
      Serial.print("[g]  z: ");
      Serial.print(zAxis);
      Serial.println("[g]");
    }
    else
    {
      Serial.println("Waiting for data.");
    }
    delay(50);
  }

  Serial.println("Entering Standby Mode.");
  myAdxl.standby();
  delay(5000); // 5 seconds of standby... really saving power during this time (0.1uA)
}
