#include <Arduino.h>

#include "WiFi.h"
#include "esp_now.h"
#include "pmk.h"

#include "variables.h"

#include "espNowHandle.h"


void loopCount();


void setup() 
{

  gamepadPacket.deviceID = 5; 

  //-----Serial
  Serial.begin(115200);

  //-----Shift register
  pinMode(ANNULAR_PIN, INPUT_PULLUP);
  pinMode(MIDDLE_PIN, INPUT_PULLUP);
  pinMode(INDEX_PIN, INPUT_PULLUP);
  pinMode(THUMB1_PIN, INPUT_PULLUP);
  pinMode(THUMB2_PIN, INPUT_PULLUP);
  pinMode(THUMB3_PIN, INPUT_PULLUP);
  pinMode(DOWN_PIN, INPUT_PULLUP);
  pinMode(UP_PIN, INPUT_PULLUP);

  //-----Leds


  //-----ESP NOW
  WiFi.mode(WIFI_STA);
  Serial.printf("MAC Address: %s\r\n", WiFi.macAddress());

  if(esp_now_init() != ESP_OK)
  {
    Serial.printf("Error initializing ESP-NOW\r\n");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
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
  esp_now_register_recv_cb(OnDataRecv);

  //Set unused bytes
  gamepadPacket.leftX = -128;
  gamepadPacket.rightX = -128;
  gamepadPacket.rightY = -128;
  gamepadPacket.leftTrigger = -128;
  gamepadPacket.rightTrigger = -128;

}




void loop() 
{

  loopCount();


  //------------------------------------------------------gpioTask
  if(micros() - gpioTask.beginTime >= gpioTask.interval)
  {
    gpioTask.beginTime = micros();
    gpioTask.inBetweenTime = gpioTask.beginTime - gpioTask.endTime;

      gamepadPacket.leftY = map(constrain(analogRead(THROTTLE_PIN), 0, 4095), 0, 4095, -128, 127);
      gamepadPacket.buttons[2] = !digitalRead(THUMB2_PIN) | !digitalRead(THUMB3_PIN) << 1 | !digitalRead(DOWN_PIN) << 2 | !digitalRead(UP_PIN) << 3 | !digitalRead(ANNULAR_PIN) << 4 | !digitalRead(MIDDLE_PIN) << 5 | !digitalRead(INDEX_PIN) << 6 | !digitalRead(THUMB1_PIN) << 7;

    gpioTask.endTime = micros();
    gpioTask.counter++;
    gpioTask.duration = gpioTask.endTime - gpioTask.beginTime;
  }
  
  //send packet
  //------------------------------------------------------espnowTask
  if(micros() - espnowTask.beginTime >= espnowTask.interval)
  {
    espnowTask.beginTime = micros();
    espnowTask.inBetweenTime = espnowTask.beginTime - espnowTask.endTime;
      
    //Send all pressed keys to packet
    //Send message via ESP-NOW
    esp_err_t result = esp_now_send(dongleAddress, (uint8_t *) &gamepadPacket, sizeof(gamepadPacket));
      
    if (result == ESP_OK) 
    {
      //Serial.printf("Sent with success\r\n");
    }
    else 
    {
      //Serial.printf("Error sending the data\r\n");
    }
//------------------
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
    //Serial.printf("%i\r\n", gpioTask.counter);
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
    //Serial.printf("%i\r\n", espnowTask.counter);
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
    Serial.printf("%i\r\n", Task.counter);
    Task.counter = 0;
  }

*/