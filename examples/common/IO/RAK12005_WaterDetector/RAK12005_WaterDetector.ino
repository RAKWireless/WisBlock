/**
 * @file RAK12005_WaterDetector.ino
 * @author rakwireless.com
 * @brief use RAK12005 water detector module to detect rain
 * @version 0.1
 * @date 2021-05-24
 * @copyright Copyright (c) 2021
 */
#define SENSOR_PIN  WB_IO6   // Attach Water sensor to Arduino Digital Pin WB_IO6

void setup() 
{
   pinMode(SENSOR_PIN, INPUT);   // The Water Sensor is an Input
   pinMode(LED_GREEN, OUTPUT);  // The LED is an Output
   pinMode(LED_BLUE, OUTPUT);   // The LED is an Output
}
void loop() 
{

   /* The water sensor will switch HIGH when water is detected.
    when water is detected turn LED on, and switch off when no water is present */
   if( digitalRead(SENSOR_PIN) == HIGH) 
   {
      digitalWrite(LED_GREEN,HIGH);   //turn on
      digitalWrite(LED_BLUE,HIGH);
   }else 
   {
      digitalWrite(LED_GREEN,LOW);
      digitalWrite(LED_BLUE,LOW);
   }
}
