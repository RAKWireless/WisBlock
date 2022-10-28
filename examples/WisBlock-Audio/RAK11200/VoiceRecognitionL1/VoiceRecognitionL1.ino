/**
   @file VoiceRecognitionL1.ino
   @author rakwireless.com
   @brief This example shows that use the WisCore and microphone to recognize the specific simple commands.
   @note Running this example requires WisCore written with a Cyberon license.
   First you need select one command group, the group value can be 0-4.
        eg:If selected group 1 need to set "#define  COMMAND_GROUP_CHOOSE     1" in the code.
   The trigger words can be "Hey RAK Star"、"Hey Helium" or "Hey RAK Cloud".
   All command words will be printed on the serial port at the beginning.
   @version 0.1
   @date 2022-06-6
   @copyright Copyright (c) 2022
*/
#include "audio.h" // Click here to install the library: http://librarymanager/All#RAKwireless-Audio
//Audio
SemaphoreHandle_t event_audio = NULL;

//hardcode model in SDK for testing
#include <CDSpotter.h>
#include "RAKwireless_Demo_pack_WithTxt_Lv1.h"
#define DSPOTTER_MODEL           g_lpdwRAKwireless_Demo_pack_WithTxt_Lv1
#define	COMMAND_GROUP_CHOOSE	   4    //group value can be 0-4
#define COMMAND_STAGE_TIMEOUT    6000                    // The minimum recording time in ms when there is no result at command stage.
#define COMMAND_STAGE_REPEAT     1                       // When it is 1, sample code will recognize repeatly at command stage till to timeout.
// Otherwise, it will switch to trigger stage immediately after command recognized.

#define USE_STEREO //ESP32 using mono record audio quality is not enough, set stereo will be better
// default number of output channels
#ifdef USE_STEREO
static const char channels = 2;
#else
static const char channels = 1;
#endif

// default PCM output frequency
static const int frequency = 16000;

// Buffer to read samples into, each sample is 16-bits
short sampleBuffer[256];

// Number of audio samples read
volatile int samplesRead;
//VR
static CDSpotter g_hDSpotter;
int g_nCurrentStage = 0;
int g_nBlink = 0;
int g_nAudioDrop = 0;
uint32_t *g_ptr;//For license

void setup() {
  int nMempoolSize = 0, nErr = -1;
  byte *pMemPool = 0;
  int nActiveCommandGroup = COMMAND_GROUP_CHOOSE;
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_GREEN, LOW);
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
  Serial.println("Before I2S init");
  //Get license from flash
  Audio_Handle.begin();
  g_ptr = Audio_Handle.GetLicenseAddr();
  Serial.println(CDSpotter::GetSerialNumber());
  //DSpotter
  nMempoolSize = CDSpotter::GetMemoryUsage(DSPOTTER_MODEL, 60);
  Serial.print("DSpotter mem usage");
  Serial.println(nMempoolSize);
  pMemPool = (byte*)malloc(sizeof(byte) * nMempoolSize);
  if (!pMemPool)
    Serial.print("allocate DSpotter mempool failed");

  //nErr = g_hDSpotter.Init(g_lpdwLicense,sizeof(g_lpdwLicense),DSPOTTER_MODEL,60,pMemPool,nMempoolSize);
  nErr = g_hDSpotter.Init(g_ptr, LICEENSE_LENGTH * sizeof(uint32_t), DSPOTTER_MODEL, 60, pMemPool, nMempoolSize);
  if (nErr != CDSpotter::Success)
  {
    Serial.print("DSpotter err: ");
    Serial.println(nErr);
    if (nErr == CDSpotter::LicenseFailed)
    {
      Serial.println("License err, please register under device ID for License and paste it in g_lpdwLicense");
      Serial.println(CDSpotter::GetSerialNumber());
    }
  }
  else
  {
    Serial.println("DSpotter init success");
  }

  //Add Set Group model API
  nErr = g_hDSpotter.SetActiveCommandGroup(nActiveCommandGroup);
  if (nErr != CDSpotter::Success) {
    Serial.println("Set active commands group failed, using default");
    nActiveCommandGroup = 1;
  }
  //show command
  for (int nStage = 0; nStage < 2; nStage++)
  {
    char szCommand[64];
    int nID;
    int nGroupChoose;
    if (nStage == 0)
    {
      Serial.println("The list of Trigger words: ");
      nGroupChoose = 0;
    }
    else
    {
      Serial.println("The list of Command words: ");
      nGroupChoose = nActiveCommandGroup;
    }
    for (int i = 0; i < g_hDSpotter.GetCommandCount(nGroupChoose); i++)
    {
      g_hDSpotter.GetCommand(nGroupChoose, i, szCommand, sizeof(szCommand), &nID);
      Serial.print(szCommand);
      Serial.print(" , ID = ");
      Serial.println(nID);
    }
    Serial.println("");
  }
  Serial.println("");
  //set 2 stage timeout
  g_hDSpotter.SetCommandStageProperty(COMMAND_STAGE_TIMEOUT, COMMAND_STAGE_REPEAT == 1);
  //Start VR
  g_hDSpotter.Start();
  Serial.println("Enter Trigger state");

  //Create Audio Task otherwise audio data drop
  if (!xTaskCreate(Audio_task, "Audio", 2048, NULL, 1, 0))
  {
    Serial.println("Ups, that should never happen");
  }
}

void loop() {
  int nRes = -1, nDataSize = 0;
  int nFil, nSG, nScore, nID;
  int nStage = 0;
  short sSample[512];
  char pCommand[64];

  //Serial.println("loop");

  //audio data lost resource not enough to do VR
  if (g_hDSpotter.GetRecordLostCount() > g_nAudioDrop)
  {
    g_nAudioDrop = g_hDSpotter.GetRecordLostCount();
    //Serial.println("drop data");
  }

  //Do VR
  nRes = g_hDSpotter.DoRecognition(&nStage);

  //VR return Key word get keyword info
  if (nRes == CDSpotter::Success)
  {
    Serial.print("Detect ID: ");
    g_hDSpotter.GetRecogResult(&nID, pCommand, sizeof(pCommand), &nScore, &nSG, &nFil);
    Serial.print(pCommand);
    Serial.print(" ");
    Serial.println(nID);
    Serial.print("SG: ");
    Serial.print(nSG);
    Serial.print(" Energy: ");
    Serial.print(nFil);
    Serial.print(" Score: ");
    Serial.println(nScore);
  }
  //Check VR stage
  if (nStage != g_nCurrentStage)
  {
    g_nCurrentStage  = nStage;
    if (nStage == CDSpotter::TriggerStage)
    {
      Serial.println("VR Switch to Trigger Stage");
      digitalWrite(LED_BLUE, LOW);
      digitalWrite(LED_GREEN, LOW);
    }
    else if (nStage == CDSpotter::CommandStage)
    {
      Serial.println("VR Switch to Command Stage");
      digitalWrite(LED_BLUE, HIGH);
      digitalWrite(LED_GREEN, HIGH);
    }
  }

}

void Audio_task(void *pvParameters)
{
  size_t bytesRead = 0;
  //int16_t buffer[512] = {0};
  // Read from the DAC. This comes in as signed data with an extra byte.

  Serial.println("I2S Thread Start");

  if (!PDM.begin(channels, frequency)) {
    Serial.println("Failed to start PDM!");
    while (1) yield();
  }

  samplesRead = sizeof(sampleBuffer) / 2;
  while (1)
  {
    bytesRead = PDM.read((void*)sampleBuffer, sizeof(sampleBuffer));

    if (bytesRead > 0)
    {

      for (int i = 0; i < samplesRead; i++)
        sampleBuffer[i] <<= 2;

#ifdef USE_STEREO
      //need move stereo data to mono
      for (int i = 1; i < samplesRead; i++)
        sampleBuffer[i] = sampleBuffer[2 * i];
      g_hDSpotter.PutRecordData(sampleBuffer, samplesRead / 2);
#else
      g_hDSpotter.PutRecordData(sampleBuffer, samplesRead);
#endif
    }
    //delay(10);
  }

  PDM.end();
}
