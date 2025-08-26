/**
 * @file app.h
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief For application specific includes and definitions
 *        Will be included from app.h
 * @version 0.3
 * @date 2023-03-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef APP_H
#define APP_H

//**********************************************/
//** Set the application firmware version here */
//**********************************************/
// major version increase on API change / not backwards compatible
#ifndef SW_VERSION_1
#define SW_VERSION_1 1
#endif
// minor version increase on API change / backward compatible
#ifndef SW_VERSION_2
#define SW_VERSION_2 0
#endif
// patch version increase on bugfix, no affect on API
#ifndef SW_VERSION_3
#define SW_VERSION_3 11
#endif

#include <Arduino.h>
/** Add you required includes after Arduino.h */

#include <Wire.h>
/** Include the WisBlock-API */
#include <WisBlock-API-V2.h> 

// Add debug output macros
#include "debug_log.h"

/** Application function definitions */
void setup_app(void);
bool init_app(void);
void app_event_handler(void);
void ble_data_handler(void) __attribute__((weak));
void lora_data_handler(void);
void init_user_at(void);

extern bool init_result;
extern uint8_t g_last_fport;

extern Ticker delayed_sending;
void send_delayed(void);

// Wakeup flags
#define PARSE 0b1000000000000000
#define N_PARSE 0b0111111111111111

// Cayenne LPP Channel numbers per sensor value
#define LPP_CHANNEL_BATT 1 // Base Board

/** Module stuff */
#include "module_handler.h"

// WiFi and MQTT stuff
void setup_wifi(void);
void reconnect_wifi(void);
bool publish_mqtt(char *topic, char *payload);
void check_mqtt(void);
extern String g_ssid_prim;
extern String g_ssid_sec;
extern String g_pw_prim;
extern String g_pw_sec;
extern char g_mqtt_server[];
extern char g_mqtt_user[];
extern char g_mqtt_pw[];
extern char g_mqtt_publish[];

// Parser
typedef struct _stParam
{
	char *argv[25];
	int argc;
} stParam;
bool mqtt_parse_send(uint8_t *data, uint16_t data_len);
bool parseBuff2Param(char *bufCmd, stParam *pParam, uint8_t maxargu);


#endif