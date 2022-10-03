 /**
   @file BasicReceiver.ino
   @author rakwireless.com
   @brief UWB transmit receiver
   @version 0.1
   @date 2022-1-28
   @copyright Copyright (c) 2022
**/

#include <SPI.h>
#include <DW1000Ng.hpp>

const uint8_t PIN_SS = SS; // spi select pin
const uint8_t PIN_RST = WB_IO6;
const uint8_t PIN_IRQ = WB_IO5;

int16_t numReceived = 0; // todo check int type
String message;

device_configuration_t DEFAULT_CONFIG = {
    false,
    true,
    true,
    true,
    false,
    SFDMode::STANDARD_SFD,
    Channel::CHANNEL_5,
    DataRate::RATE_850KBPS,
    PulseFrequency::FREQ_16MHZ,
    PreambleLength::LEN_256,
    PreambleCode::CODE_3
};


void setup() {
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH); //Turn on the power switch
  delay(300);
  time_t serial_timeout = millis();
  // DEBUG monitoring
  Serial.begin(115200);
  while (!Serial)
  {
    if ((millis() - serial_timeout) < 5000)
    {
      delay(100);
    }
    else
    {
      break;
    }
  }
  Serial.println(F("### DW1000Ng-arduino-receiver-test ###"));
  // initialize the driver
  DW1000Ng::initializeNoInterrupt(PIN_SS,PIN_RST);
  Serial.println(F("DW1000Ng initialized ..."));

  DW1000Ng::applyConfiguration(DEFAULT_CONFIG);

  DW1000Ng::setDeviceAddress(6);
  DW1000Ng::setNetworkId(10);

  DW1000Ng::setAntennaDelay(16436);
  Serial.println(F("Committed configuration ..."));
  // DEBUG chip info and registers pretty printed
  char msg[128];
  DW1000Ng::getPrintableDeviceIdentifier(msg);
  Serial.print("Device ID: "); Serial.println(msg);
  DW1000Ng::getPrintableExtendedUniqueIdentifier(msg);
  Serial.print("Unique ID: "); Serial.println(msg);
  DW1000Ng::getPrintableNetworkIdAndShortAddress(msg);
  Serial.print("Network ID & Device Address: "); Serial.println(msg);
  DW1000Ng::getPrintableDeviceMode(msg);
  Serial.print("Device mode: "); Serial.println(msg);
}

void loop() {
  DW1000Ng::startReceive();
  while(!DW1000Ng::isReceiveDone()) {
  }
  DW1000Ng::clearReceiveStatus();
  numReceived++;
  // get data as string
  DW1000Ng::getReceivedData(message);
  Serial.print("Received message ... #"); Serial.println(numReceived);
  Serial.print("Data is ... "); Serial.println(message);
  Serial.print("RX power is [dBm] ... "); Serial.println(DW1000Ng::getReceivePower());
  Serial.print("Signal quality is ... "); Serial.println(DW1000Ng::getReceiveQuality());
  Serial.println();
}
