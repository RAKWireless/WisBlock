/**
   @file BasicSender.ino
   @author rakwireless.com
   @brief UWB transmit sender
   @version 0.1
   @date 2022-1-28
   @copyright Copyright (c) 2022
**/

#include <SPI.h>
#include <DW1000Ng.hpp>

const uint8_t PIN_SS = SS; // spi select pin
const uint8_t PIN_RST = WB_IO6;
const uint8_t PIN_IRQ = WB_IO5;


// DEBUG packet sent status and count
volatile unsigned long delaySent = 0;
int16_t sentNum = 0; // todo check int type

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
  Serial.println(F("### DW1000Ng-arduino-sender-test ###"));
  // initialize the driver
  DW1000Ng::initializeNoInterrupt(PIN_SS,PIN_RST);
  Serial.println(F("DW1000Ng initialized ..."));

  DW1000Ng::applyConfiguration(DEFAULT_CONFIG);
	//DW1000Ng::applyInterruptConfiguration(DEFAULT_INTERRUPT_CONFIG);

  DW1000Ng::setDeviceAddress(5);
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
  // attach callback for (successfully) sent messages
  //DW1000Ng::attachSentHandler(handleSent);
  // start a transmission
  transmit();
}

/*
void handleSent() {
  // status change on sent success
  sentAck = true;
}
*/

void transmit() {
  // transmit some data
  Serial.print("Transmitting packet ... #"); Serial.println(sentNum);
  String msg = "Hello DW1000Ng, it's #"; msg += sentNum;
  DW1000Ng::setTransmitData(msg);
  // delay sending the message for the given amount
  delay(1000);
  DW1000Ng::startTransmit(TransmitMode::IMMEDIATE);
  delaySent = millis();
  while(!DW1000Ng::isTransmitDone()) {

  }
  sentNum++;
  DW1000Ng::clearTransmitStatus();
}

void loop() {
    transmit();
    // update and print some information about the sent message
    Serial.print("ARDUINO delay sent [ms] ... "); Serial.println(millis() - delaySent);
    uint64_t newSentTime = DW1000Ng::getTransmitTimestamp();
    Serial.print("Processed packet ... #"); Serial.println(sentNum);
    Serial.println();
}
