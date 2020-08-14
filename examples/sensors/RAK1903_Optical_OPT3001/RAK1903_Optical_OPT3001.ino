#include <Wire.h>
#include <ClosedCube_OPT3001.h> // Click here to get the library: http://librarymanager/All#OPT3001
ClosedCube_OPT3001 opt3001;
#define OPT3001_ADDRESS 0x44


void setup() {
  // Setup usb 
  Serial.begin(115200);
  while(!Serial);

  /* opt3001 init */
  opt3001.begin(OPT3001_ADDRESS);
  Serial.print("OPT3001 Manufacturer ID");
  Serial.println(opt3001.readManufacturerID());
  Serial.print("OPT3001 Device ID");
  Serial.println(opt3001.readDeviceID());

  configureSensor();
  printResult("High-Limit", opt3001.readHighLimit());
  printResult("Low-Limit", opt3001.readLowLimit());
}

void loop() {

  opt3001_get();
  delay(1000);
}

void configureSensor() {
  OPT3001_Config newConfig;
  
  newConfig.RangeNumber = B1100;  
  newConfig.ConvertionTime = B0;
  newConfig.Latch = B1;
  newConfig.ModeOfConversionOperation = B11;

  OPT3001_ErrorCode errorConfig = opt3001.writeConfig(newConfig);
  if (errorConfig != NO_ERROR)
    printError("OPT3001 configuration", errorConfig);
  else {
    OPT3001_Config sensorConfig = opt3001.readConfig();
    Serial.println("OPT3001 Current Config:");
    Serial.println("------------------------------");
    
    Serial.print("Conversion ready (R):");
    Serial.println(sensorConfig.ConversionReady,HEX);

    Serial.print("Conversion time (R/W):");
    Serial.println(sensorConfig.ConvertionTime, HEX);

    Serial.print("Fault count field (R/W):");
    Serial.println(sensorConfig.FaultCount, HEX);

    Serial.print("Flag high field (R-only):");
    Serial.println(sensorConfig.FlagHigh, HEX);

    Serial.print("Flag low field (R-only):");
    Serial.println(sensorConfig.FlagLow, HEX);

    Serial.print("Latch field (R/W):");
    Serial.println(sensorConfig.Latch, HEX);

    Serial.print("Mask exponent field (R/W):");
    Serial.println(sensorConfig.MaskExponent, HEX);

    Serial.print("Mode of conversion operation (R/W):");
    Serial.println(sensorConfig.ModeOfConversionOperation, HEX);

    Serial.print("Polarity field (R/W):");
    Serial.println(sensorConfig.Polarity, HEX);

    Serial.print("Overflow flag (R-only):");
    Serial.println(sensorConfig.OverflowFlag, HEX);

    Serial.print("Range number (R/W):");
    Serial.println(sensorConfig.RangeNumber, HEX);

    Serial.println("------------------------------");
  }
  
}

void opt3001_get()
{
  OPT3001 result = opt3001.readResult();
  if (result.error == NO_ERROR) {
    Serial.print("OPT3001");
    Serial.print(": ");
    Serial.print(result.lux);
    Serial.println(" lux");

    uint16_t luminosity = result.lux;
  }
  else {
    printError("OPT3001",result.error);
  }
}

void printResult(String text, OPT3001 result) {
  if (result.error == NO_ERROR) {
    Serial.print(text);
    Serial.print(": ");
    Serial.print(result.lux);
    Serial.println(" lux");
  }
  else {
    printError(text,result.error);
  }
}
void printError(String text, OPT3001_ErrorCode error) {
  Serial.print(text);
  Serial.print(": [ERROR] Code #");
  Serial.println(error);
}
