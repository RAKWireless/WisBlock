/**
   @file RAK14008_Gesture_INT_PAJ7620.ino
   @author rakwireless.com
   @brief This example uses an interrupt handler to detect when a gesture has occurred instead of polling for it.
   @version 0.1
   @date 2021-9-28
   @copyright Copyright (c) 2021
**/

#include "RevEng_PAJ7620.h" // Click here to get the library: http://librarymanager/All#RevEng_PAJ7620

#define INTERRUPT_PIN     WB_IO6  // Interrupt capable Arduino pin.

volatile bool g_isr = false;

RevEng_PAJ7620 sensor = RevEng_PAJ7620();   // Create gesture sensor Object.

void setup()
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  pinMode(INTERRUPT_PIN, INPUT);

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

  Serial.println("RAK14008 PAJ7620 Test example.");
  Serial.println("Recognize 9 gestures using interrupt callback.");

  if( !sensor.begin() ) // Returns 0 if sensor connect fail
  {
    Serial.print("PAJ7620 I2C error - halting");
    while(1) 
    {
      delay(10);
    }
  }

  Serial.println("PAJ7620 Init OK.");
  Serial.println("Please input your gestures:");

  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), interruptRoutine, FALLING);
}

void loop()
{
  uint8_t gesture;    // Gesture is an enum type from RevEng_PAJ7620.h
  if (g_isr == true)  // See interruptRoutine for where this is set to true
  {
    g_isr = false;    // Reset ISR flag for next interrupt
    Serial.println("Interrupt!");
    delay(100);
    gesture = sensor.readGesture(); // Read back current gesture (if any) of type Gesture
    switch (gesture)
    {
      case GES_FORWARD:
      {
        Serial.print("-->GES_FORWARD");
        break;
      }
      case GES_BACKWARD:
      {
        Serial.print("-->GES_BACKWARD");
        break;
      }
      case GES_LEFT:
      {
        Serial.print("-->GES_LEFT");
        break;
      }
      case GES_RIGHT:
      {
        Serial.print("-->GES_RIGHT");
        break;
      }
      case GES_UP:
      {
        Serial.print("-->GES_UP");
        break;
      }
      case GES_DOWN:
      {
        Serial.print("-->GES_DOWN");
        break;
      }
      case GES_CLOCKWISE:
      {
        Serial.print("-->GES_CLOCKWISE");
        break;
      }
      case GES_ANTICLOCKWISE:
      {
        Serial.print("-->GES_ANTICLOCKWISE");
        break;
      }
      case GES_WAVE:
      {
        Serial.print("-->GES_WAVE");
        break;
      }
      case GES_NONE:
      {
        break;
      }
    }
    Serial.print(", Gesture Code: ");
    Serial.println(gesture);

    // If a second gesture happens while the first is being handled,
    // isr might be set true again already, to be handled on the next loop()
    if (g_isr == true)
    {
      Serial.println(" --> Interrupt during event processing");
    }
  }
  delay(100);
}

void interruptRoutine()
{
  g_isr = true;
}
