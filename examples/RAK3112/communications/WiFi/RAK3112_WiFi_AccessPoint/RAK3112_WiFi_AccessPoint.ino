/**
 * @file RAK3112_WiFi_Client.ino
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief WiFi Client example
 * 			WiFiAccessPoint.ino creates a WiFi access point and provides a web server on it.
 * @Steps:
 * 1. Connect to the access point "RAK3112"
 * 2. Point your web browser to http://192.168.4.1/H to turn the LED on or http://192.168.4.1/L to turn it off OR
 *	  Run raw TCP "GET /H" and "GET /L" on PuTTY terminal with 192.168.4.1 as IP address and 80 as port
 * @version 0.1
 * @date 2025-06-09
 *
 * @copyright Copyright (c) 2025
 *
 */
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>

// WiFi settings
const char *ssid = "RAK3112";
const char *password = "RAKwireless";

/** WiFi Server instance */
WiFiServer server(80);

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

	Serial.println("Configuring access point...");

	// You can remove the password parameter if you want the AP to be open.
	WiFi.softAP(ssid, password);
	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);
	server.begin();

	Serial.println("Server started");
}

/**
 * @brief Arduino loop, runs forever
 *
 */
void loop(void)
{
	WiFiClient client = server.available(); // listen for incoming clients

	if (client)
	{								   // if you get a client,
		Serial.println("New Client."); // print a message out the serial port
		String currentLine = "";	   // make a String to hold incoming data from the client
		while (client.connected())
		{ // loop while the client's connected
			if (client.available())
			{							// if there's bytes to read from the client,
				char c = client.read(); // read a byte, then
				Serial.write(c);		// print it out the serial monitor
				if (c == '\n')
				{ // if the byte is a newline character

					// if the current line is blank, you got two newline characters in a row.
					// that's the end of the client HTTP request, so send a response:
					if (currentLine.length() == 0)
					{
						// HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
						// and a content-type so the client knows what's coming, then a blank line:
						client.println("HTTP/1.1 200 OK");
						client.println("Content-type:text/html");
						client.println();

						// the content of the HTTP response follows the header:
						client.print("Click <a href=\"/H\">here</a> to turn ON the LED.<br>");
						client.print("Click <a href=\"/L\">here</a> to turn OFF the LED.<br>");

						// The HTTP response ends with another blank line:
						client.println();
						// break out of the while loop:
						break;
					}
					else
					{ // if you got a newline, then clear currentLine:
						currentLine = "";
					}
				}
				else if (c != '\r')
				{					  // if you got anything else but a carriage return character,
					currentLine += c; // add it to the end of the currentLine
				}

				// Check to see if the client request was "GET /H" or "GET /L":
				if (currentLine.endsWith("GET /H"))
				{
					digitalWrite(LED_BUILTIN, HIGH); // GET /H turns the LED on
				}
				if (currentLine.endsWith("GET /L"))
				{
					digitalWrite(LED_BUILTIN, LOW); // GET /L turns the LED off
				}
			}
		}
		// close the connection:
		client.stop();
		Serial.println("Client Disconnected.");
	}
}