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
}

/**
 * LED light up Gradually
 * 
 * @param red          0-255
 * @param green        0-255
 * @param blue         0-255
 * @param stepNum      0-31
 * @param msPerStep    1ms-248ms
 */
void gradualLightUp(uint8_t red, uint8_t green, uint8_t blue, uint8_t stepNum, uint8_t msPerStep)
{
  rgb.setColor(red, green, blue);

  // set dimming up end, range is 1 to 30
  rgb.setGradualDimmingUpEnd(stepNum);

  // set dimming step ms, range is 1 ms to 248 ms
  rgb.setGradualDimming(msPerStep);

  delay(stepNum*msPerStep);
}

/**
 * LED dark down Gradually
 * 
 * @param red          0-255
 * @param green        0-255
 * @param blue         0-255
 * @param stepNum      0-31
 * @param msPerStep    1ms-248ms
 */
void gradualDarkDown(uint8_t red, uint8_t green, uint8_t blue, uint8_t stepNum, uint8_t msPerStep)
{
  rgb.setColor(red, green, blue);

  // set dimming up end, range is 1 to 30
  rgb.setGradualDimmingDownEnd(31-stepNum);

  // set dimming step ms, range is 1 ms to 248 ms
  rgb.setGradualDimming(msPerStep);

  delay(stepNum*msPerStep);
}

void loop() {
  // RED
  gradualLightUp(100, 0, 0, 30, 100);
  gradualDarkDown(100, 0, 0, 30, 100);

  // GREEN
  gradualLightUp(0, 100, 0, 30, 100);
  gradualDarkDown(0, 100, 0, 30, 100);

  // BLUE
  gradualLightUp(0, 0, 100, 30, 100);
  gradualDarkDown(0, 0, 100, 30, 100);
  
  // YELLOW
  gradualLightUp(100, 100, 0, 30, 100);
  gradualDarkDown(100, 100, 0, 30, 100);

  // PURPLE
  gradualLightUp(100, 0, 255, 30, 100);
  gradualDarkDown(100, 0, 255, 30, 100);

  // CYAN
  gradualLightUp(0, 100, 100, 30, 100);
  gradualDarkDown(0, 100, 100, 30, 100);

  // WHITE
  gradualLightUp(100, 100, 100, 30, 100);
  gradualDarkDown(100, 100, 100, 30, 100);
}
