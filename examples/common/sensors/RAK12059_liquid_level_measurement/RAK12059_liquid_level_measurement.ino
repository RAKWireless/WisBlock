/**
   @file RAK12059_liquid_level_measurement.ino
   @author rakwireless.com
   @brief This code can be used to obtain the depth of the liquid level
   @version 1.0
   @date 2022-09-21
   @copyright Copyright (c) 2022
*/

#include "ADC_SGM58031.h" //http://librarymanager/All#RAKwireless_ADC_SGM58031_library

//RAK_ADC_SGM58031 sgm58031;
RAK_ADC_SGM58031 sgm58031(SGM58031_SDA_ADDRESS);
// RAK_ADC_SGM58031 sgm58031(Wire);
// RAK_ADC_SGM58031 sgm58031(Wire,SGM58031_DEFAULT_ADDRESS);

#define ALERT_PIN     WB_IO1  //SlotA installation, please do not use it on SLOTB
//#define ALERT_PIN   WB_IO3  //SlotC installation.
//#define ALERT_PIN   WB_IO5  //SlotD installation.


#define EPISON 1e-7

//If you use 8 inches, use this code
#define REF 1500.0
#define RESISTANCE_MAX 1657.0
#define RESISTANCE 1100.0
#define LENGTH 7.0

//If you use 12 inches, use this code
//#define REF 2000.0
//#define RESISTANCE_MAX 2145.0
//#define RESISTANCE 1600.0
//#define LENGTH 11.0

//If you use 24 inches, use this code
//#define REF 4000.0
//#define RESISTANCE_MAX 4157.0
//#define RESISTANCE 3600.0
//#define LENGTH 23.0

bool  interrupt_flag = false;
void setup()
{
  // put your setup code here, to run once:
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_BLUE, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  delay(300);
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
  sgm58031.begin();
  Serial.println("RAK12059 TEST");
  if (sgm58031.getChipID() != DEVICE_ID)
  {
    Serial.println("No CHIP found ... please check your connection");
    while (1)
    {
      delay(100);
    }
  }
  else
  {
    Serial.println("Found SGM58031 Chip");
  }

  pinMode(ALERT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(ALERT_PIN), threshold_interrupt, FALLING);

  sgm58031.setAlertLowThreshold(0x0000);  // Write  0x0000  to Lo_Thresh
  sgm58031.setAlertHighThreshold(0x7FFF); // Write 0x7FFF to Hi_Thresh
  sgm58031.setConfig(0xC2E0);             // Write config, OS=1, AIN0 to GND, G=(+/-4.096V input range)
  sgm58031.setVoltageResolution(SGM58031_FS_4_096);
  delay(1000);
}




void get_liquid_distance(uint16_t resistance_max, uint16_t resistance, uint8_t length, uint16_t ref)
{
  float gVoltage = sgm58031.getVoltage();
  Serial.print(F("gVoltage="));
  Serial.print(gVoltage);
  Serial.println("V");

  float x1 = ((resistance_max + ref) * gVoltage) - (3.3 * ref);
  float x2 = (resistance / length) *  gVoltage;
  Serial.printf("x1=%f  x2=%f\r\n", x1, x2);
  if (x1 <= EPISON)
  {
    return ;
  }
  float distance_inch = x1 / x2;
  float distance_cm = distance_inch * 2.54;
  Serial.printf("distance:%0.2f inch   %0.2f cm \r\n", distance_inch, distance_cm);
}



void loop()
{
  // put your main code here, to run repeatedly:
  if (interrupt_flag)
  {
    interrupt_flag = false;
    Serial.println("The voltage exceeds the preset threshold. Please check");
  }
  get_liquid_distance(RESISTANCE_MAX, RESISTANCE, LENGTH, REF);
  Serial.println("");
  delay(1000);
}

void threshold_interrupt(void)
{
  interrupt_flag = true;
}
