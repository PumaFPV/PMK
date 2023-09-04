#include <Arduino.h>

#include "WiFi.h"
#include "esp_now.h"

#include "variables.h"

#include "gpioHandle.h"
#include "ledHandle.h"
#include "espNowHandle.h"

#include "pmk.h"


//USBHIDMouse Mouse;
mouseStruct mousePacket;

void loopCount();
bool ledState = 0;

void setup() 
{
  keyboardPacket.deviceID = 2;

  //Start Serial port for debugging. 
  Serial.begin(9600); 

  //Mouse.begin();
  //USB.begin();

  ledSetup();
  pinMode(9, OUTPUT);

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
  digitalWrite(9, ledState);

  ledState =! ledState;

  Serial.println(ledState);
  delay(1000);

  if(micros() - gpioTask.beginTime >= gpioTask.interval)
  {
    gpioTask.beginTime = micros();
    gpioTask.inBetweenTime = gpioTask.beginTime - gpioTask.endTime;

      trackballLeftCurrentState  = digitalRead(PIN_TRACKBALL_LEFT);
      trackballRightCurrentState = digitalRead(PIN_TRACKBALL_RIGHT);
      trackballUpCurrentState    = digitalRead(PIN_TRACKBALL_UP);
      trackballDownCurrentState  = digitalRead(PIN_TRACKBALL_DOWN);

      //Check for button click. If present, print to Serial monitor.
      trackballButtonCurrentState = digitalRead(PIN_TRACKBALL_BUTTON);

    gpioTask.endTime = micros();
    gpioTask.counter++;
    gpioTask.duration = gpioTask.endTime - gpioTask.beginTime;
  }

  if(micros() - trackballTask.beginTime >= trackballTask.interval)
  {
    trackballTask.beginTime = micros();
    trackballTask.inBetweenTime = trackballTask.beginTime - trackballTask.endTime;

      //Deals with movement
      if (trackballLeftPreviousState != trackballLeftCurrentState)
      {
        Serial.println("Left");
        xPosition= --xPosition;
        trackballLeftPreviousState = trackballLeftCurrentState;
      }
      if (trackballRightPreviousState != trackballRightCurrentState)
      {
        Serial.println("Right");
        xPosition= ++xPosition;
        trackballRightPreviousState = trackballRightCurrentState;
      }
      if (trackballUpPreviousState != trackballUpCurrentState)
      {
        Serial.println("Up");
        yPosition= --yPosition;
        trackballUpPreviousState = trackballUpCurrentState;
      }
      if (trackballDownPreviousState != trackballDownCurrentState)
      {
        Serial.println("Down");
        yPosition= ++yPosition;
        trackballDownPreviousState = trackballDownCurrentState;
      }

      xDistance = xPosition * range;
      yDistance = yPosition * range;

    trackballTask.endTime = micros();
    trackballTask.counter++;
    trackballTask.duration = trackballTask.endTime - trackballTask.beginTime;
  }


  if(micros() - pmkTask.beginTime >= pmkTask.interval)
  {
    pmkTask.beginTime = micros();
    pmkTask.inBetweenTime = pmkTask.beginTime - pmkTask.endTime;

      mousePacket.x = xDistance;
      mousePacket.y = yDistance;
      mousePacket.key = trackballButtonCurrentState;
      xPosition = 0;
      yPosition = 0;
      xDistance = 0;
      yDistance = 0;
      esp_err_t result = esp_now_send(dongleAddress, (uint8_t *) &mousePacket, sizeof(mousePacket));
        
      if (result == ESP_OK) 
      {
        //Serial.println("Sent with success");
      }
      else 
      {
        Serial.println("Error sending the data");
      }
      
      /*
        // if the mouse button is pressed:
      if (trackballButtonCurrentState == LOW) {
        // if the mouse is not pressed, press it:
        if (!Mouse.isPressed(MOUSE_LEFT)) {
          Mouse.press(MOUSE_LEFT);
        }
      }
      // else the mouse button is not pressed:
      else {
        // if the mouse is pressed, release it:
        if (Mouse.isPressed(MOUSE_LEFT)) {
          Mouse.release(MOUSE_LEFT);
        }
      }
  
      // if X or Y is non-zero, move:
      if ((xDistance != 0) || (yDistance != 0)) {
        Serial.print(xDistance);
        Serial.print(" ");
        Serial.print(yDistance);
        Serial.println();
        Mouse.move(xDistance, yDistance, 0);
        xPosition = 0;
        yPosition = 0;
        xDistance = 0;
        yDistance = 0;
      }
      */
    pmkTask.endTime = micros();
    pmkTask.counter++;
    pmkTask.duration = pmkTask.endTime - pmkTask.beginTime;

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

  if(trackballTask.counter == 0)
  {
    trackballTask.startCounterTime = micros();
  }
  if(micros() - trackballTask.startCounterTime > 1000000)
  {
    trackballTask.frequency = trackballTask.counter;
    //Serial.println(trackballTask.counter);
    trackballTask.counter = 0;
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