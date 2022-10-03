/**
   @file StandardRTLSAnchorMain_TWR.ino
   @author rakwireless.com
   @brief Standard RTLS AnchorMain TWR
   @version 0.1
   @date 2022-1-28
   @copyright Copyright (c) 2022
**/

#include <DW1000Ng.hpp>
#include <DW1000NgUtils.hpp>
#include <DW1000NgRanging.hpp>
#include <DW1000NgRTLS.hpp>

typedef struct Position {
  double x;
  double y;
} Position;

// connection pins
#if defined(ESP8266)
const uint8_t PIN_SS = 15;
#else
const uint8_t PIN_SS = SS; // spi select pin
const uint8_t PIN_RST = WB_IO6;
const uint8_t PIN_IRQ = WB_IO5;
#endif


Position position_self = {0, 0};
Position position_B = {4.4, 0.0};
Position position_C = {2.2, 1.80};

double range_self;
double range_B;
double range_C;

boolean received_B = false;

byte target_eui[8];
byte tag_shortAddress[] = {0x05, 0x00};

byte anchor_b[] = {0x02, 0x00};
uint16_t next_anchor = 2;
byte anchor_c[] = {0x03, 0x00};

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
  true /* This allows blink frames */
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
  Serial.println(F("### DW1000Ng-arduino-ranging-anchorMain ###"));
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

  DW1000Ng::setEUI("AA:BB:CC:DD:EE:FF:00:01");

  DW1000Ng::setPreambleDetectionTimeout(64);
  DW1000Ng::setSfdDetectionTimeout(273);
  DW1000Ng::setReceiveFrameWaitTimeoutPeriod(5000);

  DW1000Ng::setNetworkId(RTLS_APP_ID);
  DW1000Ng::setDeviceAddress(1);

  DW1000Ng::setAntennaDelay(16500);//16436

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

/* using https://math.stackexchange.com/questions/884807/find-x-location-using-3-known-x-y-location-using-trilateration */
void calculatePosition(double &x, double &y) {

  /* This gives for granted that the z plane is the same for anchor and tags */
  double A = ( (-2 * position_self.x) + (2 * position_B.x) );
  double B = ( (-2 * position_self.y) + (2 * position_B.y) );
  double C = (range_self * range_self) - (range_B * range_B) - (position_self.x * position_self.x) + (position_B.x * position_B.x) - (position_self.y * position_self.y) + (position_B.y * position_B.y);
  double D = ( (-2 * position_B.x) + (2 * position_C.x) );
  double E = ( (-2 * position_B.y) + (2 * position_C.y) );
  double F = (range_B * range_B) - (range_C * range_C) - (position_B.x * position_B.x) + (position_C.x * position_C.x) - (position_B.y * position_B.y) + (position_C.y * position_C.y);

  x = (C * E - F * B) / (E * A - B * D);
  y = (C * D - A * F) / (B * D - A * E);
}

uint8_t tag_flag;
void loop() {
  if (DW1000NgRTLS::receiveFrame()) {
    size_t recv_len = DW1000Ng::getReceivedDataLength();
    byte recv_data[recv_len];
    DW1000Ng::getReceivedData(recv_data, recv_len);
    if (recv_data[0] == BLINK) {
      //      Serial.print("recv_data=");
      //      for (uint8_t i = 0; i < 10; i++)
      //      {
      //        Serial.printf("0x%02X ", recv_data[i]);
      //      }
      //      Serial.println();
      DW1000NgRTLS::transmitRangingInitiation(&recv_data[2], tag_shortAddress);
      DW1000NgRTLS::waitForTransmission();

      RangeAcceptResult result = DW1000NgRTLS::anchorRangeAccept(NextActivity::RANGING_CONFIRM, next_anchor);
      if (!result.success) return;
      range_self = result.range;

      String rangeString = "";
      switch (recv_data[2])
      {
        case 0x00:
          rangeString += "tag-0 Range: "; rangeString += range_self; rangeString += " m";
          rangeString += "\t RX power: "; rangeString += DW1000Ng::getReceivePower(); rangeString += " dBm";
          tag_flag = recv_data[2];
          break;
        case 0x04:
          rangeString += "tag-1 Range: "; rangeString += range_self; rangeString += " m";
          rangeString += "\t RX power: "; rangeString += DW1000Ng::getReceivePower(); rangeString += " dBm";
          tag_flag = recv_data[2];
          break;
        default:
          tag_flag = 0xFF;
          Serial.println("The label EUI is incorrect. Please Check the label EUI ");
          delay(100);
          break;
      }
      Serial.println(rangeString);
    } else if (recv_data[9] == 0x60) {

      double range = static_cast<double>(DW1000NgUtils::bytesAsValue(&recv_data[10], 2) / 1000.0);
      String rangeReportString = "";
      switch (tag_flag)
      {
        case 0x00:
          rangeReportString += "tag-0 Range: "; rangeReportString += recv_data[7];
          rangeReportString += " = "; rangeReportString += range;
          Serial.println(rangeReportString);
          if (received_B == false && recv_data[7] == anchor_b[0] && recv_data[8] == anchor_b[1]) {
            range_B = range;
            received_B = true;
          } else if (received_B == true && recv_data[7] == anchor_c[0] && recv_data[8] == anchor_c[1]) {
            range_C = range;
            double x, y;
            calculatePosition(x, y);
            String positioning = "Found position(tag-0) - x: ";
            positioning += x; positioning += " y: ";
            positioning += y;
            Serial.println(positioning);
            received_B = false;
          } else {
            received_B = false;
          }
          Serial.println();
          break;
        case 0x04:
          rangeReportString += "tag-1 Range: "; rangeReportString += recv_data[7];
          rangeReportString += " = "; rangeReportString += range;
          Serial.println(rangeReportString);
          if (received_B == false && recv_data[7] == anchor_b[0] && recv_data[8] == anchor_b[1]) {
            range_B = range;
            received_B = true;
          } else if (received_B == true && recv_data[7] == anchor_c[0] && recv_data[8] == anchor_c[1]) {
            range_C = range;
            double x, y;
            calculatePosition(x, y);
            String positioning = "Found position(tag-1) - x: ";
            positioning += x; positioning += " y: ";
            positioning += y;
            Serial.println(positioning);
            received_B = false;
          } else {
            received_B = false;
          }
          Serial.println();
          break;
      }
    }
  }
}
