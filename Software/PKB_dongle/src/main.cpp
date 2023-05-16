#include <Arduino.h>

#define LED 9

#include "WiFi.h"
#include "esp_now.h"
#include "USB.h"
#include "USBHIDKeyboard.h"
//#include "USBMSC.h"
//#include "FirmwareMSC.h"
//#include "FS.h"
//#include "LittleFS.h"



#include "variables.h"
//USBMSC MSC;
//FirmwareMSC MSC_Update;


enum devices{
  leftKeyboard,
  rightKeyboard,
};

const uint8_t connectedDevices = 2;
#define MAC_ADDRESS_SIZE 6

uint8_t deviceAddress[connectedDevices][MAC_ADDRESS_SIZE] = 
{
  {0x84, 0xF7, 0x03, 0xF0, 0xF0, 0xBE}, //leftKeyboard 84:F7:03:F0:F0:BE
  {0x84, 0xF7, 0x03, 0xF0, 0xF0, 0xB8}  //rightKeyboard 84:F7:03:F0:F0:B8
};

//FlashUSB dev;

//char *l1 = "ffat";
//char *l2 = "ffat1";


#include "pmk.h"
//#include "MSCHandle.h"
#include "USBHandle.h"
#include "espNowHandle.h"
//#include "fsHandle.h"

void loopCount();

void setup(){

  Serial.begin(115200);

  //===========================================
  //====================USB====================
  //===========================================

  Keyboard.begin();
  //USB.begin();

  //USB.onEvent(usbEventCallback);

  //MSC_Update.onEvent(usbEventCallback);
  //MSC_Update.begin();
  //MSC.vendorID("ESP32");//max 8 chars
  //MSC.productID("USB_MSC");//max 16 chars
  //MSC.productRevision("1.0");//max 4 chars
  //MSC.onStartStop(onStartStop);
  //MSC.onRead(onRead);
  //MSC.onWrite(onWrite);
  //MSC.mediaPresent(true);
  //MSC.begin(DISK_SECTOR_COUNT, DISK_SECTOR_SIZE);

  //Serial.onEvent(usbEventCallback);
  


  //===================================================
  //====================File System====================
  //===================================================

  //if(!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
  //  USBSerial.println("LITTLEFS Mount Failed");
  //  return;
  //}

  //====================================================
  //====================Wifi/ESP Now====================
  //====================================================

  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  esp_now_register_recv_cb(OnDataRecv);
  
  //Register peer
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  //Add peer    
  for(uint8_t i = 0; i < connectedDevices; i++)
  {
    memcpy(peerInfo.peer_addr, deviceAddress[connectedDevices], 6);

    if (esp_now_add_peer(&peerInfo) != ESP_OK){
      Serial.print("Failed to add peer ");
      for(uint8_t i = 0; i < 6; ++i)
      {
        Serial.println(deviceAddress[connectedDevices][i]);
      }
      return;
    }
  }
}

void loop()
{
  //Receive ESP-NOW packet

  //Figure out which command it is

  //If special key (shift / fn / other) than choose correct kb layout

  //if USB command  -> send 


  //------------------------------------------------------ledTask
  if(micros() - ledTask.beginTime >= ledTask.interval)
  {
    ledTask.beginTime = micros();
    ledTask.inBetweenTime = ledTask.beginTime - ledTask.endTime;

    //**functions

    ledTask.endTime = micros();
    ledTask.counter++;
    ledTask.duration = ledTask.endTime - ledTask.beginTime;

  }

  //------------------------------------------------------espnowTask
  if(micros() - espnowTask.beginTime >= espnowTask.interval)
  {
    espnowTask.beginTime = micros();
    espnowTask.inBetweenTime = espnowTask.beginTime - espnowTask.endTime;

    //**functions

    espnowTask.endTime = micros();
    espnowTask.counter++;
    espnowTask.duration = espnowTask.endTime - espnowTask.beginTime;

  }

  //------------------------------------------------------keyboardTask
  if(micros() - keyboardTask.beginTime >= keyboardTask.interval)
  {
    keyboardTask.beginTime = micros();
    keyboardTask.inBetweenTime = keyboardTask.beginTime - keyboardTask.endTime;

    //**functions

    keyboardTask.endTime = micros();
    keyboardTask.counter++;
    keyboardTask.duration = keyboardTask.endTime - keyboardTask.beginTime;

  }

  //------------------------------------------------------uartTask
  if(micros() - uartTask.beginTime >= uartTask.interval)
  {
    uartTask.beginTime = micros();
    uartTask.inBetweenTime = uartTask.beginTime - uartTask.endTime;

    //**functions

    uartTask.endTime = micros();
    uartTask.counter++;
    uartTask.duration = uartTask.endTime - uartTask.beginTime;

  }



  
}

void loopCount()
{
  //Task frequency counter
  if(ledTask.counter == 0)
  {
    ledTask.startCounterTime = micros();
  }
  if(micros() - ledTask.startCounterTime > 1000000)
  {
    ledTask.frequency = ledTask.counter;
    //Serial.println(ledTask.counter);
    ledTask.counter = 0;
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

  //Task frequency counter
  if(keyboardTask.counter == 0)
  {
    keyboardTask.startCounterTime = micros();
  }
  if(micros() - keyboardTask.startCounterTime > 1000000)
  {
    keyboardTask.frequency = keyboardTask.counter;
    //Serial.println(keyboardTask.counter);
    keyboardTask.counter = 0;
  }

  //Task frequency counter
  if(uartTask.counter == 0)
  {
    uartTask.startCounterTime = micros();
  }
  if(micros() - uartTask.startCounterTime > 1000000)
  {
    uartTask.frequency = uartTask.counter;
    //Serial.println(uartTask.counter);
    uartTask.counter = 0;
  }
}

void uartHandle()
{
  if(Serial.available())
  {
    String macAddressCommand = "macaddress";
    String command = Serial.readString();
    Serial.println(command);
    
    if(command == macAddressCommand)
    {
      Serial.print("Device MAC address is: ");
      Serial.println(WiFi.macAddress());
    }
    else
    {
      Serial.println("Invalid command");
    }
  }
}