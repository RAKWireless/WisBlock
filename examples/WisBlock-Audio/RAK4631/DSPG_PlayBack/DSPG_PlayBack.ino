/**
   @file DSPG_PlayBack.ino
   @author rakwireless.com
   @brief This example shows how to run a voice recognition model file on the RAK18080 DSPG board.
    You can choose the voice recognition model file you need to load by modifying the macros DSPG_USE_MODEL
    and COMMAND_GROUP_CHOOSE. DSPG_USE_MODEL can be MODEL_GROUP1, MODEL_GROUP2, MODEL_GROUP3 or MODEL_GROUP4,
    the value of COMMAND_GROUP_CHOOSE must correspond to the value of DSPG_USE_MODEL,
    for example: if select MODEL_GROUP1, "#define DSPG_USE_MODEL MODEL_GROUP1" then "#define COMMAND_GROUP_CHOOSE 1",
                 if select MODEL_GROUP2, "#define DSPG_USE_MODEL MODEL_GROUP2" then "#define COMMAND_GROUP_CHOOSE 2".
    All voice command words are printed on the serial port after booting.
    When the DSPG recognizes the voice wake word "Hey RAK Star", "Hey Helium" or "Hey RAK Cloud", DSPG will play a prompt sound.
    When the DSPG recognizes a voice command word, the state of the two LED indicators on the WisBase flips once.
   @note This example need use the RAK18080、RAK18003 and RAK18060 modules.
   @version 0.1
   @date 2022-09-06
   @copyright Copyright (c) 2022
*/
#include <Arduino.h>
#ifdef NRF52_SERIES
#include <Adafruit_TinyUSB.h>
#endif
#include "audio.h" // Click here to install the library: http://librarymanager/All#RAKwireless-Audio
#include "sound.h"

#define DSPG_USE_MODEL  MODEL_GROUP4  //select voice model can be MODEL_GROUP1、MODEL_GROUP2、MODEL_GROUP3、MODEL_GROUP4
#define COMMAND_GROUP_CHOOSE  4   //the number must be the same as the number of group selected,eg: 1 MODEL_GROUP1; 2 MODEL_GROUP2; 3 MODEL_GROUP3; 4 MODEL_GROUP4

TAS2560 AMP_Left;
TAS2560 AMP_Right;

#define AMP_LEFT_ADDRESS    0x4c    //amplifier i2c address
#define AMP_RIGTT_ADDRESS   0x4f    //amplifier i2c address

QueueHandle_t integerQueue;

#define I2S_DATA_BLOCK_WORDS    512
uint32_t *p_word = NULL;
uint32_t writebuff[I2S_DATA_BLOCK_WORDS] = {0};
bool repeat_play = true;
volatile uint8_t tx_flag = 1;

char cmd_string[60] = "";
int cmd_id = 0;
uint32_t trigger_count = 0;
volatile uint8_t int_flag = 0;
uint8_t led_state = 0;

void tx_irq();
void EventProcess(void);
void speaker_task(void *pvParameters);  // This is a task.

void setup() {
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, LOW);
  delay(1000);
  digitalWrite(WB_IO2, HIGH);
  delay(500);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN, LOW);
  pinMode(LED_BLUE, OUTPUT);
  digitalWrite(LED_BLUE, LOW);

  time_t timeout = millis();
  Serial.begin(115200);
  while (!Serial)
  {
    if ((millis() - timeout) < 2000)
    {
      delay(100);
    }
    else
    {
      break;
    }
  }

  I2S.TxIRQCallBack(tx_irq);

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
  Serial.println("=====================================");

  DSPG_USER.SetActiveCommandGroup(COMMAND_GROUP_CHOOSE);  //it is necessary choose one voice model group before init DSPG.
  while (DSPG_USER.begin(DSPG_USE_MODEL, sizeof(DSPG_USE_MODEL)))
  {
    Serial.println("Please check !!!");
    delay(1000);
  }
  while (DSPG_USER.micCheck() == 0) //Check if the microphone board is connected on the RAK18003
  {
    Serial.println("There is no microphone board, please check !");
    delay(1000);
  }
  delay(100);
  //config interrupt
  DSPG_USER.detectedCallback(EventProcess);
  DSPG_USER.readIoState();  //read the RAK18003 chip IO state
  Serial.printf("init ok\r\n");
  DSPG_USER.echoCommands(COMMAND_GROUP_CHOOSE);
  int_flag = 0;

  digitalWrite(LED_BLUE, HIGH);
  digitalWrite(LED_GREEN, HIGH);

  integerQueue = xQueueCreate(10, sizeof(int));
  if (!integerQueue)
  {
    Serial.println("creat integer queue failed");
  }
  if (!xTaskCreate(speaker_task, "speaker task", 4096, NULL, 2, 0))
  {
    Serial.println("creat speaker task  failed");
  }
}

void loop()
{
  if (int_flag == 1)
  {
    trigger_count++;
    Serial.printf("***************** %d\r\n", trigger_count);
    uint16_t pin_state = DSPG_USER.readIoState();   //this is necessary to read IO expander register
    //    Serial.printf("pinstate:%04X\r\n", pin_state);
    DSPG_USER.eventCallBack(cmd_string, &cmd_id);
    Serial.printf("CMD ID: %d\r\n", cmd_id);
    Serial.println(cmd_string);
    led_state = !led_state;
    digitalWrite(LED_BLUE, led_state);
    digitalWrite(LED_GREEN, led_state);
    int id_case = cmd_id;
    //    play_task();
    xQueueSend(integerQueue, &id_case, portMAX_DELAY);
    int_flag = 0; //clear the interrupt flag
  }
}
void EventProcess(void)
{
  int_flag = 1;
}
void tx_irq()  ///< Pointer to the buffer with data to be sent.
{
  tx_flag = 1;
  I2S.write(&writebuff, sizeof(writebuff));
}
void play_task()
{
  int data_pos = 0;
  int audio_length = sizeof(sound_buff) / 4;
  while (data_pos < audio_length)
  {
    if (tx_flag == 1)
    {
      tx_flag = 0;
      for (int i = 0; i < I2S_DATA_BLOCK_WORDS; i++)
      {
        p_word = &writebuff[i];
        int pos = data_pos << 1; //=data_pos*2
        if ((pos + 4) < sizeof(sound_buff))
        {
          ((uint8_t *)p_word)[0]  = sound_buff[pos];    //left channel data
          ((uint8_t *)p_word)[1]  = sound_buff[pos + 1]; //left channel data

          ((uint8_t *)p_word)[2]  = sound_buff[pos];  //right channel data
          ((uint8_t *)p_word)[3]  = sound_buff[pos + 1]; //right channel data
        }
        else
        {
          ((uint8_t *)p_word)[0]  = 0;  //left channel data
          ((uint8_t *)p_word)[1]  = 0;  //left channel data

          ((uint8_t *)p_word)[2]  = 0;  //right channel data
          ((uint8_t *)p_word)[3]  = 0;  //right channel data
        }
        data_pos++;

      }//for
      //       I2S.write(&writebuff,sizeof(writebuff));
    }// tx_flag
  }// while

  memset(writebuff, 0, sizeof(writebuff));
}

void speaker_task(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  int cmd_id = 0;

  uint16_t write_buff[2] = {0};
  int writeByte;

  for (;;) // A Task shall never return or exit.
  {
    if (xQueueReceive(integerQueue, &cmd_id, portMAX_DELAY) == pdPASS)
    {
      if ((cmd_id >= 1001) && (cmd_id <= 1003))
      {
        play_task();
      }
    }
  }
}
