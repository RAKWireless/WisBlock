/**
   @file StandardRTLSTag_TWR.ino
   @author rakwireless.com
   @brief Standard RTLS Tag TWR
   @version 0.1
   @date 2022-1-28
   @copyright Copyright (c) 2022
**/

#include <DW1000Ng.hpp>
#include <DW1000NgUtils.hpp>
#include <DW1000NgTime.hpp>
#include <DW1000NgConstants.hpp>
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

volatile uint32_t blink_rate = 500;

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

frame_filtering_configuration_t TAG_FRAME_FILTER_CONFIG = {
  false,
  false,
  true,
  false,
  false,
  false,
  false,
  false
};

sleep_configuration_t SLEEP_CONFIG = {
  false,  // onWakeUpRunADC   reg 0x2C:00
  false,  // onWakeUpReceive
  false,  // onWakeUpLoadEUI
  true,   // onWakeUpLoadL64Param
  true,   // preserveSleep
  true,   // enableSLP    reg 0x2C:06
  false,  // enableWakePIN
  true    // enableWakeSPI
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
  Serial.println(F("### DW1000Ng-arduino-ranging-tag ###"));
  // initialize the driver
#if defined(ESP8266)
  DW1000Ng::initializeNoInterrupt(PIN_SS);
#else
  DW1000Ng::initializeNoInterrupt(PIN_SS, PIN_RST);
#endif
  Serial.println("DW1000Ng initialized ...");
  // general configuration
  DW1000Ng::applyConfiguration(DEFAULT_CONFIG);
  DW1000Ng::enableFrameFiltering(TAG_FRAME_FILTER_CONFIG);

  DW1000Ng::setEUI("AA:BB:CC:DD:EE:FF:00:00");

  DW1000Ng::setNetworkId(RTLS_APP_ID);

  DW1000Ng::setAntennaDelay(16500);//16436

  DW1000Ng::applySleepConfiguration(SLEEP_CONFIG);

  DW1000Ng::setPreambleDetectionTimeout(15);
  DW1000Ng::setSfdDetectionTimeout(273);
  DW1000Ng::setReceiveFrameWaitTimeoutPeriod(2000);

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
  DW1000Ng::deepSleep();
  delay(blink_rate);
  DW1000Ng::spiWakeup();
  DW1000Ng::setEUI("AA:BB:CC:DD:EE:FF:00:00");
  //  delay(2);
  RangeInfrastructureResult res = DW1000NgRTLS::tagTwrLocalize(1500);
  if (res.success)
    blink_rate = res.new_blink_rate;
}
