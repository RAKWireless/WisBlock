/**
   @file RAK12017_Detect_Switch.ino
   @author rakwireless.com
   @brief Detect the objects
   @version 0.1
   @date 2021-8-28
   @copyright Copyright (c) 2020
**/

void setup()
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);  
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_BLUE, LOW);   
  
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
  pinMode(WB_IO4, INPUT);
}

void loop()
{

  if (digitalRead(WB_IO4) == 0)
  {
    Serial.println("IR Status: Sensing");
    Serial.println(" value: 1");
    digitalWrite(LED_BLUE, HIGH);     
  }
  else
  {
    Serial.println("IR Status: Not Sensed");
    Serial.println(" value: 0");
    digitalWrite(LED_BLUE, LOW);     
  }
  delay(500);
}
