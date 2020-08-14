#include <ArduinoRS485.h>

int counter = 0;

void setup() {
  RS485.begin(9600);
}

void loop() {
  RS485.beginTransmission();
  RS485.write("hello ");
  RS485.endTransmission();

  delay(1000);
}
