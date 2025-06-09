/**
 * @file RAK3112_WiFi_Client.ino
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief WiFi Client example
 * 			This sketch sends data via HTTP GET requests to 45.159.220.139
 * @version 0.1
 * @date 2025-06-09
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <Arduino.h>
#include <WiFi.h>

// WiFi settings
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Change SSID and Password to match with the WiFi AP
// that will be used
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
const char *ssid = "MyHomeLan";		   // "Room-905";
const char *password = "Bernabe@1700"; // "Mxjmxj_905";

/**
 * @brief Arduino setup function, called once
 *
 */
void setup(void)
{
	// Prepare LED's BLUE ==> TX, GREEN ==> Received a packet
	pinMode(LED_GREEN, OUTPUT);
	pinMode(LED_BLUE, OUTPUT);
	digitalWrite(LED_GREEN, LOW);
	digitalWrite(LED_BLUE, LOW);

	// Initialize Serial for debug output
	Serial.begin(115200);
	time_t serial_timeout = millis();
	// On nRF52840 the USB serial is not available immediately
	while (!Serial)
	{
		if ((millis() - serial_timeout) < 5000)
		{
			delay(100);
			digitalWrite(LED_GREEN, !digitalRead(LED_GREEN));
		}
		else
		{
			break;
		}
	}
	digitalWrite(LED_GREEN, LOW);

	// We start by connecting to a WiFi network

	Serial.println();
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(ssid);

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}

	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
}

int value = 0;

/**
 * @brief Arduino loop, runs forever
 *
 */
void loop(void)
{
	//    const uint16_t port = 80;
	//    const char * host = "192.168.1.1"; // ip or dns
	const uint16_t port = 80;
	const char *host = "45.159.220.139"; // ip or dns

	Serial.print("Connecting to ");
	Serial.println(host);

	// Use NetworkClient class to create TCP connections
	NetworkClient client;

	if (!client.connect(host, port))
	{
		Serial.println("Connection failed.");
		Serial.println("Waiting 5 seconds before retrying...");
		delay(5000);
		return;
	}

	// This will send a request to the server
	// uncomment this line to send an arbitrary string to the server
	// client.print("Send this data to the server");
	// uncomment this line to send a basic document request to the server
	client.print("GET /robots.txt\n\n");

	int maxloops = 0;

	Serial.println("================================================");
	// wait for the server's reply to become available
	while (!client.available() && maxloops < 1000)
	{
		maxloops++;
		delay(1); // delay 1 msec
	}
	if (client.available() > 0)
	{
		// read back one line from the server
		String line = client.readStringUntil('\r');
		Serial.println(line);
	}
	else
	{
		Serial.println("client.available() timed out ");
	}
	Serial.println("================================================");

	Serial.println("Closing connection.");
	client.stop();

	Serial.println("Waiting 5 seconds before restarting...");
	delay(5000);
}