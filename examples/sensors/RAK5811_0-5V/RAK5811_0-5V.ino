#include <Arduino.h>

#define ULB6_SENSOR

void setup()
{
 /*
  * WisBLOCK 5811 Power On
  */
  pinMode(17, OUTPUT);
  digitalWrite(17, HIGH);

  pinMode(A1, INPUT_PULLDOWN);
  analogReference(AR_INTERNAL_3_0);
  analogOversampling(128);
  
	// Initialize Serial for debug output
	Serial.begin(115200);
  while(!Serial){delay(10);}
}

void loop()
{
  int i;
  
  int sensor_pin = A1;   // the input pin A1 for the potentiometer
  int mcu_ain_value = 0;  

  int depths;         // variable to store the value of oil depths
  int average_value;  
  float voltage_ain; 
  float voltage_sensor; // variable to store the value coming from the sensor

  for (i = 0; i < 5; i++)
  {
    mcu_ain_value += analogRead(sensor_pin);
  }
  average_value = mcu_ain_value / i;
  
  voltage_ain = average_value * 3.0 /1024;  //raef 3.0V / 10bit ADC 
  
  voltage_sensor = voltage_ain / 0.6;       //WisBlock RAK5811 (0 ~ 5V).   Input signal reduced to 6/10 and output
  
  depths =  (voltage_sensor * 1000 - 574) * 2.5; //Convert to millivolt. 574mv is the default output from sensor
  
  Serial.printf("-------average_value------ = %d\n", average_value);
  Serial.printf("-------voltage_sensor------ = %f\n", voltage_sensor);
  Serial.printf("-------depths------ = %d mm\n", depths);

  delay(2000);
}
