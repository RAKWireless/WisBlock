/**
   @file RAK13003_GPIO_Expander_Interrupt_MCP32.ino
   @author rakwireless.com
   @brief Use IIC to expand 16 GPIO.
          Can change the interrupt trigger mode.
          Trigger an interrupt when the pin is taken to ground.
          The interrupt generated on port PA will trigger the interrupt on WisBlock IO6.
          The interrupt generated on port PB will trigger the interrupt on WisBlock IO5.
   @version 0.1
   @date 2021-2-24
   @copyright Copyright (c) 2021
**/

#include <Wire.h>
#include "Adafruit_MCP23017.h" //http://librarymanager/All#Adafruit_MCP23017

#ifdef ESP32
  #define LED1 WB_LED1
  #define LED2 WB_LED2 
#else
  #define LED1 PIN_LED1
  #define LED2 PIN_LED2   
#endif

Adafruit_MCP23017 mcp;

volatile boolean g_awake_by_InterruptA = false;
volatile boolean g_awake_by_InterruptB = false;

void setup()
{
  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, 1);
  
  // Reset device
  pinMode(WB_IO4, OUTPUT);
  digitalWrite(WB_IO4, 1);
  delay(10);
  digitalWrite(WB_IO4, 0);
  delay(10);
  digitalWrite(WB_IO4, 1);
  delay(10);
  
  pinMode(WB_IO6,INPUT);  // Connect with MCP23017 INTA.
  pinMode(WB_IO5,INPUT);  // Connect with MCP23017 INTB.

  pinMode(LED1, OUTPUT);  // LED1.
  pinMode(LED2, OUTPUT);  // LED2.
  
  // Initialize Serial for debug output
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
  
  Serial.println("MCP23017 GPIO Interrupt Test.");

  mcp.begin(); // use default address 0.
   
  mcp.setupInterrupts(false,false,LOW); // INTs will be signaled with a LOW.

  for(int i=0 ;i < 16 ;i++)
  {
    mcp.pinMode(i, INPUT);
    mcp.pullUp(i, HIGH);  // Turn on a 100K pullup internally.
    mcp.setupInterruptPin(i,FALLING); // interrupt will triger when the pin is taken to ground.
  }

  attachInterrupt(WB_IO6,INTACallBack,CHANGE);
  attachInterrupt(WB_IO5,INTBCallBack,CHANGE);  // Enable interrupts.
}

/***************************************************************************************
* @brief    The int handler will just signal that the int has happen.
*           We will do the work from the main loop.
* @return   NULL.
* @param    NULL.
***************************************************************************************/
void INTACallBack()
{
  g_awake_by_InterruptA=true;
}

/***************************************************************************************
* @brief    The int handler will just signal that the int has happen.
*           We will do the work from the main loop.
* @return   NULL.
* @param    NULL.
***************************************************************************************/
void INTBCallBack()
{
  g_awake_by_InterruptB=true;
}

/***************************************************************************************
* @brief    Handle the interrupt generated on the PA port. 
*           Blink LED1 and serial output the pin number triggered by the interrupt.
* @return   NULL.
* @param    NULL.
***************************************************************************************/
void Handle_InterruptA()
{ 
  uint8_t pin=mcp.getLastInterruptPin();  // Get interrupt pin number.

  if(pin < 8) 
  {
    Serial.printf("MCP23017 Interrupt Pin is : PA%d.\r\n",pin);
    digitalWrite(LED1,HIGH);
    delay(500);
    digitalWrite(LED1,LOW);  // LED1 flashe to indicate that the PA port triggers an interrupt.
  }
  
  // We have to wait for the interrupt condition to finish.
  // Otherwise we might go to sleep with an ongoing condition and never wake up again.
  // As, an action is required to clear the INT flag, and allow it to trigger again.
  // See datasheet for datails.
  while( !(mcp.digitalRead(pin)));

  g_awake_by_InterruptA=false; // Clean queued INT signal
}

/***************************************************************************************
* @brief    Handle the interrupt generated on the PB port. 
*           Blink LED2 and serial output the pin number triggered by the interrupt.
* @return   NULL.
* @param    NULL.
***************************************************************************************/
void Handle_InterruptB()
{
  uint8_t pin=mcp.getLastInterruptPin();  // Get interrupt pin number.

  if(pin < 16)
  {
    Serial.printf("MCP23017 Interrupt Pin is : PB%d.\r\n",pin-8);
    digitalWrite(LED2,HIGH);
    delay(500);
    digitalWrite(LED2,LOW);  // LED2 flashe to indicate that the PB port triggers an interrupt.
  }
  
  // We have to wait for the interrupt condition to finish.
  // Otherwise we might go to sleep with an ongoing condition and never wake up again.
  // As, an action is required to clear the INT flag, and allow it to trigger again.
  // See datasheet for datails.
  while( !(mcp.digitalRead(pin)));
  
  g_awake_by_InterruptB=false; // Clean queued INT signal
}

void loop()
{
  if(digitalRead(WB_IO5) == LOW) // Read GPIO to clear previous interrupts.
  {
    mcp.digitalRead(8);
    mcp.digitalRead(9);
    mcp.digitalRead(10);
    mcp.digitalRead(11);
    mcp.digitalRead(12);
    mcp.digitalRead(13);
    mcp.digitalRead(14);
    mcp.digitalRead(15);
  }
  if(digitalRead(WB_IO6) == LOW) // Read GPIO to clear previous interrupts.
  {
    mcp.digitalRead(0);
    mcp.digitalRead(1);
    mcp.digitalRead(2);
    mcp.digitalRead(3);
    mcp.digitalRead(4);
    mcp.digitalRead(5);
    mcp.digitalRead(6);
    mcp.digitalRead(7);
  }

  while((!g_awake_by_InterruptA) && (!g_awake_by_InterruptB));  // Simulate a deep sleep.
  
  if(g_awake_by_InterruptA) 
  {
    Handle_InterruptA(); // Handling PA port interrupt.
  }
  if(g_awake_by_InterruptB) 
  {
    Handle_InterruptB(); // Handling PB port interrupt.
  }
}
