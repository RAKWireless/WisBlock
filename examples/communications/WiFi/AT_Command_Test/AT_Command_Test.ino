
void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while ( !Serial ) delay(10);

  Serial.println("================================");
  Serial.println("ESP32 AT CMD TEST!");
  Serial.println("================================");

  //esp32 init
  Serial1.begin(115200);
}

void loop()
{
  uint8_t resp;
  uint8_t snd;

  while(Serial1.available() > 0)
  {
    resp = (uint8_t)(Serial1.read());
    Serial.write(resp);
  }

  while (Serial.available() > 0)
  {
    snd = (uint8_t)(Serial.read());
    Serial1.write(snd);
  }
}
