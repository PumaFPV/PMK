#include <Arduino.h>

#include "WiFi.h"
#include "esp_now.h"
#include "EEPROM.h"
#include "pmk.h"

#include "variables.h"

#define FIRMWARE_REV "dev"
uint8_t deviceID;

#include "uartHandle.h"
#include "gpioHandle.h"
#include "ledHandle.h"
#include "espNowHandle.h"



void loopCount();



void setup() 
{
  //Start Serial port for debugging. 
  Serial.begin(115200);

  Serial.print("Mouse MAC address: " + WiFi.macAddress() + "\r\n");

  //Define pin functionality on the Arduino
  pinMode(PIN_TRACKBALL_BUTTON,    INPUT);
  pinMode(PIN_TRACKBALL_LEFT,      INPUT);
  pinMode(PIN_TRACKBALL_RIGHT,     INPUT);
  pinMode(PIN_TRACKBALL_UP,        INPUT);
  pinMode(PIN_TRACKBALL_DOWN,      INPUT);

  attachInterrupt(PIN_TRACKBALL_UP, upISR, RISING);
  attachInterrupt(PIN_TRACKBALL_DOWN, downISR, RISING);
  attachInterrupt(PIN_TRACKBALL_RIGHT, rightISR, RISING);
  attachInterrupt(PIN_TRACKBALL_LEFT, leftISR, RISING);

  pinMode(PIN_TRACKBALL_LED_WHITE, OUTPUT);
  pinMode(PIN_TRACKBALL_LED_GREEN, OUTPUT);
  pinMode(PIN_TRACKBALL_LED_RED,   OUTPUT);
  pinMode(PIN_TRACKBALL_LED_BLUE,  OUTPUT);

  //Pull LED pins low to prevent flickering
  digitalWrite(PIN_TRACKBALL_LED_WHITE, LOW);
  digitalWrite(PIN_TRACKBALL_LED_GREEN, LOW);
  digitalWrite(PIN_TRACKBALL_LED_RED,   LOW);
  digitalWrite(PIN_TRACKBALL_LED_BLUE,  LOW);  
  
  ledSetup();


  //-----EEPROM
  EEPROM.begin(EEPROM_SIZE);
  deviceID = EEPROM.read(DEVICEID_ADDRESS);
  mousePacket.deviceID = deviceID;

  //-----ESP NOW
  WiFi.mode(WIFI_STA);

  if(esp_now_init() != ESP_OK) 
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(onDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, dongleAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(onDataReceive);
  
}



/*********************Main Loop*************************/
void loop() 
{  

  loopCount();

  if(micros() - gpioTask.beginTime >= gpioTask.interval)
  {
    gpioTask.beginTime = micros();
    gpioTask.inBetweenTime = gpioTask.beginTime - gpioTask.endTime;

      mousePacket.key = !digitalRead(PIN_TRACKBALL_BUTTON);

    gpioTask.endTime = micros();
    gpioTask.counter++;
    gpioTask.duration = gpioTask.endTime - gpioTask.beginTime;
  }

  if(micros() - pmkTask.beginTime >= pmkTask.interval)
  {
    pmkTask.beginTime = micros();
    pmkTask.inBetweenTime = pmkTask.beginTime - pmkTask.endTime;

      //if(deg == 360)
      //{
      //  deg = 0;
      //}
      //mousePacket.x = cos(deg*DEG_TO_RAD)*radius;//(cos(float(deg * DEG_TO_RAD)) - cos(float((deg-1) * DEG_TO_RAD)))*radius;
      //mousePacket.y = sin(deg*DEG_TO_RAD)*radius;//(sin(float(deg* DEG_TO_RAD)) - sin(float((deg-1)*DEG_TO_RAD)))*radius;
      //Serial.printf("deg: %i, x: %i, y: %i\r\n", deg, mousePacket.x, mousePacket.y);
      //deg = deg + 5;

      mousePacket.x = xPosition;
      mousePacket.y = yPosition;
      xPosition = 0;
      yPosition = 0;

      esp_err_t result = esp_now_send(dongleAddress, (uint8_t *) &mousePacket, sizeof(mousePacket));

      if (result == ESP_OK)
      {
        //Serial.println("Sent with success");
      }
      else
      {
        Serial.println("Error sending the data");
      }

    pmkTask.endTime = micros();
    pmkTask.counter++;
    pmkTask.duration = pmkTask.endTime - pmkTask.beginTime;

  }

  //------------------------------------------------------uartTask
  if(micros() - uartTask.beginTime >= uartTask.interval)
  {
    uartTask.beginTime = micros();
    uartTask.inBetweenTime = uartTask.beginTime - uartTask.endTime;

    //**functions
    handleUart();

    uartTask.endTime = micros();
    uartTask.counter++;
    uartTask.duration = uartTask.endTime - uartTask.beginTime;

  }
}



void loopCount()
{
  if(gpioTask.counter == 0)
  {
    gpioTask.startCounterTime = micros();
  }
  if(micros() - gpioTask.startCounterTime > 1000000)
  {
    gpioTask.frequency = gpioTask.counter;
    //Serial.println(gpioTask.counter);
    gpioTask.counter = 0;
  }

  if(pmkTask.counter == 0)
  {
    pmkTask.startCounterTime = micros();
  }
  if(micros() - pmkTask.startCounterTime > 1000000)
  {
    pmkTask.frequency = pmkTask.counter;
    //Serial.println(pmkTask.counter);
    pmkTask.counter = 0;
  }

}


/* Typical task outline

  //Task
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