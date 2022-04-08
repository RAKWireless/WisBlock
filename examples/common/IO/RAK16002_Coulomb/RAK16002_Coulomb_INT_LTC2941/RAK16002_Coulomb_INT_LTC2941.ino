/**
   @file RAK16002_Coulomb_INT_LTC2941.ino
   @author rakwireless.com
   @brief  Trigger an interrupt when Charge alert high/low or VBAT alert.
   @version 0.1
   @date 2021-11-08
   @copyright Copyright (c) 2020
**/

#include "LTC2941.h"	//Click here to get the library: http://librarymanager/All#GROVE-Coulomb_Counter

#define INTERRUPT_PIN     WB_IO6  // Interrupt capable Arduino pin.

#define BATTERY_CAPACITY  2200     // unit:mAh  

#define LTC2942_I2C_ALERT_RESPONSE  0x0C

volatile bool g_isr = false;

void setup(void)
{
  //Sensor power switch
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);	

/*
 * The LED_BLUE  lights up to indicate Charge alert high.
 * The LED_GREEN lights up to indicate Charge alert low.
 * All lights up to indicate VBAT alert.
 */
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_BLUE, LOW);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, LOW);

  pinMode(INTERRUPT_PIN, INPUT);
  
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
  
  Serial.println("=====================================");
  Serial.println("RAK16002 Coulomb LTC2941 example");
  Serial.println("=====================================");
  
  Wire.begin();
    
  ltc2941.initialize();
  ltc2941.setBatteryFullMAh( BATTERY_CAPACITY , false);
  ltc2941.setAccumulatedCharge(42352); // Set the current battery level to 1800 mAh. 
  
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), interruptRoutine, FALLING);
  
  
  ltc2941.setBatteryAlert( VBAT_3_0_V );

  ltc2941.setChargeThresholdHigh( 42353 );//CURRENT_CAPACITY
  ltc2941.setChargeThresholdLow( 42351 );

  ltc2941.setAlertConfig( ALERT_MODE );

  /*
  *  The LTC2941 will stop pulling down the AL/CC pin and will not respond 
  *  to further ARA requests until a new alert event occurs.
  */
  Wire.requestFrom(LTC2942_I2C_ALERT_RESPONSE, 1);
  while(Wire.available())
  {
    Wire.read();
  }
}

void loop(void)
{
  float coulomb = 0, mAh = 0, percent = 0;
  uint8_t Status;

  if (g_isr == true)
  {
    g_isr = false; 
    
    ltc2941.setAccumulatedCharge(42352); // Reset the battery level only for periodic presentation interruption.

    Wire.requestFrom((int16_t)0x0C, 1); // Send alert response protocol.
    while(Wire.available())
    {
      Wire.read();
    }

    Status = ltc2941.getStatus();
    if(Status & 0x02)
    {
       Serial.println("VBAT alert interrupt！"); 
       digitalWrite(LED_GREEN, HIGH);
       digitalWrite(LED_BLUE, HIGH);
    }
    if(Status & 0x04)
    {
       Serial.println("Charge alert low interrupt！"); 
       digitalWrite(LED_GREEN, HIGH);
    }
    if(Status & 0x08)
    {
       Serial.println("Charge alert high interrupt！"); 
       digitalWrite(LED_BLUE, HIGH);
    }
  }
  coulomb = ltc2941.getCoulombs();
  mAh = ltc2941.getmAh();
  percent = ltc2941.getPercent(); 

  Serial.print(coulomb);
  Serial.print("C   ");
  Serial.print(mAh);
  Serial.print("mAh   ");
  Serial.print(percent);
  Serial.println("%");
  delay(1000);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_BLUE, LOW);
}

void interruptRoutine()
{
  g_isr = true;
}
