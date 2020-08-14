#include <ArduinoRS485.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);

  RS485.begin(9600);

  // enable reception, can be disabled with: RS485.noReceive();
  RS485.receive();
}

void loop() {
  if (RS485.available()) {
    Serial.write(RS485.read());
  }
}
