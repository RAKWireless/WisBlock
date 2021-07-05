#include <Wire.h>
#include <NCP5623.h>

NCP5623 rgb;

void setup() {
  // enable RAK14001
  pinMode(WB_IO6, OUTPUT);
  digitalWrite(WB_IO6, HIGH);

  // waits for input in seial monitor
  Serial.begin(115200);
  uint32_t t = millis();
  uint32_t timeout = 5000;
  while (!Serial)
  {
    if (millis() - t > timeout)
      break;
  }

  // If using Native I2C
  Wire.begin();
  Wire.setClock(100000);

  Serial.println("RAK14001 RGB LED Test");

  if (!rgb.begin())
  {
    Serial.println("RAK14001 not found on the I2C line");
    while (1);
  }
  else
  {
    Serial.println("RAK14001 Found. Begining execution");
  }

  // set the current output level max, the range is 1 to 31
  rgb.setCurrent(26);
}

void breathingLamp(bool Red, bool Green, bool Blue)
{
  int pwm = 31;
  for(int i=pwm; i>=0; i--)
  {
    rgb.setColor(Red?i*8:0, Green?i*8:0, Blue?i*8:0);
    delay(100);
  }

  delay(200);

  for(int j=1; j<pwm; j++)
  {
    rgb.setColor(Red?j*8:0, Green?j*8:0, Blue?j*8:0);
    delay(100);
  }
}

void loop() {
  rgb.setColor(0,0,0);     // OFF
  delay(1000);
  rgb.setColor(255,0,0);    // RED
  delay(1000);
  rgb.setColor(0,255,0);    // GREEN
  delay(1000);
  rgb.setColor(0,0,255);    // BLUE
  delay(1000);
  rgb.setColor(255,255,0);  // YELLOW
  delay(1000);
  rgb.setColor(255,0,255);  // PURPLE
  delay(1000);
  rgb.setColor(0,255,255);  // CYAN
  delay(1000);
  rgb.setColor(255,255,255); // WHITE
  delay(1000);

  breathingLamp(true, false, false); // RED Breathing Lamp
  breathingLamp(false, true, false); // GREEN Breathing Lamp
  breathingLamp(false, false, true); // BLUE Breathing Lamp
  breathingLamp(true, true, false);  // YELLOW Breathing Lamp
  breathingLamp(true, false, true);   // PURPLE Breathing Lamp
  breathingLamp(false, true, true);  // CYAN Breathing Lamp
  breathingLamp(true, true, true);   // WHITE Breathing Lamp
}
