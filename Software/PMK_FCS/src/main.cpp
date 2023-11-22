#include <Arduino.h>

#include "WiFi.h"
#include "esp_now.h"
#include "pmk.h"

#include "variables.h"

#include "espNowHandle.h"



void loopCount();



void setup() 
{

  gamepadPacket.deviceID = 4; 


  //-----Serial
  Serial.begin(115200);


  //-----Shift register
  Serial.printf("Initialising GPIO\r\n");
  pinMode(PINKY_PIN, INPUT_PULLUP);
  pinMode(MIDDLE_PIN, INPUT_PULLUP);
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  pinMode(THUMB_PIN, INPUT_PULLUP);
  pinMode(HAT_UP_PIN, INPUT_PULLUP);
  pinMode(HAT_LEFT_PIN, INPUT_PULLUP);
  pinMode(HAT_RIGHT_PIN, INPUT_PULLUP);
  pinMode(HAT_DOWN_PIN, INPUT_PULLUP);

  //-----Leds


  //-----ESP NOW
  WiFi.mode(WIFI_STA);
  Serial.printf("MAC Address: %s\r\n", WiFi.macAddress());

  if(esp_now_init() != ESP_OK)
  {
    Serial.printf("Error initializing ESP-NOW \r\n");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Transmitted packet
  Serial.printf("Registering OnDataSent CB\r\n");
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, dongleAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.printf("Failed to add peer\r\n");
    return;
  }

  // Register for a callback function that will be called when data is received
  Serial.printf("Registering OnDataRecv CB\r\n");
  esp_now_register_recv_cb(OnDataRecv);

}



void loop() 
{
  loopCount();


  //------------------------------------------------------gpioTask
  if(micros() - gpioTask.beginTime >= gpioTask.interval)
  {
    gpioTask.beginTime = micros();
    gpioTask.inBetweenTime = gpioTask.beginTime - gpioTask.endTime;

      gamepadPacket.y = map(constrain(analogRead(PITCH_PIN), 0, 4095), 0, 4095, -128, 127);
      gamepadPacket.x = map(constrain(analogRead(ROLL_PIN), 0, 4095), 0, 4095, 127, -128);
      gamepadPacket.buttons[0] = !digitalRead(PINKY_PIN) | !digitalRead(MIDDLE_PIN) << 1 | !digitalRead(TRIGGER_PIN) << 2 | !digitalRead(THUMB_PIN) << 3;
      gamepadPacket.hh = 0X00 | !digitalRead(HAT_UP_PIN) << 1 | !digitalRead(HAT_RIGHT_PIN) << 2 | !digitalRead(HAT_DOWN_PIN) << 3 | !digitalRead(HAT_LEFT_PIN) << 4;

    gpioTask.endTime = micros();
    gpioTask.counter++;
    gpioTask.duration = gpioTask.endTime - gpioTask.beginTime;
  }


  //------------------------------------------------------espnowTask
  if(micros() - espnowTask.beginTime >= espnowTask.interval)
  {
    espnowTask.beginTime = micros();
    espnowTask.inBetweenTime = espnowTask.beginTime - espnowTask.endTime;
      
    //Send all pressed keys to packet
    // Send message via ESP-NOW
    esp_err_t result = esp_now_send(dongleAddress, (uint8_t *) &gamepadPacket, sizeof(gamepadPacket));
      
    if (result == ESP_OK) 
    {
      //Serial.printf("Sent with success\r\n");
    }
    else 
    {
      Serial.printf("Error sending the data\r\n");
    }

    espnowTask.endTime = micros();
    espnowTask.counter++;
    espnowTask.duration = espnowTask.endTime - espnowTask.beginTime;

  }
}

void loopCount()
{
  //Task frequency counter

  //gpioTask frequency counter
  if(gpioTask.counter == 0)
  {
    gpioTask.startCounterTime = micros();
  }
  if(micros() - gpioTask.startCounterTime > 1000000)
  {
    gpioTask.frequency = gpioTask.counter;
    //Serial.printf("%i\n\r", gpioTask.counter);
    gpioTask.counter = 0;
  }

  //Task frequency counter
  if(espnowTask.counter == 0)
  {
    espnowTask.startCounterTime = micros();
  }
  if(micros() - espnowTask.startCounterTime > 1000000)
  {
    espnowTask.frequency = espnowTask.counter;
    //Serial.printf("%i\n\r", espnowTask.counter);
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
    Serial.printf("%i\n\r", Task.counter);
    Task.counter = 0;
  }

*/