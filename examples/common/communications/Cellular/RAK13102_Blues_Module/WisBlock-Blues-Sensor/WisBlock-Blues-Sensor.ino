//
// Copyright 2019 Blues Inc.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.
//
// This example contains the complete source for the Sensor Tutorial at dev.blues.io
// https://dev.blues.io/build/tutorials/sensor-tutorial/notecarrier-af/esp32/arduino-wiring/
//
// This tutorial requires an external Adafruit BME680 Sensor.
//

// Include the Arduino library for the Notecard
#include <Notecard.h>               // Click to install library: http://librarymanager/All#Blues-Wireless-Notecard
#ifdef NRF52
#include <Adafruit_TinyUSB.h>
#endif
#include <Wire.h>
#include <Adafruit_BME680.h>        // Click to install library: http://librarymanager/All#Adafruit-BME680

Adafruit_BME680 bmeSensor(&Wire);

// This is the unique Product Identifier for your device
#ifndef PRODUCT_UID
#define PRODUCT_UID "" // "com.my-company.my-name:my-project"
#pragma message "PRODUCT_UID is not defined in this example. Please ensure your Notecard has a product identifier set before running this example or define it in code here. More details at https://dev.blues.io/tools-and-sdks/samples/product-uid"
#endif

#define myProductID PRODUCT_UID
Notecard notecard;

uint8_t sync_request_counter = 59;

bool has_rak1906 = false;

J *req;

char prn_buff[2048];

bool blues_send_req(void)
{
  char *json = JPrintUnformatted(req);
  Serial.printf("Card request = %s\n", json);

  notecard.sendRequest(req);
  // J *rsp;
  // rsp = notecard.requestAndResponse(req);
  // if (rsp == NULL)
  // {
  //  return false;
  // }
  // json = JPrintUnformatted(rsp);
  // Serial.printf("Card response = %s\n", json);
  // notecard.deleteResponse(rsp);

  return true;
}

// One-time Arduino initialization
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  // Initialize Serial for debug output
  Serial.begin(115200);

  time_t serial_timeout = millis();
  // On nRF52840 the USB serial is not available immediately
  while (!Serial)
  {
    if ((millis() - serial_timeout) < 5000)
    {
      delay(100);
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
    else
    {
      break;
    }
  }
  digitalWrite(LED_BUILTIN, LOW);

  Serial.println("**********************************************");
  Serial.println("Blues Notecard Test");
  Serial.println("**********************************************");

  notecard.setDebugOutputStream(Serial);

  // Initialize the physical I/O channel to the Notecard
  Wire.begin();
  delay(500);
  notecard.begin();

  Serial.println("**********************************************");
  Serial.println("Hub setup:");
//  req = notecard.newRequest("hub.set");
//  if (myProductID[0])
//  {
//    JAddStringToObject(req, "product", myProductID);
//  }
//  JAddStringToObject(req, "mode", "continuous");
//  blues_send_req();

  Serial.println("**********************************************");

  if (!bmeSensor.begin(0x76))
  {
    Serial.println("Could not find a valid BME680 sensor...");
  }
  else
  {
    Serial.println("BME680 Connected...");
    bmeSensor.setTemperatureOversampling(BME680_OS_8X);
    bmeSensor.setHumidityOversampling(BME680_OS_2X);

    has_rak1906 = true;
  }

  delay(2000);

  Serial.println("Get hub sync status:");
  // {“req”:”hub.sync.status”}
  req = notecard.newRequest("hub.sync.status");
  blues_send_req();

  Serial.println("**********************************************");
  delay(2000);

  Serial.println("Get note card status:");
  // {“req”:”card.wireless”}
  req = notecard.newRequest("card.wireless");
  blues_send_req();

  Serial.println("**********************************************");
  delay(2000);

  Serial.println("Get note card version:");
  // {“req”:”card.wireless”}
  req = notecard.newRequest("card.version");
  blues_send_req();

  Serial.println("**********************************************");
  delay(2000);
}

void loop()
{
  Serial.println("**********************************************");
  Serial.println("Get hub sync status:");
  // {“req”:”hub.sync.status”}
  req = notecard.newRequest("hub.sync.status");
  blues_send_req();

  Serial.println("**********************************************");
  Serial.println("Get hub status:");
  // {“req”:”hub.status”}
  req = notecard.newRequest("hub.status");
  blues_send_req();

  Serial.println("**********************************************");
  delay(2000);

  Serial.println("Get note card status:");
  // {“req”:”card.wireless”}
  req = notecard.newRequest("card.wireless");
  blues_send_req();

  Serial.println("**********************************************");
  delay(2000);

  float temperature, humidity, pressure;
  if (has_rak1906)
  {
    if (!bmeSensor.performReading())
    {
      Serial.println("Failed to obtain a reading...");
      delay(15000);
      return;
    }

    temperature = bmeSensor.temperature;
    humidity = bmeSensor.humidity;
    pressure = (float)bmeSensor.pressure / 100.0;

    Serial.printf("Temperature = %.2f *C\n", bmeSensor.temperature);
    Serial.printf("Humidity = %.2f %%RH\n", bmeSensor.humidity);
    Serial.printf("Pressure = %.2f mBar\n", (float)bmeSensor.pressure / 100.0);
  }
  else
  {
    temperature = 28.3;
    humidity = 65.7;
    pressure = 1008.05;
  }

  req = notecard.newRequest("note.add");
  if (req != NULL)
  {
    JAddStringToObject(req, "file", "data.qo");
    // JAddBoolToObject(req, "sync", true);

    J *body = JCreateObject();
    if (body != NULL)
    {
      JAddStringToObject(body, "sensor", "rak4631");
      JAddNumberToObject(body, "temperature", temperature);
      JAddNumberToObject(body, "humidity", humidity);
      JAddNumberToObject(body, "pressure", pressure);
      JAddItemToObject(req, "body", body);
    }

    blues_send_req();
  }

  req = NoteNewRequest("card.location");

  if (req != NULL)
  {
    blues_send_req();
  }

  sync_request_counter++;
  if (sync_request_counter == 60)
  {
    sync_request_counter = 0;
    Serial.println("Get hub sync status:");
    // {“req”:”hub.sync”}
    req = notecard.newRequest("hub.sync");
    blues_send_req();

    Serial.println("**********************************************");

    for (uint8_t check = 0; check < 6; check++)
    {
      Serial.println("Get hub status:");
      // {“req”:”hub.sync.status”}
      req = notecard.newRequest("hub.status");
      blues_send_req();

      Serial.println("**********************************************");

      delay(10000);
    }
  }
  delay(60000);
}
