/**
 * @file blues_parse_send.cpp
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Parse received LoRa packet and create Blues NoteCard note
 * @version 0.1
 * @date 2023-04-25
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "main.h"

/** Number of defined sensor types */
#define NUM_DEFINED_SENSOR_TYPES 38

uint8_t value_id[NUM_DEFINED_SENSOR_TYPES] = {0, 1, 2, 3, 100, 101, 102, 103,
											  104, 112, 113, 115, 116, 117, 118, 120,
											  121, 125, 128, 130, 131, 132, 133, 134,
											  135, 136, 137, 138, 142, 188, 190, 191,
											  192, 193, 194, 195, 203, 255};

uint8_t value_size[NUM_DEFINED_SENSOR_TYPES] = {1, 1, 2, 2, 4, 2, 1, 2,
												1, 2, 6, 2, 2, 2, 4, 1,
												2, 2, 2, 4, 4, 2, 4, 6,
												3, 9, 11, 2, 1, 2, 2, 2,
												2, 2, 2, 2, 1, 4};

String value_name[NUM_DEFINED_SENSOR_TYPES] = {"digital_in", "digital_out", "analog_in", "analog_out", "generic", "illuminance", "presence", "temperature",
											   "humidity", "humidity_prec", "accelerometer", "barometer", "voltage", "current", "frequency", "percentage",
											   "altitude", "concentration", "power", "distance", "energy", "direction", "time", "gyrometer",
											   "colour", "gps", "gps", "voc", "switch", "soil_moist", "wind_speed", "wind_direction",
											   "soil_ec", "soil_ph_h", "soil_ph_l", "pyranometer", "light", "node_id"};

uint32_t value_divider[NUM_DEFINED_SENSOR_TYPES] = {1, 1, 100, 100, 1, 1, 1, 10,
													2, 10, 1000, 10, 100, 1000, 1, 1,
													1, 1, 1, 1000, 1000, 1, 1, 100,
													1, 10000, 1000000, 1, 1, 10, 100, 1,
													1000, 100, 10, 1, 1, 1};

// {136;9;"gps";true; [ 10000, 10000, 100 ]},
// {137;11;"gps";true;[ 1000000, 1000000, 100 ]},

bool blues_parse_send(uint8_t *data, uint16_t data_len)
{
	blues_start_req("note.add");

	JAddStringToObject(req, "file", "sensors.qo");
	// JAddBoolToObject(req, "sync", true);
	J *body = JCreateObject();
	if (body != NULL)
	{
		uint16_t byte_idx = 0;
		uint8_t sens_num = 0;
		float float_val1 = 0.0;
		float float_val2 = 0.0;
		float float_val3 = 0.0;
		uint32_t unsigned_val1 = 0;
		uint32_t unsigned_val2 = 0;
		uint32_t unsigned_val3 = 0;
		int32_t signed_val1 = 0;
		String sens_full_name = "";
		char node_id_str[9];
		J *sec_lvl;
		char rounding[40];

		// JAddStringToObject(body, "node_id", sensor_id.c_str());

		while (byte_idx < data_len)
		{
			uint16_t current_byte_idx = byte_idx;
			uint16_t sens_idx = 256;
			sens_num = data[current_byte_idx++];
			MYLOG("PARSE", "Sensor Number %d", sens_num);
			// find matching index
			for (int idx = 0; idx < NUM_DEFINED_SENSOR_TYPES; idx++)
			{
				if (value_id[idx] == data[current_byte_idx])
				{
					sens_idx = idx;
					break;
				}
			}
			if (sens_idx == 256)
			{
				// Wrong sensor ID
				MYLOG("PARSE", "Unknown Sensor %d", data[current_byte_idx]);
				JAddStringToObject(body, "error", "Invalid LPP ID");
				JAddItemToObject(req, "body", body);
				if (!blues_send_req())
				{
					MYLOG("PARSE", "Failed to send error packet");
				}
				return false;
			}
			MYLOG("PARSE", "Found Sensor %d", data[current_byte_idx]);
			current_byte_idx++;
			switch (value_id[sens_idx])
			{
			case 113:
			case 134:
				MYLOG("PARSE", "Found accelerometer or gyrometer");
				sens_full_name = value_name[sens_idx] + "_" + String(sens_num);
				sec_lvl = JAddObjectToObject(body, sens_full_name.c_str());

				float_val1 = (float)((int16_t)data[current_byte_idx + 1] << 8 | (int16_t)data[current_byte_idx]) / value_divider[sens_idx];
				current_byte_idx += 2;
				JAddNumberToObject(sec_lvl, "X", float_val1);
				float_val2 = (float)((int16_t)data[current_byte_idx + 1] << 8 | (int16_t)data[current_byte_idx]) / value_divider[sens_idx];
				current_byte_idx += 2;
				JAddNumberToObject(sec_lvl, "Y", float_val2);
				float_val3 = (float)((int16_t)data[current_byte_idx + 1] << 8 | (int16_t)data[current_byte_idx]) / value_divider[sens_idx];
				current_byte_idx += 2;
				JAddNumberToObject(sec_lvl, "Z", float_val3);
				MYLOG("PARSE", "x %.4f y %.4f z %.4f", float_val1, float_val2, float_val3);
				break;
			case 136:
				MYLOG("PARSE", "Found GPS 4 digit");
				sens_full_name = value_name[sens_idx] + "_" + String(sens_num);
				sec_lvl = JAddObjectToObject(body, sens_full_name.c_str());

				float_val1 = (float)((int16_t)data[current_byte_idx + 2] << 16 | (int16_t)data[current_byte_idx + 1] << 8 | (int16_t)data[current_byte_idx]) / 10000.0;
				current_byte_idx += 3;
				JAddNumberToObject(sec_lvl, "Lat", float_val1);
				float_val2 = (float)((int16_t)data[current_byte_idx + 2] << 16 | (int16_t)data[current_byte_idx + 1] << 8 | (int16_t)data[current_byte_idx]) / 10000.0;
				current_byte_idx += 3;
				JAddNumberToObject(sec_lvl, "Lng", float_val2);
				float_val3 = (float)((int16_t)data[current_byte_idx + 2] << 16 | (int16_t)data[current_byte_idx + 1] << 8 | (int16_t)data[current_byte_idx]) / 100.0;
				current_byte_idx += 3;
				JAddNumberToObject(sec_lvl, "Alt", float_val3);
				MYLOG("PARSE", "lat %.4f lng %.4f alt %.4f", float_val1, float_val2, float_val3);
				break;
			case 137:
				MYLOG("PARSE", "Found GPS 6 digit");
				sens_full_name = value_name[sens_idx] + "_" + String(sens_num);
				sec_lvl = JAddObjectToObject(body, sens_full_name.c_str());

				float_val1 = (float)((int16_t)data[current_byte_idx + 3] << 16 | (int16_t)data[current_byte_idx + 2] << 16 | (int16_t)data[current_byte_idx + 1] << 8 | (int16_t)data[current_byte_idx]) / 1000000.0;
				current_byte_idx += 4;
				JAddNumberToObject(sec_lvl, "Lat", float_val1);
				float_val2 = (float)((int16_t)data[current_byte_idx + 3] << 16 | (int16_t)data[current_byte_idx + 2] << 16 | (int16_t)data[current_byte_idx + 1] << 8 | (int16_t)data[current_byte_idx]) / 1000000.0;
				current_byte_idx += 4;
				JAddNumberToObject(sec_lvl, "Lng", float_val2);
				float_val3 = (float)((int16_t)data[current_byte_idx + 2] << 16 | (int16_t)data[current_byte_idx + 1] << 8 | (int16_t)data[current_byte_idx]) / 100.0;
				current_byte_idx += 3;
				JAddNumberToObject(sec_lvl, "Alt", float_val3);
				MYLOG("PARSE", "lat %.4f lng %.4f alt %.4f", float_val1, float_val2, float_val3);
				break;
			case 135:
				MYLOG("PARSE", "Found Color");
				sens_full_name = value_name[sens_idx] + "_" + String(sens_num);
				sec_lvl = JAddObjectToObject(body, sens_full_name.c_str());

				unsigned_val1 = (int16_t)data[current_byte_idx];
				current_byte_idx += 4;
				JAddNumberToObject(sec_lvl, "Red", unsigned_val1);
				unsigned_val2 = (int16_t)data[current_byte_idx];
				current_byte_idx += 4;
				JAddNumberToObject(sec_lvl, "Green", unsigned_val1);
				unsigned_val3 = (int16_t)data[current_byte_idx];
				current_byte_idx += 3;
				JAddNumberToObject(sec_lvl, "Blue", unsigned_val1);
				MYLOG("PARSE", "r %ld g %ld b %ld", unsigned_val1, unsigned_val2, unsigned_val3);
				break;
			case 255:
				unsigned_val1 = 0;
				for (int cnt = 0; cnt < value_size[sens_idx]; cnt++)
				{
					unsigned_val1 = (unsigned_val1 << 8) | data[current_byte_idx];
					current_byte_idx++;
				}
				sprintf(node_id_str,"%08LX",(uint64_t)unsigned_val1);
				MYLOG("PARSE", "unsigned_val1 %s", node_id_str);

				sens_full_name = value_name[sens_idx] + "_" + String(sens_num);
				JAddStringToObject(body, value_name[sens_idx].c_str(), node_id_str);

				MYLOG("PARSE", "Added %s %08LX", sens_full_name.c_str(), (uint64_t)unsigned_val1);

				break;
			default:
				signed_val1 = 0;
				for (int cnt = 0; cnt < value_size[sens_idx]; cnt++)
				{
					signed_val1 = (signed_val1 << 8) | data[current_byte_idx];
					current_byte_idx++;
				}
				float_val1 = (float)signed_val1 / value_divider[sens_idx];

				// Limit to 2 decimals
				sprintf(rounding, "%.2f", float_val1);
				sscanf(rounding, "%f", &float_val1);

				sens_full_name = value_name[sens_idx] + "_" + String(sens_num);
				JAddNumberToObject(body, sens_full_name.c_str(), float_val1);
				MYLOG("PARSE", "Added %s %.2f", sens_full_name.c_str(), float_val1);

				break;
			}
			byte_idx = byte_idx + value_size[sens_idx] + 2;
			// MYLOG("PARSE", "Data size %d Position %d", data_len, byte_idx);
			MYLOG("PARSE", ">>>>><<<<<");
		}
		JAddItemToObject(req, "body", body);

		JAddBinaryToObject(req, "payload", data, data_len);

		MYLOG("PARSE", "Finished parsing");
		if (!blues_send_req())
		{
			MYLOG("PARSE", "Send request failed");
			return false;
		}
		return true;
	}
	else
	{
		MYLOG("PARSE", "Error creating body");
	}

	return false;
}
