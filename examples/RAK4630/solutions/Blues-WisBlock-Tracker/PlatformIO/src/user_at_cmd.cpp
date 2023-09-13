/**
 * @file user_at_cmd.cpp
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief User AT commands
 * @version 0.1
 * @date 2023-08-18
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "main.h"

#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
using namespace Adafruit_LittleFS_Namespace;

/** Filename to save Blues settings */
static const char blues_file_name[] = "BLUES";

/** File to save battery check status */
File this_file(InternalFS);

/** Structure for saved Blues Notecard settings */
s_blues_settings g_blues_settings;

/**
 * @brief Set Blues Product UID
 *
 * @param str Product UID as Hex String
 * @return int AT_SUCCESS if ok, AT_ERRNO_PARA_FAIL if invalid value
 */
int at_set_blues_prod_uid(char *str)
{
	if (strlen(str) < 25)
	{
		return AT_ERRNO_PARA_NUM;
	}

	for (int i = 0; str[i] != '\0'; i++)
	{
		if (str[i] >= 'A' && str[i] <= 'Z') // checking for uppercase characters
			str[i] = str[i] + 32;			// converting uppercase to lowercase
	}

	char new_uid[256] = {0};
	snprintf(new_uid, 255, str);

	MYLOG("USR_AT", "Received new Blues Product UID %s", new_uid);

	bool need_save = strcmp(new_uid, g_blues_settings.product_uid) == 0 ? false : true;

	if (need_save)
	{
		snprintf(g_blues_settings.product_uid, 256, new_uid);
	}

	// Save new master node address if changed
	if (need_save)
	{
		save_blues_settings();
	}
	return AT_SUCCESS;
}

/**
 * @brief Get Blues Product UID
 *
 * @return int AT_SUCCESS
 */
int at_query_blues_prod_uid(void)
{
	snprintf(g_at_query_buf, ATQUERY_SIZE, "%s", g_blues_settings.product_uid);
	return AT_SUCCESS;
}

/**
 * @brief Set usage of eSIM or external SIM and APN
 *
 * @param str params as string, format 0 or 1:APN_NAME
 * @return int
 * 			AT_SUCCESS is params are set correct
 * 			AT_ERRNO_PARA_NUM if params error
 */
int at_set_blues_ext_sim(char *str)
{
	char *param;
	bool new_use_ext_sim;
	char new_ext_sim_apn[256];

	// Get string up to first :
	param = strtok(str, ":");
	if (param != NULL)
	{
		if (param[0] == '0')
		{
			MYLOG("USR_AT", "Enable eSIM");
			new_use_ext_sim = false;
		}
		else if (param[0] == '1')
		{
			MYLOG("USR_AT", "Enable external SIM");
			new_use_ext_sim = true;
			param = strtok(NULL, ":");
			if (param != NULL)
			{
				for (int i = 0; param[i] != '\0'; i++)
				{
					if (param[i] >= 'A' && param[i] <= 'Z') // checking for uppercase characters
						param[i] = param[i] + 32;			// converting uppercase to lowercase
				}
				snprintf(new_ext_sim_apn, 256, "%s", param);
			}
			else
			{
				MYLOG("USR_AT", "Missing external SIM APN");
				return AT_ERRNO_PARA_NUM;
			}
		}
		else
		{
			MYLOG("USR_AT", "Invalid SIM flag %d", param[0]);
			return AT_ERRNO_PARA_NUM;
		}
	}

	bool need_save = false;
	if (new_use_ext_sim != g_blues_settings.use_ext_sim)
	{
		g_blues_settings.use_ext_sim = new_use_ext_sim;
		need_save = true;
	}
	if (strcmp(new_ext_sim_apn, g_blues_settings.product_uid) != 0)
	{
		snprintf(g_blues_settings.ext_sim_apn, 256, new_ext_sim_apn);
		need_save = true;
	}

	if (need_save)
	{
		save_blues_settings();
	}
	return AT_SUCCESS;
}

/**
 * @brief Get Blues SIM settings
 *
 * @return int AT_SUCCESS
 */
int at_query_blues_ext_sim(void)
{
	if (g_blues_settings.use_ext_sim)
	{
		snprintf(g_at_query_buf, ATQUERY_SIZE, "1:%s", g_blues_settings.ext_sim_apn);
		MYLOG("USR_AT", "Using external SIM with APN = %s", g_blues_settings.ext_sim_apn);
	}
	else
	{
		snprintf(g_at_query_buf, ATQUERY_SIZE, "0");
		MYLOG("USR_AT", "Using eSIM");
	}
	return AT_SUCCESS;
}

/**
 * @brief Set Blues NoteCard mode
 *       /// \todo work in progress
 *
 * @param str params as string, format 0 or 1
 * @return int
 * 			AT_SUCCESS is params are set correct
 * 			AT_ERRNO_PARA_NUM if params error
 */
int at_set_blues_mode(char *str)
{
	bool new_connection_mode;

	if (str[0] == '0')
	{
		MYLOG("USR_AT", "Set minimum connection mode");
		new_connection_mode = false;
		blues_disable_attn();
	}
	else if (str[0] == '1')
	{
		MYLOG("USR_AT", "Set continuous connection mode");
		new_connection_mode = true;
		blues_enable_attn();
	}
	else
	{
		MYLOG("USR_AT", "Invalid motion trigger flag %d", str[0]);
		return AT_ERRNO_PARA_NUM;
	}

	bool need_save = false;
	if (new_connection_mode != g_blues_settings.conn_continous)
	{
		g_blues_settings.conn_continous = new_connection_mode;
		need_save = true;
	}

	if (need_save)
	{
		save_blues_settings();
	}
	return AT_SUCCESS;
}

/**
 * @brief Get Blues mode settings
 *
 * @return int AT_SUCCESS
 */
int at_query_blues_mode(void)
{
	snprintf(g_at_query_buf, ATQUERY_SIZE, "%s", g_blues_settings.conn_continous ? "1" : "0");
	MYLOG("USR_AT", "Using %s connection", g_blues_settings.conn_continous ? "continous" : "periodic");
	return AT_SUCCESS;
}

/**
 * @brief Enable/disable the motion trigger
 *
 * @param str params as string, format 0 or 1
 * @return int
 * 			AT_SUCCESS is params are set correct
 * 			AT_ERRNO_PARA_NUM if params error
 */
int at_set_blues_trigger(char *str)
{
	bool new_motion_trigger;

	if (str[0] == '0')
	{
		MYLOG("USR_AT", "Disable motion trigger");
		new_motion_trigger = false;
		blues_disable_attn();
	}
	else if (str[0] == '1')
	{
		MYLOG("USR_AT", "Enable motion trigger");
		new_motion_trigger = true;
		blues_enable_attn();
	}
	else
	{
		MYLOG("USR_AT", "Invalid motion trigger flag %d", str[0]);
		return AT_ERRNO_PARA_NUM;
	}

	bool need_save = false;
	if (new_motion_trigger != g_blues_settings.motion_trigger)
	{
		g_blues_settings.motion_trigger = new_motion_trigger;
		need_save = true;
	}

	if (need_save)
	{
		save_blues_settings();
	}
	return AT_SUCCESS;
}

/**
 * @brief Get Blues motion trigger settings
 *
 * @return int AT_SUCCESS
 */
int at_query_blues_trigger(void)
{
	snprintf(g_at_query_buf, ATQUERY_SIZE, "%s", g_blues_settings.motion_trigger ? "1" : "0");
	MYLOG("USR_AT", "Motion trigger is %s", g_blues_settings.motion_trigger ? "enabled" : "disabled");
	return AT_SUCCESS;
}

static int at_reset_blues_settings(void)
{
	if (InternalFS.exists(blues_file_name))
	{
		InternalFS.remove(blues_file_name);
	}
	return AT_SUCCESS;
}

/**
 * @brief Read saved Blues Product ID
 *
 */
bool read_blues_settings(void)
{
	bool structure_valid = false;
	if (InternalFS.exists(blues_file_name))
	{
		this_file.open(blues_file_name, FILE_O_READ);
		this_file.read((void *)&g_blues_settings.valid_mark, sizeof(s_blues_settings));
		this_file.close();

		// Check for valid data
		if (g_blues_settings.valid_mark == 0xAA55)
		{
			structure_valid = true;
			MYLOG("USR_AT", "Valid Blues settings found, Blues Product UID = %s", g_blues_settings.product_uid);
			if (g_blues_settings.use_ext_sim)
			{
				MYLOG("USR_AT", "Using external SIM with APN = %s", g_blues_settings.ext_sim_apn);
			}
			else
			{
				MYLOG("USR_AT", "Using eSIM");
			}
		}
		else
		{
			MYLOG("USR_AT", "No valid Blues settings found");
		}
	}

	if (!structure_valid)
	{
		return false;

		// No settings file found optional to set defaults (ommitted!)
		// g_blues_settings.valid_mark = 0xAA55;										// Validity marker
		// sprintf(g_blues_settings.product_uid, "com.my-company.my-name:my-project"); // Blues Product UID
		// g_blues_settings.conn_continous = false;									// Use periodic connection
		// g_blues_settings.use_ext_sim = false;										// Use external SIM
		// sprintf(g_blues_settings.ext_sim_apn, "-");									// APN to be used with external SIM
		// g_blues_settings.motion_trigger = true;										// Send data on motion trigger
		// save_blues_settings();
	}

	return true;
}

/**
 * @brief Save the Blues Product ID
 *
 */
void save_blues_settings(void)
{
	if (InternalFS.exists(blues_file_name))
	{
		InternalFS.remove(blues_file_name);
	}

	g_blues_settings.valid_mark = 0xAA55;
	this_file.open(blues_file_name, FILE_O_WRITE);
	this_file.write((const char *)&g_blues_settings.valid_mark, sizeof(s_blues_settings));
	this_file.close();
	MYLOG("USR_AT", "Saved Blues Settings");
}

/**
 * @brief List of all available commands with short help and pointer to functions
 *
 */
atcmd_t g_user_at_cmd_new_list[] = {
	/*|    CMD    |     AT+CMD?      |    AT+CMD=?    |  AT+CMD=value |  AT+CMD  | Permissions |*/
	// Module commands
	{"+BUID", "Set/get the Blues product UID", at_query_blues_prod_uid, at_set_blues_prod_uid, NULL, "RW"},
	{"+BSIM", "Set/get Blues SIM settings", at_query_blues_ext_sim, at_set_blues_ext_sim, NULL, "RW"},
	{"+BMOD", "Set/get Blues NoteCard connection modes", at_query_blues_mode, at_set_blues_mode, NULL, "RW"},
	{"+BTRIG", "Set/get Blues send trigger", at_query_blues_trigger, at_set_blues_trigger, NULL, "RW"},
	{"+BR", "Remove all Blues Settings", NULL, NULL, at_reset_blues_settings, "RW"},
};

/** Number of user defined AT commands */
uint8_t g_user_at_cmd_num = 0;

/** Pointer to the combined user AT command structure */
atcmd_t *g_user_at_cmd_list;

/**
 * @brief Initialize the user defined AT command list
 *
 */
void init_user_at(void)
{
	// Assign custom AT command list to pointer used by WisBlock API
	g_user_at_cmd_list = g_user_at_cmd_new_list;

	// Add AT commands to structure
	g_user_at_cmd_num += sizeof(g_user_at_cmd_new_list) / sizeof(atcmd_t);
	MYLOG("USR_AT", "Added %d User AT commands", g_user_at_cmd_num);
}
