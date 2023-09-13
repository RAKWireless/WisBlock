/**
 * @file main.h
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Includes, defines and globals
 * @version 0.1
 * @date 2023-04-25
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef _MAIN_H_
#define _MAIN_H_

// Application definitions
// major version increase on API change / not backwards compatible
#define SW_VERSION_1 1     
// minor version increase on API change / backward compatible
#define SW_VERSION_2 0     
// patch version increase on bugfix, no affect on API
#define SW_VERSION_3 0     
// 0 = Notecard V1 version, 1 = Notecard V2 version
#define IS_V2 1        
    
#include <Arduino.h>
#include <WisBlock-API-V2.h> // Click to install library: http://librarymanager/All#WisBlock-API-V2
#include <Notecard.h>        // Click to install library: http://librarymanager/All#Blues-Wireless-Notecard
#include "RAK1906_env.h"

/** Define the version of your SW */
#ifndef SW_VERSION_1
#define SW_VERSION_1 1 // major version increase on API change / not backwards compatible
#endif
#ifndef SW_VERSION_2
#define SW_VERSION_2 0 // minor version increase on API change / backward compatible
#endif
#ifndef SW_VERSION_3
#define SW_VERSION_3 0 // patch version increase on bugfix, no affect on API
#endif

/** Application function definitions */
void setup_app(void);
bool init_app(void);
void app_event_handler(void);
void ble_data_handler(void) __attribute__((weak));
void lora_data_handler(void);

// Wakeup flags
#define PARSE   0b1000000000000000
#define N_PARSE 0b0111111111111111

// Cayenne LPP Channel numbers per sensor value
#define LPP_CHANNEL_BATT 1 // Base Board

// Globals
extern WisCayenne g_solution_data;

// Parser
bool blues_parse_send(uint8_t *data, uint16_t data_len);

// Blues.io
struct s_blues_settings
{
	uint16_t valid_mark = 0xAA55;								 // Validity marker
	char product_uid[256] = "com.my-company.my-name:my-project"; // Blues Product UID
	bool conn_continous = false;								 // Use periodic connection
	bool use_ext_sim = false;									 // Use external SIM
	char ext_sim_apn[256] = "internet";							 // APN to be used with external SIM
	bool motion_trigger = true;									 // Send data on motion trigger
};

bool init_blues(void);
bool blues_send_req(void);
void blues_hub_status(void);
bool blues_start_req(String request_name);
bool blues_send_req(void);
bool blues_send_payload(uint8_t *data, uint16_t data_len);

extern J *req;
extern s_blues_settings g_blues_settings;

// User AT commands
void init_user_at(void);
bool read_blues_settings(void);
void save_blues_settings(void);

#endif // _MAIN_H_
