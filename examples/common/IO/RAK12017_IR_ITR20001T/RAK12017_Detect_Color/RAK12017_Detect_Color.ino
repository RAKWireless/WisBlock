/**
   @file RAK12017_Detect_Color.ino
   @author rakwireless.com
   @brief Detect the White or Black
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
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, LOW);   
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
    Serial.println("White");
    digitalWrite(LED_BLUE, HIGH);
    delay(50);
    digitalWrite(LED_BLUE, LOW);         
  }
  else
  {
    Serial.println("Black");
    digitalWrite(LED_GREEN, HIGH);
    delay(50);
    digitalWrite(LED_GREEN, LOW);             
  }

}
