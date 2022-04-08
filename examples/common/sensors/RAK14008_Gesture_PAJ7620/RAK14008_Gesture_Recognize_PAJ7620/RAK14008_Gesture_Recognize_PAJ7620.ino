/**
   @file RAK14008_Gesture_Recognize_PAJ7620.ino
   @author rakwireless.com
   @brief This example can recognize 9 gestures and output the result,
          including move up, move down, move left, move right, move forward,
          move backward, circle-clockwise, circle-counter clockwise, and wave.
   @version 0.1
   @date 2021-9-28
   @copyright Copyright (c) 2021
**/

#include "RevEng_PAJ7620.h" //Click here to get the library: http://librarymanager/All#Gesture_PAJ7620

RevEng_PAJ7620 sensor = RevEng_PAJ7620();   // Create gesture sensor Object.

void setup()
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);

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
  Serial.println("PAJ7620 sensor demo: Recognizing all 9 gestures.");

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
}

void loop()
{
  Gesture gesture;                  // Gesture is an enum type from RevEng_PAJ7620.h
  gesture = sensor.readGesture();   // Read back current gesture (if any) of type Gesture

  switch (gesture)
  {
    case GES_FORWARD:
    {
      Serial.print("GES_FORWARD");
      break;
    }
    case GES_BACKWARD:
    {
      Serial.print("GES_BACKWARD");
      break;
    }
    case GES_LEFT:
    {
      Serial.print("GES_LEFT");
      break;
    }
    case GES_RIGHT:
    {
      Serial.print("GES_RIGHT");
      break;
    }
    case GES_UP:
    {
      Serial.print("GES_UP");
      break;
    }
    case GES_DOWN:
    {
      Serial.print("GES_DOWN");
      break;
    }
    case GES_CLOCKWISE:
    {
      Serial.print("GES_CLOCKWISE");
      break;
    }
    case GES_ANTICLOCKWISE:
    {
      Serial.print("GES_ANTICLOCKWISE");
      break;
    }
    case GES_WAVE:
    {
      Serial.print("GES_WAVE");
      break;
    }
    case GES_NONE:
    {
      break;
    }
  }

  if(gesture != GES_NONE)
  {
    Serial.print(", Code: ");
    Serial.println(gesture);
  }

  delay(100);
}
