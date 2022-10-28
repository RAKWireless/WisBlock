/**
   @file AnalogMicRecordToSD.ino
   @author rakwireless.com
   @brief This example shows how to record sound to SD card.
   First, you need to prepare the hardware module and SD card.
    Step 1: Compile and download the example to RAK11200.
    Part 2: Reset RAK11200 and open the serial port.
    Part 3: When the serial port prints out "Recording started", the microphone starts to collect sound,
            and the default sampling time is 8 seconds.
    Step 4: When the serial port prints "Recording finished", the microphone completes the recording and
            stores the audio data to the SD card.
    Part 5: Insert the SD card into the player to play the WAV format audio file you have recorded,
            the default file name is "record.wav".
   @note This example need use the RAK18040 analog microphone module and RAK18003 module.
   @version 0.1
   @date 2022-06-6
   @copyright Copyright (c) 2022
*/

#include "audio.h" // Click here to install the library: http://librarymanager/All#RAKwireless-Audio
#include <driver/i2s.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

File new_audio_file;
// default number of output channels
static const int channels = 1;
// default PCM output frequency
static const int frequency = 16000;
#define BIT_PER_SAMPLE 16
#define RECORD_TIME    8    //recording time default 8 seconds

NAU85L40B   MIC(0x1D);

TPT29555   Expander1(0x23);
TPT29555   Expander2(0x25);

void RAK18003Init(void);
void microphone_record(const char* song_name, uint32_t duration);
bool create_wav_file(const char* song_name, uint32_t duration, uint16_t num_channels, const uint32_t sampling_rate, uint16_t bits_per_sample);
void SD_CS_low();
void SD_CS_high();

void setup() {

  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  delay(500);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_GREEN, LOW);

  // Initialize Serial for debug output
  time_t timeout = millis();
  Serial.begin(115200);
  while (!Serial)
  {
    if ((millis() - timeout) < 1000)
    {
      delay(100);
    }
    else
    {
      break;
    }
  }

  MIC.begin();
  MIC.config(frequency,(MIC_CHANNEL1 | MIC_CHANNEL2),I2S_SAMPLE_16BIT);
  Serial.println("==========Record To SD example============");
  RAK18003Init();

  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_GREEN, LOW);

  SD_CS_low();  //enable the SPI CS
  if (!SD.begin(SS, SPI, 8000000, "/sd", 5)) { // Start access to the SD.
    Serial.println("Card Mount Failed");
  }

  if (SD.exists("/record.wav")) {
    Serial.println("Remove file first!");
    SD.remove("/record.wav");
  }

  // initialize I2S with:
  I2S.begin(channels, frequency,BIT_PER_SAMPLE);
  
  microphone_record("/record.wav", RECORD_TIME);    //the default record file name is "record.wav".
  SD_CS_high(); //disable the SPI CS
}
void loop()
{

}
void RAK18003Init(void)
{
  while (!Expander1.begin())
  {
    Serial.println("Did not find RAK18003 IO Expander Chip1, please check !");
    delay(500);
  }

  while (!Expander2.begin())
  {
    Serial.println("Did not find RAK18003 IO Expander Chip2, please check !");
    delay(500);
  }
  Expander1.pinMode(0, INPUT);    //SD check
  Expander1.pinMode(1, INPUT);    //MIC check
  Expander1.pinMode(2, INPUT);    //MIC CTR1
  Expander1.pinMode(3, INPUT);    //MIC CTR2
  Expander1.pinMode(4, INPUT);    //AMP check
  Expander1.pinMode(5, INPUT);    //AMP CTR1
  Expander1.pinMode(6, INPUT);    //AMP CTR2
  Expander1.pinMode(7, INPUT);    //AMP CTR3
  Expander1.pinMode(8, INPUT);    //DSP check
  Expander1.pinMode(9, INPUT);    //DSP CTR1  DSP int
  Expander1.pinMode(10, INPUT);   //DSP CTR2  DSP ready
  Expander1.pinMode(11, OUTPUT);  //DSP CTR3  DSP reset
  Expander1.pinMode(12, INPUT);   //DSP CTR4  not use
  Expander1.pinMode(13, INPUT);   //DSP CTR5  not use
  Expander1.pinMode(14, INPUT);   //NOT USE
  Expander1.pinMode(15, INPUT);   //NOT USE

  //  Expander1.digitalWrite(14, 0);    //set chip 1 not use pin output low
  //  Expander1.digitalWrite(15, 0);    //set chip 1 not use pin output low

  Expander2.pinMode(0, OUTPUT);  //CORE  SPI CS1 for DSPG CS
  Expander2.pinMode(1, OUTPUT);  //CORE  SPI CS2
  Expander2.pinMode(2, OUTPUT);  //CORE  SPI CS3
  Expander2.pinMode(3, OUTPUT);  //PDM switch CTR    1 to dsp   0 to core
  Expander2.pinMode(4, INPUT);  //not use
  Expander2.pinMode(5, INPUT);  //not use
  Expander2.pinMode(6, INPUT);  //not use
  Expander2.pinMode(7, INPUT);  //not use
  Expander2.pinMode(8, INPUT);  //not use
  Expander2.pinMode(9, INPUT);  //not use
  Expander2.pinMode(10, INPUT); //not use
  Expander2.pinMode(11, INPUT); //not use
  Expander2.pinMode(12, INPUT); //not use
  Expander2.pinMode(13, INPUT); //not use
  Expander2.pinMode(14, INPUT); //not use
  Expander2.pinMode(15, INPUT); //not use

  Expander2.digitalWrite(0, 1);  //set SPI CS1 High
  Expander2.digitalWrite(1, 1);  //set SPI CS2 High
  Expander2.digitalWrite(2, 1);  //set SPI CS3 High

  Expander2.digitalWrite(3, 0);   //set the PDM data direction from MIC to WisCore

  while(Expander1.digitalRead(0) == 1)  //Check SD card
   {
     Serial.println("There is no SD card on the RAK18003 board, please check !");
     delay(500);
   }

//  while (Expander1.digitalRead(1) == 0) //Check if the microphone board is connected on the RAK18003
//  {
//    Serial.println("There is no microphone board, please check !");
//    delay(500);
//  }

  // if(Expander1.digitalRead(4) == 0)  //Check if the RAK18060 AMP board is connected on the RAK18003
  // {
  //   Serial.println("There is no RAK18060 AMP board, please check !");
  // }

  // if(Expander1.digitalRead(8) == 0)  //Check if the RAK18080 DSPG board is connected on the RAK18003
  // {
  //   Serial.println("There is no RAK18080 DSPG board, please check !");
  // }

}

void SD_CS_high()
{
  Expander2.digitalWrite(2, 1);
}
void SD_CS_low()
{
  Expander2.digitalWrite(2, 0);
}
// Create a file and add wav header to it so we can play it from PC later
bool create_wav_file(const char* song_name, uint32_t duration, uint16_t num_channels, const uint32_t sampling_rate, uint16_t bits_per_sample) {
  // data size in bytes - > this amount of data should be recorded from microphone later
  uint32_t data_size = sampling_rate * num_channels * bits_per_sample * duration / 8;

  new_audio_file = SD.open(song_name, FILE_WRITE);
  if (new_audio_file == NULL) {
    Serial.println("Failed to create file");
    return false;
  }

  /* *************** ADD ".WAV" HEADER *************** */
  uint8_t CHUNK_ID[4] = {'R', 'I', 'F', 'F'};
  new_audio_file.write(CHUNK_ID, 4);

  uint32_t chunk_size = data_size + 36;
  uint8_t CHUNK_SIZE[4] = {chunk_size, chunk_size >> 8, chunk_size >> 16, chunk_size >> 24};
  new_audio_file.write(CHUNK_SIZE, 4);

  uint8_t FORMAT[4] = {'W', 'A', 'V', 'E'};
  new_audio_file.write(FORMAT, 4);

  uint8_t SUBCHUNK_1_ID[4] = {'f', 'm', 't', ' '};
  new_audio_file.write(SUBCHUNK_1_ID, 4);

  uint8_t SUBCHUNK_1_SIZE[4] = {0x10, 0x00, 0x00, 0x00};
  new_audio_file.write(SUBCHUNK_1_SIZE, 4);

  uint8_t AUDIO_FORMAT[2] = {0x01, 0x00};
  new_audio_file.write(AUDIO_FORMAT, 2);

  uint8_t NUM_CHANNELS[2] = {num_channels, num_channels >> 8};
  new_audio_file.write(NUM_CHANNELS, 2);

  uint8_t SAMPLING_RATE[4] = {sampling_rate, sampling_rate >> 8, sampling_rate >> 16, sampling_rate >> 24};
  new_audio_file.write(SAMPLING_RATE, 4);

  uint32_t byte_rate = num_channels * sampling_rate * bits_per_sample / 8;
  uint8_t BYTE_RATE[4] = {byte_rate, byte_rate >> 8, byte_rate >> 16, byte_rate >> 24};
  new_audio_file.write(BYTE_RATE, 4);

  uint16_t block_align = num_channels * bits_per_sample / 8;
  uint8_t BLOCK_ALIGN[2] = {block_align, block_align >> 8};
  new_audio_file.write(BLOCK_ALIGN, 2);

  uint8_t BITS_PER_SAMPLE[2] = {bits_per_sample, bits_per_sample >> 8};
  new_audio_file.write(BITS_PER_SAMPLE, 2);

  uint8_t SUBCHUNK_2_ID[4] = {'d', 'a', 't', 'a'};
  new_audio_file.write(SUBCHUNK_2_ID, 4);

  uint8_t SUBCHUNK_2_SIZE[4] = {data_size, data_size >> 8, data_size >> 16, data_size >> 24};
  new_audio_file.write(SUBCHUNK_2_SIZE, 4);

  // Actual data should be appended after this point later
  new_audio_file.close();
  return true;
}
void microphone_record(const char* song_name, uint32_t duration) {

  // Add wav header to the file so we can play it from PC later
  //  if (!create_wav_file(song_name, duration, channels, frequency, BIT_PER_SAMPLE)) {
  //    Serial.println("Error during wav header creation");
  //    return;
  //  }
  //Extract mono data from stereo
  if (!create_wav_file(song_name, duration, 1, frequency, BIT_PER_SAMPLE)) {
    Serial.println("Error during wav header creation");
    return;
  }
  // Buffer to receive data from microphone
  const size_t fifo_size = 1024;
  uint8_t* buf = (uint8_t*)malloc(fifo_size);

  // Open created .wav file in append+binary mode to add PCM data
  File audio_file = SD.open(song_name, FILE_APPEND);
  if (audio_file == NULL) {
    Serial.println("Failed to create file");
    return;
  }
  // data size in bytes - > this amount of data should be recorded from microphone
  uint32_t data_sample = frequency * BIT_PER_SAMPLE * duration / 8;

  // Record until "file_size" bytes have been read from mic.
  uint32_t counter = 0;
  digitalWrite(LED_BLUE, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  Serial.println("Recording started");
  while (counter != data_sample) {
    // Check for file size overflow
    if (counter > data_sample) {
      Serial.println("File is corrupted. data_sample must be multiple of fifo_size. Please modify fifo_size");
      break;
    }
    // Read data from microphone
    int bytes_read = I2S.read(buf, fifo_size);
    if (bytes_read > 0)
    {
      for (int i = 0; i < fifo_size; i++)
      {
        buf[i] = buf[i] << 1; //2x the original volume
      }
    }

    if (channels == 2)
    {
      const size_t write_size = bytes_read >> 1;
      uint8_t* write_buf = (uint8_t*)malloc(write_size);
      for (int i = 0; i < (write_size >> 1); i++) //Convert stereo data to mono
      {
        write_buf[i * 2] = buf[i * 4];
        write_buf[i * 2 + 1] = buf[i * 4 + 1];
      }
      // Save data to SD card
      audio_file.write(write_buf, write_size);
      // Increment the counter
      counter += write_size;
      free(write_buf);
    }
    else
    {
      audio_file.write(buf, fifo_size);
      // Increment the counter
      counter += fifo_size;
    }
  }
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_GREEN, LOW);  
  audio_file.close();
  delay(200);
  Serial.println("Recording finished");
  I2S.end();
  free(buf);
}
