/**
   @file RAK12016_Flex_FSL0095103ST.ino
   @author rakwireless.com
   @brief use Flex sensor read sensor datas example.
   @version 0.1
   @date 2021-07-26
   @copyright Copyright (c) 2021
*/

#include <Wire.h>
#include "ADC121C021.h"    // Click here to get the library: http://librarymanager/All#RAKwireless_MQx_library
#define  ReferenceVoltage  3.3
#define  FLEX_ADDRESS      0x54    //the device i2c address

// Measure the voltage at 3.3V and the actual resistance of your// 10k resistor, and enter them below:
const float VCC = 3.3;                       //Module supply voltage, ADC reference voltage is 3.3V
const float R_DIV = 10000.0;                 // The partial voltage resistance is 10KΩ
// Upload the code, then try to adjust these values to more// accurately calculate bend degree.
const float STRAIGHT_RESISTANCE = 11563.0;   // The flat Resistance
const float BEND_RESISTANCE = 21117.0;      //Resistance at 90 degree bending
ADC121C021 Flex;
void setup() {
  // put your setup code here, to run once:
  /* WisBLOCK 12016 Power On*/
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  delay(300);
  /* WisBLOCK 12016 Power On*/
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

  //********ADC121C021 ADC convert init ********************************
  while (!(Flex.begin(FLEX_ADDRESS, Wire)))
  {
    Serial.println("please check device!!!");
    delay(200);
  }
  
  Flex.setVoltageResolution(ReferenceVoltage);
  float flexRef = Flex.getVoltageResolution();
  uint8_t flexflag = abs(flexRef - ReferenceVoltage);
  while(flexflag)
  {
    Serial.println("please check the Reference Voltage!!!");
    delay(200);
  }
  Serial.println("RAK12016 test Example");
}

/*
 * Step 1: Connect the Flex sensor to the sensor module, supply power VCC (3.3V) to the module
 * Step 2: Open serial debugging assistant (baud rate 115200), you can see the current printed resistance value. Record the resistance value (unit Ω) at the level and maximum bending to be measured respectively.
 * Step 3: Rerunned the test routine after correcting the values of STRAIGHT_RESISTANCE and BEND_RESISTANCE both horizontal resistance variables
 * Step 4: The Flex sensor can only be bent to one side of the print. Bending the sensor in the other direction does not produce any reliable data and may damage the sensor,Also be careful not to bend the sensors near the bottom, as they have the potential to kink and fail
 */
void loop() {
  // put your main code here, to run repeatedly:
  float flexVoltage = Flex.getSensorVoltage();
  float flexR = R_DIV * (VCC / flexVoltage - 1.0);
  Serial.println("Voltage: " + String(flexVoltage) + " V");
  Serial.println("Resistance: " + String(flexR) + " ohms");
  float angle = map(flexR, STRAIGHT_RESISTANCE, BEND_RESISTANCE, 0, 90.0);
  Serial.println("Bend: " + String(angle) + " degrees");
  Serial.println();
  delay(500);
}
