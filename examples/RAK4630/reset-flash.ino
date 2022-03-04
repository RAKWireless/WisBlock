#include <InternalFileSystem.h>

// resets device flash storage and then blinks LED

void setup() 
{
  Serial.begin(115200);

#if CFG_DEBUG
  // Blocking wait for connection when debug mode is enabled via IDE
  while ( !Serial ) yield();
#endif

  Serial.println("InternalFS Format");
  InternalFS.format();

}

void loop() 
{
  // Toggle both LEDs every 1 second
  digitalToggle(LED_GREEN);

  delay(1000);
}
