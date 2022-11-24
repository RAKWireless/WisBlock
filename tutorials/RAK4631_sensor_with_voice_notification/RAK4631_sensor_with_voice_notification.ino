/**
   @file RAK4631_sensor_with_voice_notification.ino
   @author ronald.collamar@rakwireless.com and carl.rowan@rakwireless.com
   @brief This code demonstrate how to use the RAKwireless Audio Module to create a sensor with audio notification to users.
   It has a RAK18003 interposer board that host the SD card and also has boart-to-board connector for the speaker amplifier.
   This code has the following features:
   - Run the recorded wav files from the SD card.
   - Indicate when the devices is successfully initiated.
   - Notify the user of the temperature of the sensor reach a upper threshold limit.
   - There is a upper and lower threshold which can be set by the user. The lower threshold is a reset threshold when it will start to notify again.
   - Self-test button that can be used to check if the sensor is still ok and if the speacker is still functional (during idle time in which temperature is in normal condition).
   @version 1.0
   @date 2022-11-25
   @copyright Copyright (c) 2022
**/

#include "Arduino.h"
#include "audio.h"
#include "SPI.h"
#include "SD.h"     //http://librarymanager/All#SD

#include "SparkFun_SHTC3.h"     //Click here to get the library: http://librarymanager/All#SparkFun_SHTC3

SHTC3 g_shtc3;                  // Declare an instance of the SHTC3 class

#define SELF_TEST_BUTTON WB_IO4    // IO4 pin connected to a push button switch connected to GND
#define NOTIFICATION_COUNT 10      // how many times alert will be prompt depending on the user

float temperature_raw = 0;
float humidity_raw = 0;
float temperature_upper_limit = 35;    // setting temperature upper limit; You can set your own limit
float temperature_lower_limit = 33;    // setting temperature lower limit; You can set your own limit
bool notificaiton_done = 0;       // start with notification not done yet
bool sensor_ok = 1;               // assume first sensor is ok

#define I2S_DATA_BLOCK_WORDS    1024
#define DATA_BLOCK_SIZE         (I2S_DATA_BLOCK_WORDS*4)  //4096
//Buffered data bytes
int8_t mDataBytes[DATA_BLOCK_SIZE] = {0};

uint32_t maBufferA[I2S_DATA_BLOCK_WORDS] = {0};
uint32_t maBufferB[I2S_DATA_BLOCK_WORDS] = {0};
uint32_t writebuff[I2S_DATA_BLOCK_WORDS] = {0};

Audio rak_audio;

#define AMP_LEFT_ADDRESS    0x4c    //amplifier i2c address
#define AMP_RIGTT_ADDRESS   0x4f    //amplifier i2c address

TPT29555   Expander1(0x23);
TPT29555   Expander2(0x25);

TAS2560 AMP_Left;
TAS2560 AMP_Right;

File            audiofile;
static uint32_t f_startPos = 44;  //audio data start position
static uint32_t f_readPos = 0;    //read audio data position
static uint32_t f_endPos = 0;     //audio data end position

volatile int play_count = 0;
uint8_t play_flag = 0;
uint32_t *p_word = NULL;
#define WAV_HEAD_LEN 320
typedef struct wavhead_s
{
  /*RIFF chunk*/
  char RIFF_BUF[4];       //（RIFF）  0x46464952
  uint32_t wavFileLen;    //Data size, including header size and audio file size (total file size - 8)
  /*Format Chunk*/
  char WAVEfmt_BUF[8];    //format type（"WAVE"）   0x45564157   //"fmt "  0X20746D66
  uint32_t fmtLen;        //Sizeof(WAVEFORMATEX)
  uint16_t code;          //Encoding format  WAVE_FORMAT_PCM、WAVEFORMAT_ADPCM
  uint16_t channels;      //Number of channels, 1 for mono, 2 for stereo
  uint32_t sampleRate;     //Sampling frequency
  uint32_t bytePerSecond;    //data volume per second
  uint16_t doubleBitsPerSample;    //
  uint16_t singleBitsPerSample;   //
  char DATA_BUF[4];          //"data"
  uint32_t audioDataSize;   //size of audio data
  float playTime;
  uint32_t fileSize;
} wavhead_t;
wavhead_t g_wav_head;

volatile uint8_t tx_flag = 0;
bool mBufferASelected = true;

void SD_CS_low();
void SD_CS_high();
void RAK18003Init(void);
void SD_init(void);
void AMP_init(void);

bool endsWith (const char* base, const char* str);
int lastIndexOf(const char* haystack, const char* needle);
void read_file_header(uint8_t *wav_head, struct wavhead_s *a);
void get_flie_info(void);
void listDir( const char * dirname, uint8_t levels);
bool connectFS(const char* path);
void play_wav(const char* file);
void play_task(void);
void ContinuePlayback();
void tx_irq() ;

void errorDecoder(SHTC3_Status_TypeDef message)   // The errorDecoder function prints "SHTC3_Status_TypeDef" resultsin a human-friendly way
{
  switch (message)
  {
    case SHTC3_Status_Nominal:
      Serial.print("Nominal");
      break;
    case SHTC3_Status_Error:
      Serial.print("Error");
      break;
    case SHTC3_Status_CRC_Fail:
      Serial.print("CRC Fail");
      break;
    default:
      Serial.print("Unknown return code");
      break;
  }
}

void shtc3_read_data(void)
{
  g_shtc3.update();
  if (g_shtc3.lastStatus == SHTC3_Status_Nominal) // You can also assess the status of the last command by checking the ".lastStatus" member of the object
  {
    temperature_raw = g_shtc3.toDegC();               // Packing LoRa data
    humidity_raw = g_shtc3.toPercent();

    Serial.print("RH = ");
    Serial.print(g_shtc3.toPercent());            // "toPercent" returns the percent humidity as a floating point number
    Serial.print("% (checksum: ");

    if (g_shtc3.passRHcrc)                        // Like "passIDcrc" this is true when the RH value is valid from the sensor (but not necessarily up-to-date in terms of time)
    {
      Serial.print("pass");
    }
    else
    {
      Serial.print("fail");
    }

    Serial.print("), T = ");
    Serial.print(g_shtc3.toDegC());               // "toDegF" and "toDegC" return the temperature as a flaoting point number in deg F and deg C respectively
    Serial.print(" deg C (checksum: ");

    if (g_shtc3.passTcrc)                         // Like "passIDcrc" this is true when the T value is valid from the sensor (but not necessarily up-to-date in terms of time)
    {
      Serial.print("pass");

    }
    else
    {
      Serial.print("fail");
    }
    Serial.println(")");
    sensor_ok = 1;
  }
  else
  {
    Serial.print("Update failed, error: ");
    errorDecoder(g_shtc3.lastStatus);
    sensor_ok = 0;
    Serial.println();
  }
}

void setup()
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  pinMode(SELF_TEST_BUTTON, INPUT_PULLUP);
  delay(500);
  // Initialize Serial for debug output
  time_t timeout = millis();
  Serial.begin(115200);
  while (!Serial)
  {
    if ((millis() - timeout) < 3000)
    {
      delay(100);
    }
    else
    {
      break;
    }
  }

  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, HIGH);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_BLUE, HIGH);
  Wire.begin();
  Serial.println("shtc3 init");
  Serial.print("Beginning sensor. Result = "); // Most SHTC3 functions return a variable of the type "SHTC3_Status_TypeDef" to indicate the status of their execution
  errorDecoder(g_shtc3.begin());              // To start the sensor you must call "begin()", the default settings use Wire (default Arduino I2C port)
  Wire.setClock(400000);                      // The sensor is listed to work up to 1 MHz I2C speed, but the I2C clock speed is global for all sensors on that bus so using 400kHz or 100kHz is recommended
  Serial.println();

  if (g_shtc3.passIDcrc)                      // Whenever data is received the associated checksum is calculated and verified so you can be sure the data is true
  { // The checksum pass indicators are: passIDcrc, passRHcrc, and passTcrc for the ID, RH, and T readings respectively
    Serial.print("ID Passed Checksum. ");
    Serial.print("Device ID: 0b");
    Serial.println(g_shtc3.ID, BIN);          // The 16-bit device ID can be accessed as a member variable of the object
  }
  else
  {
    Serial.println("ID Checksum Failed. ");
  }
  Serial.println("=========RAK4631 audio Test========");
  RAK18003Init();
  AMP_init();
  rak_audio.setVolume(18); //The volume level can be set to 0-21
  SD_init();
  I2S.setSize(I2S_DATA_BLOCK_WORDS);
  I2S.TxIRQCallBack(tx_irq);
  Serial.println("=====================================");

  play_wav("config.wav");  // Inserted wav file for "RAK4631 Audio Test"
  delay(2000);
}

void loop()
{
  shtc3_read_data();
  Serial.printf("Temperature = %.2f。C\r\n", temperature_raw);

  if ((temperature_raw > temperature_upper_limit) && (notificaiton_done == 0))
  {
    Serial.println("Temperature high reached");
    uint8_t notification_loop = NOTIFICATION_COUNT;
    while (notification_loop != 0)      //It will loop 10 times since NOTIFICATION_COUNT macro is 10. Can be changed above.
    {
      notification_loop--;
      play_wav("alert.wav");   // Inserted wav file for "Temperature high reached"
      delay(2000);
      notificaiton_done = 1;
    }
  }
  else if (temperature_raw < temperature_lower_limit)
  {
    notificaiton_done = 0;
  }

  if (digitalRead(SELF_TEST_BUTTON) == 0)  //Check if pressed!
  {
    Serial.println("Perform self test!"); 
    play_wav("test.wav");    // Inserted wav file for "Perform Self Test"
    delay(2000);
    if (sensor_ok == 0)
    {
      Serial.println("Device failed self-test!");  
      play_wav("fail.wav");   // Inserted wav file for "Device Failed Self-test"
      delay(2000);
    }
    else
    {
      Serial.println("Device self-test ok!"); 
      play_wav("ok.wav");   // Inserted wav file for "Device Self-test Ok"
      delay(2000);
    }
  }
}
void AMP_init(void)
{
  if (!AMP_Left.begin(AMP_LEFT_ADDRESS))
  {
    Serial.printf("TAS2560 left init failed\r\n");
    delay(500);
  }
  AMP_Left.set_pcm_channel(LeftMode);
  if (!AMP_Right.begin(AMP_RIGTT_ADDRESS))
  {
    Serial.printf("TAS2560 rigth init failed\r\n");
    delay(500);
  }
  AMP_Right.set_pcm_channel(RightMode);
}
void SD_init(void)
{
  SD_CS_low();  //enable the SPI CS
  if (!SD.begin(32000000, SS))
  {
    Serial.println("SD init failed.");
    return; //Punt. We can't work without SD card
  }
  else
  {
    listDir("/", 0);
  }
  Serial.println("SD init completed.");
  SD_CS_high(); //disable the SPI CS
}
void RAK18003Init(void)
{
  while (!Expander1.begin())
  {
    Serial.println("Did not find RAK18003 IO Expander Chip1,please check!");
    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    delay(200);
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_GREEN, LOW);
    delay(200);
  }

  while (!Expander2.begin())
  {
    Serial.println("Did not find RAK18003 IO Expander Chip2,please check!");
    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    delay(200);
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_GREEN, LOW);
    delay(200);
  }
  Expander1.pinMode(0, INPUT);    //SD check
  Expander1.pinMode(1, INPUT);    //MIC check
  Expander1.pinMode(2, OUTPUT);   //MIC CTR1
  Expander1.pinMode(3, OUTPUT);   //MIC CTR2
  Expander1.pinMode(4, INPUT);    //AMP check
  Expander1.pinMode(5, INPUT);    //AMP CTR1  AMP INT  check
  Expander1.pinMode(6, OUTPUT);   //AMP CTR2
  Expander1.pinMode(7, OUTPUT);   //AMP CTR3
  Expander1.pinMode(8, INPUT);    //DSP check
  Expander1.pinMode(9, INPUT);    //DSP CTR1  DSP int
  Expander1.pinMode(10, INPUT);   //DSP CTR2  DSP ready
  Expander1.pinMode(11, OUTPUT);  //DSP CTR3  DSP reset
  Expander1.pinMode(12, OUTPUT);  //DSP CTR4  not use
  Expander1.pinMode(13, OUTPUT);  //DSP CTR5  not use
  Expander1.pinMode(14, OUTPUT);  //NOT USE
  Expander1.pinMode(15, OUTPUT);  //NOT USE

  Expander2.pinMode(0, OUTPUT);  //CORE  SPI CS1   DSP CS
  Expander2.pinMode(1, OUTPUT);  //CORE  SPI CS2
  Expander2.pinMode(2, OUTPUT);  //CORE  SPI CS3
  Expander2.pinMode(3, OUTPUT);  //PDM switch CTR    1 to dsp   0 to core
  Expander2.pinMode(4, OUTPUT);  //not use
  Expander2.pinMode(5, OUTPUT);  //not use
  Expander2.pinMode(6, OUTPUT);  //not use
  Expander2.pinMode(7, OUTPUT);  //not use
  Expander2.pinMode(8, OUTPUT);  //not use
  Expander2.pinMode(9, OUTPUT);  //not use
  Expander2.pinMode(10, OUTPUT); //not use
  Expander2.pinMode(11, OUTPUT); //not use
  Expander2.pinMode(12, OUTPUT); //not use
  Expander2.pinMode(13, OUTPUT); //not use
  Expander2.pinMode(14, OUTPUT); //not use
  Expander2.pinMode(15, OUTPUT); //not use

  Expander2.digitalWrite(3, 0);   //set the PDM data direction from MIC to WisCore

  if (Expander1.digitalRead(4) == 0) //Check if the RAK18060 AMP board is connected on the RAK18003
  {
    Serial.println("There is no RAK18060 AMP board, please check !");
    delay(200);
  }

  if (Expander1.digitalRead(0) == 1) //Check SD card
  {
    Serial.println("There is no SD card on the RAK18003 board, please check !");
    delay(200);
  }
}
void SD_CS_high()
{
  delay(20);
  Expander2.digitalWrite(2, 1);
}
void SD_CS_low()
{
  Expander2.digitalWrite(2, 0);
  delay(20);
}
bool endsWith (const char* base, const char* str)
{
  //fb
  int slen = strlen(str) - 1;
  const char *p = base + strlen(base) - 1;
  while (p > base && isspace(*p)) p--; // rtrim
  p -= slen;
  if (p < base) return false;
  return (strncmp(p, str, slen) == 0);
}
int lastIndexOf(const char* haystack, const char* needle)
{
  //fb
  int nlen = strlen(needle);
  if (nlen == 0) return -1;
  const char *p = haystack - nlen + strlen(haystack);
  while (p >= haystack) {
    int i = 0;
    while (needle[i] == p[i])
      if (++i == nlen) return p - haystack;
    p--;
  }
  return -1;
}
void read_file_header(uint8_t *wav_head, struct wavhead_s *a)
{
  if (NULL == wav_head || NULL == a)
  {
    Serial.printf("%s %d parameter NULL\n", __FUNCTION__, __LINE__);
    return;
  }

  a->RIFF_BUF[0] = wav_head[0];//'R'
  a->RIFF_BUF[1] = wav_head[1];//'I'
  a->RIFF_BUF[2] = wav_head[2];//'F'
  a->RIFF_BUF[3] = wav_head[3];//'F'
  Serial.printf("RIFF_BUF = %c%c%c%c \n", a->RIFF_BUF[0], a->RIFF_BUF[1], a->RIFF_BUF[2], a->RIFF_BUF[3]);

  uint32_t ntemp = 0;
  ntemp = wav_head[7] & 0xff;
  ntemp = (ntemp << 8) | wav_head[6];
  ntemp = (ntemp << 8) | wav_head[5];
  ntemp = (ntemp << 8) | wav_head[4];
  a->wavFileLen = ntemp;
  f_endPos = ntemp + 8;
  a->fileSize = ntemp + 8;
  Serial.printf("wavFileLen = %d\n", a->wavFileLen);

  a->WAVEfmt_BUF[0] = wav_head[8];//'W'
  a->WAVEfmt_BUF[1] = wav_head[9];//'A'
  a->WAVEfmt_BUF[2] = wav_head[10];//'V'
  a->WAVEfmt_BUF[3] = wav_head[11];//'E'
  a->WAVEfmt_BUF[4] = wav_head[12];//'f'
  a->WAVEfmt_BUF[5] = wav_head[13];//'m'
  a->WAVEfmt_BUF[6] = wav_head[14];//'t'
  a->WAVEfmt_BUF[7] = wav_head[15];//''
  Serial.printf("WAVEfmt_BUF = %c%c%c%c%c%c%c%c \n", a->WAVEfmt_BUF[0], a->WAVEfmt_BUF[1],
                a->WAVEfmt_BUF[2], a->WAVEfmt_BUF[3],
                a->WAVEfmt_BUF[4], a->WAVEfmt_BUF[5],
                a->WAVEfmt_BUF[6], a->WAVEfmt_BUF[7]);

  ntemp = 0;
  ntemp = wav_head[19] & 0xff;
  ntemp = (ntemp << 8) | wav_head[18];
  ntemp = (ntemp << 8) | wav_head[17];
  ntemp = (ntemp << 8) | wav_head[16];
  a->fmtLen = ntemp;
  Serial.printf("fmtLen = %d\n", a->fmtLen);

  //1:PCM 6:G711A 7:G711U
  a->code = (wav_head[21] & 0xFF);
  a->code = (a->code << 8) | wav_head[20];
  Serial.printf("code = %d\n", a->code);

  //Number of channels 1: Mono 2: Stereo
  a->channels = (wav_head[23] & 0xFF);
  a->channels = (a->channels << 8) | wav_head[22];
  Serial.printf("channels = %d\n", a->channels);

  /*Sample rate For example: 8000 16000 44100 The sample rate (samples per second)
    indicates the playback speed of each channel */
  ntemp = 0;
  ntemp = wav_head[27] & 0xff;
  ntemp = (ntemp << 8) | wav_head[26];
  ntemp = (ntemp << 8) | wav_head[25];
  ntemp = (ntemp << 8) | wav_head[24];
  a->sampleRate = ntemp;
  Serial.printf("sampleRate = %d\n", a->sampleRate);

  //The amount of audio data per second = sample rate * number of channels * number of samples / 8
  ntemp = 0;
  ntemp = wav_head[31] & 0xff;
  ntemp = (ntemp << 8) | wav_head[30];
  ntemp = (ntemp << 8) | wav_head[29];
  ntemp = (ntemp << 8) | wav_head[28];
  a->bytePerSecond = ntemp;
  Serial.printf("bytePerSecond = %d\n", a->bytePerSecond);

  //The size of each sample = the number of samples * the number of channels / 88
  a->doubleBitsPerSample = (wav_head[33] & 0xFF);
  a->doubleBitsPerSample = (a->doubleBitsPerSample << 8) | wav_head[32];
  Serial.printf("doubleBitsPerSample = %d\n", a->doubleBitsPerSample);

  a->singleBitsPerSample = (wav_head[35] & 0xFF);
  a->singleBitsPerSample = (a->singleBitsPerSample << 8) | wav_head[34];
  Serial.printf("singleBitsPerSample = %d\n", a->singleBitsPerSample);

  uint16_t data_pos = 35;
  while ((data_pos++) && (data_pos < WAV_HEAD_LEN))
  {
    if ((wav_head[data_pos] == 'd') && (wav_head[data_pos + 1] == 'a') && (wav_head[data_pos + 2] == 't') && (wav_head[data_pos + 3] == 'a'))
    {
      f_startPos = data_pos + 8;
      f_readPos = f_startPos;
      audiofile.seek(f_startPos);
      Serial.printf("data block position:%d\r\n", f_startPos);
      break;
    }
    //     Serial.printf("data_pos:%d\r\n",data_pos);
  }

  a->DATA_BUF[0] = wav_head[data_pos];  //'d'
  a->DATA_BUF[1] = wav_head[++data_pos];//'a'
  a->DATA_BUF[2] = wav_head[++data_pos];//'t'
  a->DATA_BUF[3] = wav_head[++data_pos];//'a'
  Serial.printf("Chunk = %c%c%c%c \n", a->DATA_BUF[0], a->DATA_BUF[1], a->DATA_BUF[2], a->DATA_BUF[3]);

  //Length of audio data = total file length - 44 (wav header length)
  ntemp = 0;
  ntemp = wav_head[data_pos + 4] & 0xff;
  ntemp = (ntemp << 8) | wav_head[data_pos + 3];
  ntemp = (ntemp << 8) | wav_head[data_pos + 2];
  ntemp = (ntemp << 8) | wav_head[data_pos + 1];
  a->audioDataSize = ntemp;
  Serial.printf("audioDataSize = %d\n", a->audioDataSize);
  a->playTime = (double)(a->audioDataSize) / (a->bytePerSecond);
  Serial.printf("audio play time = %3.3f S\r\n", a->playTime);
}
void get_flie_info(void)
{
  uint8_t s_wav_head[WAV_HEAD_LEN] = {0};
  audiofile.seek(0x0);  //read file data from first byte
  audiofile.read(s_wav_head, sizeof(s_wav_head));  //read .wav file head
  read_file_header(s_wav_head, &g_wav_head);
}
void listDir( const char * dirname, uint8_t levels)
{
  Serial.printf("Listing directory: %s\n", dirname);
  File root = SD.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(file.name(), levels - 1);
      }
    } else {
      Serial.print("File: ");
      Serial.print(file.name());
      Serial.print("  Size: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}
bool connectFS(const char* path)
{
  if (strlen(path) > 255) return false;
  char audioName[256];
  memcpy(audioName, path, strlen(path) + 1);
  //  Serial.printf("path:%s\r\n", path);
  if (audioName[0] != '/')
  {
    for (int i = 255; i > 0; i--)
    {
      audioName[i] = audioName[i - 1];
    }
    audioName[0] = '/';
  }
  //  Serial.printf("audioName:%s\r\n", audioName);
  if (SD.exists(audioName))
  {
    audiofile = SD.open(audioName);
  }

  if (!audiofile)
  {
    Serial.println("Failed to open file for reading");
    return false;
  }
  char* afn = NULL;  // audioFileName
#ifdef SDFATFS_USED
  audiofile.getName(chbuf, sizeof(chbuf));
  afn = strdup(chbuf);
#else
  afn = strdup(audiofile.name());
#endif

  uint8_t dotPos = lastIndexOf(afn, ".");
  for (uint8_t i = dotPos + 1; i < strlen(afn); i++)
  {
    afn[i] = toLowerCase(afn[i]);
  }

  if (endsWith(afn, ".wav"))
  { // WAVE section
    free(afn);
    return true;
  } // end WAVE section
  audiofile.close();
  if (afn) free(afn);
  return false;
}
void play_wav(const char* file)
{
  char audioName[256];
  memcpy(audioName, file, strlen(file) + 1);
  if (file)
  {
    SD_CS_low();  //enable the SPI
    if (connectFS(audioName))
    {
      get_flie_info();
      if (g_wav_head.channels == 1)
      {
        I2S.begin(Left, g_wav_head.sampleRate, g_wav_head.singleBitsPerSample);
      }
      else
      {
        I2S.begin(Stereo, g_wav_head.sampleRate, g_wav_head.singleBitsPerSample);
      }
      play_task();
    }
    else
    {
      SD_CS_high(); //disable the SPI CS
      Serial.println("fail to open the file");
    }
  }
}
void play_task(void)
{
  //  int16_t sample[I2S_DATA_BLOCK_WORDS*2] = {0};
  SD_CS_low();  //enable the SPI
  delay(10);
  audiofile.seek(f_startPos);
  f_readPos = f_startPos;
  Serial.println("start play");
  I2S.start();
  while (f_readPos < f_endPos)
  {
    ContinuePlayback();
  }
  Serial.println("stop play");
  memset(writebuff, 0, sizeof(writebuff));
  delay(100);
  I2S.stop();
  //  I2S.end();
  audiofile.close();
}
void ContinuePlayback()
{
  if (tx_flag == 1)
  {
    tx_flag = 0;
    int bytes_read = audiofile.read(mDataBytes, DATA_BLOCK_SIZE);
    memcpy(writebuff, mDataBytes, DATA_BLOCK_SIZE);
    for (int i = 0; i < I2S_DATA_BLOCK_WORDS; i++)
    {
      writebuff[i] = rak_audio.Gain(writebuff[i]);
    }
    f_readPos = f_readPos + bytes_read;
  }
}
void tx_irq()  ///< Pointer to the buffer with data to be sent.
{
  tx_flag = 1;
  I2S.write(&writebuff, sizeof(writebuff));
}
