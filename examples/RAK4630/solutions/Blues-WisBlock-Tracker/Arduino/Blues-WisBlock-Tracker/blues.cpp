/**
 * @file blues.cpp
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Blues.IO NoteCard handler
 * @version 0.1
 * @date 2023-04-27
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "main.h"

#ifndef PRODUCT_UID
#define PRODUCT_UID "com.my-company.my-name:my-project"
#endif
#define myProductID PRODUCT_UID

Notecard notecard;

void blues_attn_cb(void);

J *req;

/**
 * @brief Initialize Blues NoteCard
 *
 * @return true if NoteCard was found and setup was successful
 * @return false if NoteCard was not found or the setup failed
 */
bool init_blues(void)
{
	Wire.begin();
	notecard.begin();

	// Get the ProductUID from the saved settings
	// If no settings are found, use NoteCard internal settings!
	if (read_blues_settings())
	{
		Serial.printf("Found saved settings, override NoteCard internal settings!\n");
		if (memcmp(g_blues_settings.product_uid, "com.my-company.my-name", 22) == 0)
		{
			Serial.printf("No Product ID saved\n");
			AT_PRINTF(":EVT NO PUID");
			memcpy(g_blues_settings.product_uid, PRODUCT_UID, 33);
		}

		Serial.printf("Set Product ID and connection mode\n");
		if (blues_start_req("hub.set"))
		{
			JAddStringToObject(req, "product", g_blues_settings.product_uid);
			if (g_blues_settings.conn_continous)
			{
				JAddStringToObject(req, "mode", "continuous");
			}
			else
			{
				JAddStringToObject(req, "mode", "minimum");
			}
			// Set sync time to 20 times the sensor read time
			JAddNumberToObject(req, "seconds", (g_lorawan_settings.send_repeat_time * 20 / 1000));
			JAddBoolToObject(req, "heartbeat", true);

			if (!blues_send_req())
			{
				Serial.printf("hub.set request failed\n");
				return false;
			}
		}
		else
		{
			Serial.printf("hub.set request failed\n");
			return false;
		}

#if USE_GNSS == 1
		Serial.printf("Set location mode\n");
		if (blues_start_req("card.location.mode"))
		{
			// Continous GNSS mode
			// JAddStringToObject(req, "mode", "continous");

			// Periodic GNSS mode
			JAddStringToObject(req, "mode", "periodic");

			// Set location acquisition time to the sensor read time
			JAddNumberToObject(req, "seconds", (g_lorawan_settings.send_repeat_time / 2000));
			JAddBoolToObject(req, "heartbeat", true);
			if (!blues_send_req())
			{
				Serial.printf("card.location.mode request failed\n");
				return false;
			}
		}
		else
		{
			Serial.printf("card.location.mode request failed\n");
			return false;
		}
#else
		Serial.printf("Stop location mode\n");
		if (blues_start_req("card.location.mode"))
		{
			// GNSS mode off
			JAddStringToObject(req, "mode", "off");
			if (!blues_send_req())
			{
				Serial.printf("card.location.mode request failed\n");
				return false;
			}
		}
		else
		{
			Serial.printf("card.location.mode request failed\n");
			return false;
		}
#endif

		/// \todo reset attn signal needs rework
		// pinMode(WB_IO5, INPUT);
		// if (g_blues_settings.motion_trigger)
		// {
		// 	if (blues_start_req("card.attn"))
		// 	{
		// 		JAddStringToObject(req, "mode", "disarm");
		// 		if (!blues_send_req())
		// 		{
		// 			Serial.printf("card.attn request failed\n");
		// 		}

		// 		if (!blues_enable_attn())
		// 		{
		// 			return false;
		// 		}
		// 	}
		// }
		// else
		// {
		// 	Serial.printf("card.attn request failed\n");
		// 	return false;
		// }

		Serial.printf("Set APN\n");
		// {“req”:”card.wireless”}
		if (blues_start_req("card.wireless"))
		{
			JAddStringToObject(req, "mode", "auto");

			if (g_blues_settings.use_ext_sim)
			{
				// USING EXTERNAL SIM CARD
				JAddStringToObject(req, "apn", g_blues_settings.ext_sim_apn);
				JAddStringToObject(req, "method", "dual-secondary-primary");
			}
			else
			{
				// USING BLUES eSIM CARD
				JAddStringToObject(req, "method", "primary");
			}
			if (!blues_send_req())
			{
				Serial.printf("card.wireless request failed\n");
				return false;
			}
		}
		else
		{
			Serial.printf("card.wireless request failed\n");
			return false;
		}

#if IS_V2 == 1
		// Only for V2 cards, setup the WiFi network
		Serial.printf("Set WiFi\n");
		if (blues_start_req("card.wifi"))
		{
			JAddStringToObject(req, "ssid", "-");
			JAddStringToObject(req, "password", "-");
			JAddStringToObject(req, "name", "RAK-");
			JAddStringToObject(req, "org", "RAK-PH");
			JAddBoolToObject(req, "start", false);

			if (!blues_send_req())
			{
				Serial.printf("card.wifi request failed\n");
			}
		}
		else
		{
			Serial.printf("card.wifi request failed\n");
			return false;
		}
#endif
	}

	// {"req": "card.version"}
	if (blues_start_req("card.version"))
	{
		if (!blues_send_req())
		{
			Serial.printf("card.version request failed\n");
		}
	}
	return true;
}

/**
 * @brief Send a data packet to NoteHub.IO
 * 
 * @param data Payload as byte array (CayenneLPP formatted)
 * @param data_len Length of payload
 * @return true if note could be sent to NoteCard
 * @return false if note send failed
 */
bool blues_send_payload(uint8_t *data, uint16_t data_len)
{
	if (blues_start_req("note.add"))
	{
		JAddStringToObject(req, "file", "data.qo");
		JAddBoolToObject(req, "sync", true);
		J *body = JCreateObject();
		if (body != NULL)
		{
			char node_id[24];
			sprintf(node_id, "%02x%02x%02x%02x%02x%02x%02x%02x",
					g_lorawan_settings.node_device_eui[0], g_lorawan_settings.node_device_eui[1],
					g_lorawan_settings.node_device_eui[2], g_lorawan_settings.node_device_eui[3],
					g_lorawan_settings.node_device_eui[4], g_lorawan_settings.node_device_eui[5],
					g_lorawan_settings.node_device_eui[6], g_lorawan_settings.node_device_eui[7]);
			JAddStringToObject(body, "dev_eui", node_id);

			JAddItemToObject(req, "body", body);

			JAddBinaryToObject(req, "payload", data, data_len);

			Serial.printf("Finished parsing\n");
			if (!blues_send_req())
			{
				Serial.printf("Send request failed\n");
				return false;
			}
			return true;
		}
		else
		{
			Serial.printf("Error creating body\n");
		}
	}
	return false;
}

/**
 * @brief Create a request structure to be sent to the NoteCard
 *
 * @param request_name name of request, e.g. card.wireless
 * @return true if request could be created
 * @return false if request could not be created
 */
bool blues_start_req(String request_name)
{
	req = notecard.newRequest(request_name.c_str());
	if (req != NULL)
	{
		return true;
	}
	return false;
}

/**
 * @brief Send a completed request to the NoteCard.
 *
 * @return true if request could be sent and the response does not have "err"
 * @return false if request could not be sent or the response did have "err"
 */
bool blues_send_req(void)
{
	char *json = JPrintUnformatted(req);
	Serial.printf("Card request = %s\n", json);

	J *rsp;
	rsp = notecard.requestAndResponse(req);
	if (rsp == NULL)
	{
		return false;
	}
	json = JPrintUnformatted(rsp);
	if (JIsPresent(rsp, "err"))
	{
		Serial.printf("Card error response = %s\n", json);
		notecard.deleteResponse(rsp);
		return false;
	}
	Serial.printf("Card response = %s\n", json);
	notecard.deleteResponse(rsp);

	return true;
}

/**
 * @brief Request NoteHub status, mainly for debug purposes
 *
 */
void blues_hub_status(void)
{
	blues_start_req("hub.status");
	if (!blues_send_req())
	{
		Serial.printf("hub.status request failed\n");
	}
}

/**
 * @brief Get the location information from the NoteCard
 *
 * @return true if a location could be acquired
 * @return false if request failed or no location is available
 */
bool blues_get_location(void)
{
	bool result = false;
	if (blues_start_req("card.location"))
	{
		J *rsp;
		rsp = notecard.requestAndResponse(req);
		if (rsp == NULL)
		{
			Serial.printf("card.location failed, report no location\n");
			return false;
		}
		char *json = JPrintUnformatted(rsp);
		Serial.printf("Card response = %s\n", json);

		if (JHasObjectItem(rsp, "lat") && JHasObjectItem(rsp, "lat"))
		{
			float blues_latitude = JGetNumber(rsp, "lat");
			float blues_longitude = JGetNumber(rsp, "lon");
			float blues_altitude = 0;

			if ((blues_latitude == 0.0) && (blues_longitude == 0.0))
			{
				Serial.printf("No valid GPS data, report no location\n");
			}
			else
			{
				Serial.printf("Got location Lat %.6f Long %0.6f\n", blues_latitude, blues_longitude);
				g_solution_data.addGNSS_6(LPP_CHANNEL_GPS, (uint32_t)(blues_latitude * 10000000), (uint32_t)(blues_longitude * 10000000), blues_altitude);
				result = true;
			}
		}

		notecard.deleteResponse(rsp);
	}

	if (!result)
	{
		// No GPS coordinates, get last tower location
		if (blues_start_req("card.time"))
		{
			J *rsp;
			rsp = notecard.requestAndResponse(req);
			if (rsp == NULL)
			{
				Serial.printf("card.time failed, report no location\n");
				return false;
			}
			char *json = JPrintUnformatted(rsp);
			Serial.printf("Card response = %s\n", json);

			if (JHasObjectItem(rsp, "lat") && JHasObjectItem(rsp, "lat"))
			{
				float blues_latitude = JGetNumber(rsp, "lat");
				float blues_longitude = JGetNumber(rsp, "lon");
				float blues_altitude = 0;

				if ((blues_latitude == 0.0) && (blues_longitude == 0.0))
				{
					Serial.printf("No valid GPS data, report no location\n");
				}
				else
				{
					Serial.printf("Got tower location Lat %.6f Long %0.6f\n", blues_latitude, blues_longitude);
					g_solution_data.addGNSS_6(LPP_CHANNEL_GPS, (uint32_t)(blues_latitude * 10000000), (uint32_t)(blues_longitude * 10000000), blues_altitude);
					result = true;
				}
			}

			notecard.deleteResponse(rsp);
		}
	}

	// Clear last GPS location
	if (blues_start_req("card.location.mode"))
	{
		JAddBoolToObject(req, "delete", true);
		J *rsp;
		rsp = notecard.requestAndResponse(req);
		if (rsp == NULL)
		{
			Serial.printf("card.location.mode\n");
		}
		char *json = JPrintUnformatted(rsp);
		Serial.printf("Card response = %s\n", json);
		notecard.deleteResponse(rsp);
	}
	return result;
}

/**
 * @brief Enable ATTN interrupt
 * 		At the moment enables only the alarm on motion
 *
 * @return true if ATTN could be enabled
 * @return false if ATTN could not be enabled
 */
bool blues_enable_attn(void)
{
	Serial.printf("Enable ATTN on motion\n");
	if (blues_start_req("card.attn"))
	{
		JAddStringToObject(req, "mode", "motion");
		if (!blues_send_req())
		{
			Serial.printf("card.attn request failed\n");
			return false;
		}
	}
	else
	{
		Serial.printf("Request creation failed\n");
	}
	attachInterrupt(WB_IO5, blues_attn_cb, RISING);

	Serial.printf("Arm ATTN on motion\n");
	if (blues_start_req("card.attn"))
	{
		JAddStringToObject(req, "mode", "arm");
		if (!blues_send_req())
		{
			Serial.printf("card.attn request failed\n");
			return false;
		}
	}
	else
	{
		Serial.printf("Request creation failed\n");
	}
	return true;
}

/**
 * @brief Disable ATTN interrupt
 *
 * @return true if ATTN could be disabled
 * @return false if ATTN could not be disabled
 */
bool blues_disable_attn(void)
{
	Serial.printf("Disable ATTN on motion\n");
	detachInterrupt(WB_IO5);

	if (blues_start_req("card.attn"))
	{
		JAddStringToObject(req, "mode", "disarm");
		if (!blues_send_req())
		{
			Serial.printf("card.attn request failed\n");
		}
	}
	else
	{
		Serial.printf("Request creation failed\n");
	}
	if (blues_start_req("card.attn"))
	{
		JAddStringToObject(req, "mode", "-motion");
		if (!blues_send_req())
		{
			Serial.printf("card.attn request failed\n");
		}
	}
	else
	{
		Serial.printf("Request creation failed\n");
	}

	return true;
}

/**
 * @brief Get the reason for the ATTN interrup
 *  /// \todo work in progress
 * @return String reason /// \todo return value not final yet
 */
String blues_attn_reason(void)
{
	return "";
}

/**
 * @brief Callback for ATTN interrupt
 *       Wakes up the app_handler with an BLUES_ATTN event
 *
 */
void blues_attn_cb(void)
{
	api_wake_loop(BLUES_ATTN);
}
