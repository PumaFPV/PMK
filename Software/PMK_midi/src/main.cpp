/*
 *THIS IS SAMPLE CODE FOR PMK DEVICE
 *IT CONTAINS THE MINIMUM AMOUNT OF CODE NEEDED FOR A PMK DEVICE
 *IT USES ITS OWN "RTOS", IF NEEDED, MAKE SURE YOU IMPLEMENT NEW TASKS ACCORDINGLY
 *YOU CAN SEND AS MANY PACKET TYPES AS YOU WANT
 */

#include <Arduino.h>

#include "SPI.h"
#include "WiFi.h"
#include "esp_now.h"
#include "EEPROM.h"
#include "pmk.h"

#include "variables.h"

#define FIRMWARE_REV "dev"
uint8_t deviceID;

#include "commandHandle.h"
#include "espnowHandle.h"
#include "ledHandle.h"
#include "ioHandle.h"



void loopCount();



void setup() 
{
  //-----Serial
  Serial.begin(115200);



  //-----CLI
  cliSetup();



  //-----CPU
  setCpuFrequencyMhz(80);



  //-----EEPROM
  EEPROM.begin(EEPROM_SIZE);
  deviceID = EEPROM.read(DEVICEID_ADDRESS);
  knobPacket.deviceID = deviceID;

  for(uint8_t i = 0; i < MAC_ADDRESS_SIZE; i++)
  {
    dongleAddress[i] = EEPROM.read(DONGLE_MACADDRESS_ADDRESS + i);
  }



  //-----ESP NOW
  WiFi.mode(WIFI_STA);
  delay(10);
  WiFi.setTxPower(WIFI_POWER_19_5dBm);

  if(esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register dongles
  registerDongle();

  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}




void loop() 
{
  loopCount();



  //------------------------------------------------------
  //------------------------------------------------------ioTask
  //------------------------------------------------------
  if(micros() - ioTask.beginTime >= ioTask.interval)
  {
    ioTask.beginTime = micros();
    ioTask.inBetweenTime = ioTask.beginTime - ioTask.endTime;

      for (uint8_t i = 0; i < 5; i++)
      {
        knobPacket.knob[i] = map(analogRead(sliderPin[i]), 0, 4095, 0, 255);
      }

      if (debug[2])
      {
        for (uint8_t i = 0; i < 5; i++)
        {
          Serial.printf("%u ", analogRead(sliderPin[i]));
        }
        Serial.printf("\r\n");
      }  
      

    ioTask.endTime = micros();
    ioTask.counter++;
    ioTask.duration = ioTask.endTime - ioTask.beginTime;
  }
  


  //------------------------------------------------------
  //------------------------------------------------------espnowTask
  //------------------------------------------------------
  if(micros() - espnowTask.beginTime >= espnowTask.interval)
  {
    espnowTask.beginTime = micros();
    espnowTask.inBetweenTime = espnowTask.beginTime - espnowTask.endTime;

      espnowLoop();
      
    espnowTask.endTime = micros();
    espnowTask.counter++;
    espnowTask.duration = espnowTask.endTime - espnowTask.beginTime;

  }



  //------------------------------------------------------
  //------------------------------------------------------UART Task
  //------------------------------------------------------
  if(micros() - uartTask.beginTime >= uartTask.interval)
  {
    uartTask.beginTime = micros();
    uartTask.inBetweenTime = uartTask.beginTime - uartTask.endTime;

      static char receivedCommand[64];
      static uint8_t i = 0;
      static String command;
      
      uint8_t incominSerial = Serial.read();

      if(incominSerial != 0xFF)
      {
        receivedCommand[i] = incominSerial;
        Serial.printf("%c", incominSerial);
        if(receivedCommand[i] == 0x0D) // 0x0D = \r
        {
          for(uint8_t j = 0; j < i; j++)
          {
            command = command + receivedCommand[j];
          }
          Serial.printf("\r\n");
          cli.parse(command);
          
          i = 0;
          command = "";
        }
        else
        {
          i++;
        }
      }

    uartTask.endTime = micros();
    uartTask.counter++;
    uartTask.duration = uartTask.endTime - uartTask.beginTime;

  }
}



void loopCount()
{
  //srTask frequency counter
  if(ioTask.counter == 0)
  {
    ioTask.startCounterTime = micros();
  }
  if(micros() - ioTask.startCounterTime > 1000000)
  {
    ioTask.frequency = ioTask.counter;
    //Serial.println(srTask.counter);
    ioTask.counter = 0;
  }

  //Task frequency counter
  if(espnowTask.counter == 0)
  {
    espnowTask.startCounterTime = micros();
  }
  if(micros() - espnowTask.startCounterTime > 1000000)
  {
    espnowTask.frequency = espnowTask.counter;
    //Serial.println(espnowTask.counter);
    espnowTask.counter = 0;
  }
}

/* Typical task outline

  //------------------------------------------------------Task
  if(micros() - Task.beginTime >= Task.interval)
  {
    Task.beginTime = micros();
    Task.inBetweenTime = Task.beginTime - Task.endTime;

    **functions

    Task.endTime = micros();
    Task.counter++;
    Task.duration = Task.endTime - Task.beginTime;

  }

  //Task frequency counter
  if(Task.counter == 0)
  {
    Task.startCounterTime = micros();
  }
  if(micros() - Task.startCounterTime > 1000000)
  {
    Task.frequency = Task.counter;
    debug.println(Task.counter);
    Task.counter = 0;
  }

*/
