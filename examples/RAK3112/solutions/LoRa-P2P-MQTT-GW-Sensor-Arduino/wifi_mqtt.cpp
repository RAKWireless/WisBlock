/**
 * @file wifi_mqtt.cpp
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief WiFi and MQTT functions
 * @version 0.1
 * @date 2024-08-17
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "app.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include <esp_wifi.h>
#include <PubSubClient.h> // https://github.com/knolleary/pubsubclient/archive/master.zip

/** Multi WiFi */
extern WiFiMulti wifi_multi;

// Replace it with your MQTT Broker IP address or domain
char g_mqtt_server[128] = "000.000.000.000";

// if need username and password to connect mqtt server, they cannot be NULL.
char g_mqtt_user[128] = "MQTT_USER";
char g_mqtt_pw[128] = "MQTT_PW";
char g_mqtt_publish[128] = "rak11160";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

/**
 * @brief Setup WiFi and MQTT connections
 *
 */
void setup_wifi(void)
{
	delay(10);
	// We start by connecting to a WiFi network
	MYLOG("WiFi", "Connecting to %s or %s", g_ssid_prim, g_ssid_sec);

	// Setup mqtt broker
	mqttClient.setServer(g_mqtt_server, 1883);
	mqttClient.setBufferSize(1024);
	mqttClient.setKeepAlive(g_lorawan_settings.send_repeat_time / 1000 * 2);
	mqttClient.setSocketTimeout(g_lorawan_settings.send_repeat_time / 1000 * 2);

	//* ********************************************************* */
	//* Requires WiFi credentials setup through WisBlock Toolbox  */
	//* or through AT commands                                    */
	//* Until AT commands implemented, set them here manually     */
	//* ********************************************************* */
	Preferences preferences;
	// Init Wifi with WisBlock-API-V2
	init_wifi();

	// Get MQTT settings
	preferences.begin("MQTTCred", false);
	if (preferences.getBool("valid", false))
	{
		preferences.getString("g_mqtt_url", g_mqtt_server, 128);
		preferences.getString("g_mqtt_user", g_mqtt_user, 128);
		preferences.getString("g_mqtt_pw", g_mqtt_pw, 128);
	}
	else
	{
		preferences.putString("g_mqtt_url", "mqtt_server\0");
		preferences.putString("g_mqtt_user", "mqttUsername\0");
		preferences.putString("g_mqtt_pw", "mqttPassword\0");
		preferences.putBool("valid", true);
	}
	preferences.end();

	MYLOG("WiFi", "WiFi 1 >%s<", g_ssid_prim.c_str());
	MYLOG("WiFi", "WiFi 1 PW >%s<", g_pw_prim.c_str());
	MYLOG("WiFi", "WiFi 2 >%s<", g_ssid_sec.c_str());
	MYLOG("WiFi", "WiFi 2 PW >%s<", g_pw_sec.c_str());
	MYLOG("WiFi", "MQTT PW >%s<", g_mqtt_pw);
	MYLOG("WiFi", "MQTT Name >%s<", g_mqtt_user);
	MYLOG("WiFi", "MQTT URL >%s<", g_mqtt_server);
	MYLOG("WiFi", "MQTT ID >%s<", g_ap_name);

	time_t start_wait = millis();
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		if ((millis() - start_wait) > 5000)
		{
			MYLOG("WiFi", "No connection in 5 seconds");
			break;
		}
	}

	if (WiFi.status() == WL_CONNECTED)
	{
		String ips = WiFi.localIP().toString();
		MYLOG("WiFi", "WiFi connected, IP address: %s", ips.c_str());

		// Connect to MQTT server
		// if (mqttClient.connect(mqttClientId, g_mqtt_user, g_mqtt_pw, "P2P_GW", 1, true, "Connected"))
		mqttClient.setSocketTimeout(60);
		if (mqttClient.connect(g_ap_name, g_mqtt_user, g_mqtt_pw, "RAK3112", 1, true, "Alive"))
		{
			MYLOG("MQTT", "MQTT connected");
		}
		else
		{
			MYLOG("MQTT", "MQTT connect failed code %d", mqttClient.state());
		}
	}
}

/**
 * @brief Check WiFi connection and re-connect to MQTT broker
 *
 */
void reconnect_wifi(void)
{
	if (WiFi.status() != WL_CONNECTED)
	{
		// To be checked, the WiFi functions in the WisBlock-API-V2 should automatically try to reconnect
		// WiFi.begin(ssid, password);

		// Try to just run wifi_multi.run();
		wifi_multi.run();

		time_t start_wait = millis();
		while (WiFi.status() != WL_CONNECTED)
		{
			delay(500);
			if ((millis() - start_wait) > 5000)
			{
				MYLOG("WiFi", "No connection in 5 seconds");
				break;
			}
		}
	}

	if ((WiFi.status() == WL_CONNECTED) && !mqttClient.connected())
	{
		// Connect to MQTT server
		// if (mqttClient.connect(mqttClientId, g_mqtt_user, g_mqtt_pw, "P2P_GW", 1, true, "Connected"))
		mqttClient.setSocketTimeout(60);
		if (mqttClient.connect(g_ap_name, g_mqtt_user, g_mqtt_pw, "RAK3112", 1, true, "Alive"))
		{
			MYLOG("MQTT", "MQTT connected");
		}
		else
		{
			MYLOG("MQTT", "MQTT failed code %d", mqttClient.state());
		}
	}
}

/**
 * @brief Publish a topic to the MQTT broker
 *
 * @param topic char array with the topic
 * @param payload char array with the payload (we use JSON here)
 * @return true Publish successful
 * @return false Publish failed (MQTT or WiFi connection problem)
 */
bool publish_mqtt(char *topic, char *payload)
{
	check_mqtt();
	if ((WiFi.status() != WL_CONNECTED) || !mqttClient.connected())
	{
		MYLOG("MQTT", "No connection");
		reconnect_wifi();
	}
	if ((WiFi.status() == WL_CONNECTED) && mqttClient.connected())
	{
		MYLOG("MQTT", "Try to send");
		if (mqttClient.publish(topic, payload))
		{
			MYLOG("MQTT", "Publish returned OK");
			return true;
		}
		else
		{
			MYLOG("MQTT", "Publish returned FAIL");
			return false;
		}
	}
	return false;
}

/**
 * @brief Check connection to MQTT broker
 * 		If disconnected, try to reconnect
 * 		Run the mqttClient loop should not be required
 * 		as we do not subscribe to anything
 *
 */
void check_mqtt(void)
{
	reconnect_wifi();
	mqttClient.loop();
}