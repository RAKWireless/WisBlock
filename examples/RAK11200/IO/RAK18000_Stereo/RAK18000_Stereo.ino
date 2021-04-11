/**
 * @file RAK18000_Stereo.ino
 * @author rakwireless
 * @brief  This example reads audio data from the on-board PDM microphones, and prints
 * out the samples to the Serial console. The Serial Plotter built into the
 * Arduino IDE can be used to plot the audio data (Tools -> Serial Plotter)
 * @version 0.1
 * @date 2021-03-7
 *
 * @copyright Copyright (c) 2020
 *
 */
#include <Arduino.h>
#include "esp_now.h"
#include "driver/i2s.h"

void setup()
{
	time_t timeout = millis();
	Serial.begin(115200);
	while (!Serial)
	{
		if ((millis() - timeout) < 5000)
		{
			delay(100);
		}
		else
		{
			break;
		}
	}

	// I2S PDM mode
	Serial.println("Setup I2S...");
	i2s_config_t i2s_config = {
		.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM),
		.sample_rate = 16000, 
		.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,									
		.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,											
		.communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_PCM | I2S_COMM_FORMAT_PCM_SHORT), //(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_PCM_SHORT),//
		.intr_alloc_flags = 0,// ESP_INTR_FLAG_LEVEL1,	
		.dma_buf_count = 4,
		.dma_buf_len = 512,
		.use_apll = true,
		.tx_desc_auto_clear = false,
		.fixed_mclk = 0,
	};
	i2s_pin_config_t pin_config = {
		.bck_io_num = I2S_PIN_NO_CHANGE, // Bit Clock.
		.ws_io_num = 23,		// Word Select.
		.data_out_num = I2S_PIN_NO_CHANGE,
		.data_in_num = 26, // Data-out of the mic.
	};
	if (ESP_OK != i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL))
	{
		Serial.println("i2s_driver_install: error");
	}
	if (ESP_OK != i2s_set_pin(I2S_NUM_0, &pin_config))
	{
		Serial.println("i2s_set_pin: error");
	}
	i2s_set_clk(I2S_NUM_0, 16000, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_STEREO);//
	i2s_stop(I2S_NUM_0);
	i2s_start(I2S_NUM_0);
}

void loop()
{
  size_t bytesRead = 0;
  int16_t buffer[512] = {0};
	// Read from the DAC. This comes in as signed data with an extra byte.
	
	i2s_read(I2S_NUM_0, &buffer, sizeof(buffer), &bytesRead, portMAX_DELAY);
  
	int samplesRead = bytesRead / 2;

    for (int i = 0; i < samplesRead; i++) 
    {
        //Simple filtering, if less than 200, it will be regarded as noise
        if(buffer[i] <= 200 && buffer[i] >= -200)
        {
            buffer[i] = 0;
        } 
        Serial.println(buffer[i]);
    }

	// clear the read count
	samplesRead = 0;
}
