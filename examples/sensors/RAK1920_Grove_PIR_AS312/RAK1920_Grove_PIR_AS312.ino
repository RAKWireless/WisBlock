/*
    Description: Human body detection using PIR Unit.
*/

void setup() {

  Serial.begin(115200);
  while(!Serial);
  pinMode(21, INPUT);
}

void loop() {

  if(digitalRead(21)==1){

    Serial.println("PIR Status: Sensing");
    Serial.println(" value: 1");
  }
  else{
    Serial.println("PIR Status: Not Sensed");
    Serial.println(" value: 0");
  }
  delay(500);
}
