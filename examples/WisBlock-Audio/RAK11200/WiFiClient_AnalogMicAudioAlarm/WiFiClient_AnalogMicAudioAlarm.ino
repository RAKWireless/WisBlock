/*
   @file WiFiClient_AnalogMicAudioAlarm.ino
   @author rakwireless.com
   @brief The analog microphone detects sound thresholds and sends an alarm over HTTP.
   @note This example need use the RAK18040 analog microphone module.
   @version 0.1
   @date 2021-01-12
   @copyright Copyright (c) 2020
*/
#include "audio.h" // Click here to install the library: http://librarymanager/All#RAKwireless-Audio
#include <WiFi.h>

NAU85L40B   MIC(0x1D);

int channels = 1;
// default PCM output frequency
static const int frequency = 16000;
int sampleBit = 16;
// buffer to read samples into, each sample is 16-bits
short sampleBuffer[BUFFER_SIZE];

int audio_threshold = 2000;  //You can modify the noise judgment threshold according to your environmental conditions

const char *ssid = "YourWIFI";
const char *password = "YourPassword";

const char* host = "www.baidu.com";   // your server will start a TCP listening
const char* streamId   = "....................";
const char* privateKey = "....................";
int value = 0;
int g_alarm = 0;

void setup()
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  delay(500);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, LOW);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_BLUE, LOW);
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  MIC.begin();
  MIC.config(frequency, (MIC_CHANNEL1 | MIC_CHANNEL2 ), I2S_SAMPLE_16BIT);

  I2S.setI2Sformat(I2S_COMM_FORMAT_STAND_I2S);
  I2S.begin(channels, frequency, sampleBit);
  delay(2000);

  Serial.print("connecting to ");
  Serial.println(host);
}

void send_to_host()
{
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    while (1);
  }

  // We now create a URI for the request
  String url = "/input/";
  url += streamId;
  url += "?private_key=";
  url += privateKey;
  url += "&alarm=";
  url += 1;

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");
}

int abs_int(short data)
{
  if (data > 0) return data;
  else return (0 - data);
}

void loop()
{
  // Read data from analog microphone by I2S
  int sampleRead = I2S.read(sampleBuffer, sizeof(sampleBuffer));
  sampleRead = sampleRead >> 1;
  // wait for samples to be read
  if (sampleRead > 0) {
    uint32_t sum = 0;
    // print samples to the serial monitor or plotter
    for (int i = 0; i < BUFFER_SIZE; i++) {
      sum = sum + abs(sampleBuffer[i]);
    }
    int aver = sum / BUFFER_SIZE;
    if (aver > audio_threshold)
    {
      g_alarm++;
      Serial.printf("Alarm %d\r\n",g_alarm);
      digitalWrite(LED_BLUE, HIGH);
      digitalWrite(LED_GREEN, HIGH);
      send_to_host();
      delay(2000);
      digitalWrite(LED_BLUE, LOW);
      digitalWrite(LED_GREEN, LOW);
    }
    sampleRead = 0;
  }
}
