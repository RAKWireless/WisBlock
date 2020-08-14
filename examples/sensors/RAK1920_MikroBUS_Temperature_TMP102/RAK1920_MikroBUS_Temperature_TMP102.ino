// Include the SparkFun TMP102 library.
// Click here to get the library: http://librarymanager/All#SparkFun_TMP102

#include <Wire.h> // Used to establied serial communication on the I2C bus
#include <SparkFunTMP102.h> // Used to send and recieve specific information from our sensor

TMP102 sensor0;

void setup() {
  Serial.begin(115200);
  while(!Serial){delay(10);}
  Wire.begin(); //Join I2C Bus
  
  /* The TMP102 uses the default settings with the address 0x48 using Wire.
  
   Optionally, if the address jumpers are modified, or using a different I2C bus,
   these parameters can be changed here. E.g. sensor0.begin(0x49,Wire1)
   
   It will return true on success or false on failure to communicate. */
  while(!sensor0.begin())
  {
    Serial.println("Cannot connect to TMP102.");
    Serial.println("Is the board connected? Is the device ID correct?");
    delay(1000);
  }
  
  Serial.println("Connected to TMP102!");
  delay(100);

  // Initialize sensor0 settings  
  // set the number of consecutive faults before triggering alarm.
  // 0-3: 0:1 fault, 1:2 faults, 2:4 faults, 3:6 faults.
  sensor0.setFault(0);  // Trigger alarm immediately
  
  // set the polarity of the Alarm. (0:Active LOW, 1:Active HIGH).
  sensor0.setAlertPolarity(1); // Active HIGH
  
  // set the sensor in Comparator Mode (0) or Interrupt Mode (1).
  sensor0.setAlertMode(0); // Comparator Mode.
  
  // set the Conversion Rate (how quickly the sensor gets a new reading)
  //0-3: 0:0.25Hz, 1:1Hz, 2:4Hz, 3:8Hz
  sensor0.setConversionRate(2);
  
  //set Extended Mode.
  //0:12-bit Temperature(-55C to +128C) 1:13-bit Temperature(-55C to +150C)
  sensor0.setExtendedMode(0);

  //set T_HIGH, the upper limit to trigger the alert on
  sensor0.setHighTempF(85.0);  // set T_HIGH in F
  //sensor0.setHighTempC(29.4); // set T_HIGH in C
  
  //set T_LOW, the lower limit to shut turn off the alert
  sensor0.setLowTempF(84.0);  // set T_LOW in F
  //sensor0.setLowTempC(26.67); // set T_LOW in C
}
 
void loop()
{
  float temperature;
  boolean alertPinState, alertRegisterState;
  
  // Turn sensor on to start temperature measurement.
  // Current consumtion typically ~10uA.
  sensor0.wakeup();

  // read temperature data
  //temperature = sensor0.readTempF();
  temperature = sensor0.readTempC();
  
  // Place sensor in sleep mode to save power.
  // Current consumtion typically <0.5uA.
  sensor0.sleep();

  // Print temperature and alarm state
  Serial.print("Temperature: ");
  Serial.println(temperature);
  
  delay(1000);  // Wait 1000ms
}
