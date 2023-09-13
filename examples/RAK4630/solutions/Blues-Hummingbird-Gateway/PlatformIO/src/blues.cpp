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
#include "product_uid.h"

#ifndef PRODUCT_UID
#define PRODUCT_UID "com.my-company.my-name:my-project"
#pragma message "PRODUCT_UID is not defined in this example. Please ensure your Notecard has a product identifier set before running this example or define it in code here. More details at https://dev.blues.io/tools-and-sdks/samples/product-uid"
#endif
#define myProductID PRODUCT_UID

Notecard notecard;

J *req;

bool init_blues(void)
{
	Wire.begin();
	notecard.begin();

	// Get the ProductUID from the saved settings
	// If no settings are found, use NoteCard internal settings!
	if (read_blues_settings())
	{
		MYLOG("BLUES", "Found saved settings, override NoteCard internal settings!");
		if (memcmp(g_blues_settings.product_uid, "com.my-company.my-name", 22) == 0)
		{
			MYLOG("BLUES", "No Product ID saved");
			AT_PRINTF(":EVT NO PUID");
			memcpy(g_blues_settings.product_uid, PRODUCT_UID, 33);
		}

		MYLOG("BLUES", "Set Product ID and connection mode");
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
				MYLOG("BLUES", "hub.set request failed");
				return false;
			}
		}
		else
		{
			MYLOG("BLUES", "hub.set request failed");
			return false;
		}

#if USE_GNSS == 1
		MYLOG("BLUES", "Set location mode");
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
				MYLOG("BLUES", "card.location.mode request failed");
				return false;
			}
		}
		else
		{
			MYLOG("BLUES", "card.location.mode request failed");
			return false;
		}
#else
		MYLOG("BLUES", "Stop location mode");
		if (blues_start_req("card.location.mode"))
		{
			// GNSS mode off
			JAddStringToObject(req, "mode", "off");
			if (!blues_send_req())
			{
				MYLOG("BLUES", "card.location.mode request failed");
				return false;
			}
		}
		else
		{
			MYLOG("BLUES", "card.location.mode request failed");
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
		// 			MYLOG("BLUES", "card.attn request failed");
		// 		}

		// 		if (!blues_enable_attn())
		// 		{
		// 			return false;
		// 		}
		// 	}
		// }
		// else
		// {
		// 	MYLOG("BLUES", "card.attn request failed");
		// 	return false;
		// }

		MYLOG("BLUES", "Set APN");
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
				MYLOG("BLUES", "card.wireless request failed");
				return false;
			}
		}
		else
		{
			MYLOG("BLUES", "card.wireless request failed");
			return false;
		}

#if IS_V2 == 1
		// Only for V2 cards, setup the WiFi network
		MYLOG("BLUES", "Set WiFi");
		if (blues_start_req("card.wifi"))
		{
			JAddStringToObject(req, "ssid", "-");
			JAddStringToObject(req, "password", "-");
			JAddStringToObject(req, "name", "RAK-");
			JAddStringToObject(req, "org", "RAK-PH");
			JAddBoolToObject(req, "start", false);

			if (!blues_send_req())
			{
				MYLOG("BLUES", "card.wifi request failed");
			}
		}
		else
		{
			MYLOG("BLUES", "card.wifi request failed");
			return false;
		}
#endif
	}

	// {"req": "card.version"}
	if (blues_start_req("card.version"))
	{
		if (!blues_send_req())
		{
			MYLOG("BLUES", "card.version request failed");
		}
	}
	return true;
}

bool blues_start_req(String request_name)
{
	req = notecard.newRequest(request_name.c_str());
	if (req != NULL)
	{
		return true;
	}
	return false;
}

bool blues_send_req(void)
{
	char *json = JPrintUnformatted(req);
	MYLOG("BLUES", "Card request = %s", json);

	J *rsp;
	rsp = notecard.requestAndResponse(req);
	if (rsp == NULL)
	{
		return false;
	}
	json = JPrintUnformatted(rsp);
	MYLOG("BLUES", "Card response = %s", json);
	notecard.deleteResponse(rsp);

	return true;
}

void blues_hub_status(void)
{
	blues_start_req("hub.status");
	if (!blues_send_req())
	{
		MYLOG("BLUES", "hub.status request failed");
	}
}