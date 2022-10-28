/**
   @file PlayBackFromSD.ino
   @author rakwireless.com
   @brief This example shows how to play WAV audio files from SD card. The quality of the audio played is 
   related to the quality of the audio source and the speed of the SD card.So this example does not guarantee that you can play high-quality music.
   The volume can be set from 0 to 21, and the appropriate volume can be set according to your speaker situation.
   @note This example need use the RAK18003 and RAK18060 module. And use the battery power for WisBase.
   This example only supports playing audio in WAV format.The default file name to play is "test.wav",you can modify it to the file name you want to play.    
   The library provides some pure music in the "pure music" folder under the "sound" folder for testing.
   @version 0.1
   @date 2022-08-04
   @copyright Copyright (c) 2022
**/
#include "Arduino.h"
#include "audio.h" // Click here to install the library: http://librarymanager/All#RAKwireless-Audio
#include "SPI.h"
#include "SD.h"

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

uint8_t play_flag = 0;

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
bool connectFS(fs::FS &fs, const char* path);
void play_wav(const char* file);
void play_task(void);

void setup()
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
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
  Serial.println("=========RAK11200 audio Test========");
  RAK18003Init();
  AMP_init();
  rak_audio.setVolume(6); //The volume level can be set to 0-21
  SD_init();
  Serial.println("=====================================");
}

void loop()
{
  play_wav("test.wav"); //play test.wav, you can change the file name use your want to play, but only support play WAV format file.
  while (1); //If you need to repeat playback, you can comment out this line.
}
void play_task(void)
{
  int16_t sample[2] = {0};
  SD_CS_low();  //enable the SPI
  delay(10);
  audiofile.seek(f_startPos);
  f_readPos = f_startPos;
  Serial.println("start play");
  while (f_readPos < f_endPos)
  {
    int bytes_read = audiofile.read((uint8_t*)&sample, sizeof(sample));
//    uint32_t s32 = rak_audio.Gain(sample); // vosample2lume;
//    I2S.write((void *) &s32, sizeof(uint32_t));
    I2S.write((void *) &sample, sizeof(sample));
    f_readPos = f_readPos + bytes_read;
  }
  delay(200);
  SD_CS_high(); //disable the SPI CS
  I2S.end();
  audiofile.close();
  Serial.println("stop play");
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
  if (!SD.begin(SS, SPI, 8000000, "/sd", 5)) { // Start access to the SD.
    Serial.println("Card Mount Failed");
  }
  else
  {
    listDir("/", 0);
  }
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

  while (Expander1.digitalRead(4) == 0) //Check if the RAK18060 AMP board is connected on the RAK18003
  {
    Serial.println("There is no RAK18060 AMP board, please check !");
    delay(200);
  }

  while (Expander1.digitalRead(0) == 1) //Check SD card
  {
    Serial.println("There is no SD card on the RAK18003 board, please check !");
    delay(200);
  }
}
void SD_CS_high()
{
  Expander2.digitalWrite(2, 1);
}
void SD_CS_low()
{
  Expander2.digitalWrite(2, 0);
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
  Serial.printf("WAVEfmt_BUF = %c%c%c%c%c%c%c%c \n", a->WAVEfmt_BUF[0], a->WAVEfmt_BUF[1], a->WAVEfmt_BUF[2], a->WAVEfmt_BUF[3], a->WAVEfmt_BUF[4], a->WAVEfmt_BUF[5], a->WAVEfmt_BUF[6], a->WAVEfmt_BUF[7]);

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
bool connectFS(fs::FS &fs, const char* path)
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
  if (fs.exists(audioName))
  {
    audiofile = fs.open(audioName);
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
    delay(100);
    if (connectFS(SD, audioName))
    {
      get_flie_info();
      I2S.setSampleBit(g_wav_head.singleBitsPerSample);
      I2S.begin(g_wav_head.channels, g_wav_head.sampleRate);
      play_task();
    }
    else
    {
      SD_CS_high(); //disable the SPI CS
      Serial.println("fail to open the file");
    }
  }
}
