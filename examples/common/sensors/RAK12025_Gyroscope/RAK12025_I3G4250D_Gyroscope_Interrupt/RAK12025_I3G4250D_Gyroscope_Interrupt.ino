/**
   @file RAK12025_I3G4250D_Gyroscope_Interrupt.ino
   @author rakwireless.com
   @brief Gyroscope interrupt example
   @version 0.1
   @date 2021-9-28
   @copyright Copyright (c) 2020
**/

#include <Wire.h>
#include "I3G4250D.h"	// Click here to get the library: http://librarymanager/All#I3G4250D
// It use WB_IO2 to power up and is conflicting with INT1, so better use in Slot C and Slot D.

#define INT1    WB_IO4  // If is use Slot D, INT1 is connected to WB_IO6.

I3G4250D i3g4240d;
I3G4250D_DataScaled i3g4240d_data = {0};

volatile boolean g_threshold_exceeded = false;

void setup()
{
  uint8_t id = 0;
  uint8_t i2cData = 0;
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  uint8_t error = -1;

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

  Serial.println("Gyroscope example");

  error = i3g4240d.I3G4250D_Init(0xFF,0x00,0x00,0x00,0x00,I3G4250D_SCALE_500);
  if(error != 0)
  {
    Serial.println("init fail");
  }
   i3g4240d.readRegister(0x0F,&id,1);
   Serial.print("Gyroscope Device ID = ");
   Serial.println(id,HEX);

  /* The value of 1 LSB of the threshold corresponds to ~7.5 mdps
   * Set Threshold ~100 dps on X, Y and Z axis
   */
  i3g4240d.I3G4250D_SetTresholds(0x1415,0x1415,0x1415);
  
  i3g4240d.I3G4250D_InterruptCtrl(I3G4250D_INT_CTR_XLI_ON | I3G4250D_INT_CTR_YLI_ON | I3G4250D_INT_CTR_ZLI_ON);
  
  i3g4240d.I3G4250D_Enable_INT1();
  
  pinMode(INT1,INPUT);  // Connect with I3G4250D INT1.
  attachInterrupt( digitalPinToInterrupt(INT1), INT1CallBack, RISING);
}

void loop()
{
  i3g4240d_data = i3g4240d.I3G4250D_GetScaledData();
  Serial.println();
  Serial.print("Gyroscope X(dps) = ");
  Serial.println(i3g4240d_data.x);  
  Serial.print("Gyroscope Y(dps) = ");
  Serial.println(i3g4240d_data.y);  
  Serial.print("Gyroscope Z(dps) = ");
  Serial.println(i3g4240d_data.z); 
  delay(1000);
  if(g_threshold_exceeded == true)
  {
    g_threshold_exceeded = false;
    //attachInterrupt( digitalPinToInterrupt(INT1), INT1CallBack, RISING);
    printEventSrc();
  }
}

void INT1CallBack()
{
  g_threshold_exceeded = true;
  //detachInterrupt( digitalPinToInterrupt(INT1) ); // Disable INT.
}

void printEventSrc(void)
{
  uint8_t eventSrc = i3g4240d.I3G4250D_GetInterruptSrc();
  if( eventSrc & I3G4250D_INT_CTR_XLI_ON)
  {
    Serial.println("Interrupt generation on X LOW event.");
  }
  if( eventSrc & I3G4250D_INT_CTR_XHI_ON)
  {
    Serial.println("Interrupt generation on X HIGH event.");
  }
  if( eventSrc & I3G4250D_INT_CTR_YLI_ON)
  {
    Serial.println("Interrupt generation on Y LOW event.");
  }
  if( eventSrc & I3G4250D_INT_CTR_YHI_ON)
  {
    Serial.println("Interrupt generation on Y HIGH event.");
  }
  if( eventSrc & I3G4250D_INT_CTR_ZLI_ON)
  {
    Serial.println("Interrupt generation on Z LOW event.");
  }
  if( eventSrc & I3G4250D_INT_CTR_ZHI_ON)
  {
    Serial.println("Interrupt generation on Z HIGH event.");
  }
}