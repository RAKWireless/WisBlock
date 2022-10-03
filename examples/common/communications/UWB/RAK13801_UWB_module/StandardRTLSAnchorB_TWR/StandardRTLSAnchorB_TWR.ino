/**
   @file StandardRTLSAnchorB_TWR.ino
   @author rakwireless.com
   @brief Standard RTLS AnchorB TWR
   @version 0.1
   @date 2022-1-28
   @copyright Copyright (c) 2022
**/

#include <DW1000Ng.hpp>
#include <DW1000NgUtils.hpp>
#include <DW1000NgRanging.hpp>
#include <DW1000NgRTLS.hpp>

// connection pins
#if defined(ESP8266)
const uint8_t PIN_SS = 15;
#else
const uint8_t PIN_SS = SS; // spi select pin
const uint8_t PIN_RST = WB_IO6;
const uint8_t PIN_IRQ = WB_IO5;
#endif

byte main_anchor_address[] = {0x01, 0x00};
uint16_t next_anchor = 3;

double range_self;

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

frame_filtering_configuration_t ANCHOR_FRAME_FILTER_CONFIG = {
  false,
  false,
  true,
  false,
  false,
  false,
  false,
  false
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
  Serial.println(F("### arduino-DW1000Ng-ranging-anchor-B ###"));
  // initialize the driver
#if defined(ESP8266)
  DW1000Ng::initializeNoInterrupt(PIN_SS);
#else
  DW1000Ng::initializeNoInterrupt(PIN_SS, PIN_RST);
#endif
  Serial.println(F("DW1000Ng initialized ..."));
  // general configuration
  DW1000Ng::applyConfiguration(DEFAULT_CONFIG);
  DW1000Ng::enableFrameFiltering(ANCHOR_FRAME_FILTER_CONFIG);

  DW1000Ng::setEUI("AA:BB:CC:DD:EE:FF:00:02");

  DW1000Ng::setAntennaDelay(16500);//16436
  DW1000Ng::setSfdDetectionTimeout(273);
  DW1000Ng::setReceiveFrameWaitTimeoutPeriod(5000);

  DW1000Ng::setNetworkId(RTLS_APP_ID);
  DW1000Ng::setDeviceAddress(2);

  DW1000Ng::setAntennaDelay(16500);

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

void transmitRangeReport() {
  byte rangingReport[] = {DATA, SHORT_SRC_AND_DEST, DW1000NgRTLS::increaseSequenceNumber(), 0, 0, 0, 0, 0, 0, 0x60, 0, 0 };
  DW1000Ng::getNetworkId(&rangingReport[3]);
  memcpy(&rangingReport[5], main_anchor_address, 2);
  DW1000Ng::getDeviceAddress(&rangingReport[7]);
  DW1000NgUtils::writeValueToBytes(&rangingReport[10], static_cast<uint16_t>((range_self * 1000)), 2);
  DW1000Ng::setTransmitData(rangingReport, sizeof(rangingReport));
  DW1000Ng::startTransmit();
}

void loop() {
  RangeAcceptResult result = DW1000NgRTLS::anchorRangeAccept(NextActivity::RANGING_CONFIRM, next_anchor);
  if (result.success) {
    //delay(1);
    delayMicroseconds(300); // Tweak based on your hardware
    range_self = result.range;
    transmitRangeReport();

    String rangeString = "Range: "; rangeString += range_self; rangeString += " m";
    rangeString += "\t RX power: "; rangeString += DW1000Ng::getReceivePower(); rangeString += " dBm";
    Serial.println(rangeString);
  }
}
