#include <Arduino.h>
#include <Wire.h>

void setup()
{
    // Initialize built in green LED
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    // Initialize Serial for debug output
    Serial.begin(115200);

    // Wait for USB Serial to be ready or terminal to be connected
    time_t timeout = millis(); // Timeout in case the system runs on its own
    // Waiting for Serial
    while (!Serial)
    {
        if ((millis() - timeout) < 5000)
        {
            // Blink the LED rapidly while waiting for serial
            delay(100);
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        }
        else
        {
            // Timeout while waiting for USB Serial
            break;
        }
    }

  // After serial connected, print HELLO WORLD and blink LED every second
  while (true)
  {
    delay(1000);
    Serial.println("HELLO WORLD");
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
}

void loop()
{
}
