#include <Arduino.h>

#include "WiFi.h"
#include "esp_now.h"

#include "USB.h"
#include "USBHIDKeyboard.h"
#include "USBMSC.h"
#include "FirmwareMSC.h"

USBHIDKeyboard Keyboard;
USBCDC USBSerial;
USBMSC MSC;
FirmwareMSC MSC_Update;

#include "pmk.h"

#include "FS.h"
#include "LittleFS.h"

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



#include "MSCHandle.h"
#include "USBHandle.h"
#include "espNowHandle.h"
#include "fsHandle.h"


void setup(){

  //char toto[] = "ffat";
  //const char tata[] = "/fat1";
//
  //if (dev.init(tata, toto))
  //{
  //  if (dev.begin())
  //  {
  //    Serial.println("MSC lun 1 begin");
  //  }
  //  else
  //    log_e("LUN 1 failed");
  //}


  Keyboard.begin();
  USB.begin();

  USB.onEvent(usbEventCallback);

  MSC_Update.onEvent(usbEventCallback);
  MSC_Update.begin();
  MSC.vendorID("ESP32");//max 8 chars
  MSC.productID("USB_MSC");//max 16 chars
  MSC.productRevision("1.0");//max 4 chars
  MSC.onStartStop(onStartStop);
  MSC.onRead(onRead);
  MSC.onWrite(onWrite);
  MSC.mediaPresent(true);
  MSC.begin(DISK_SECTOR_COUNT, DISK_SECTOR_SIZE);

  USBSerial.onEvent(usbEventCallback);
  USBSerial.begin();

  if(!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
    USBSerial.println("LITTLEFS Mount Failed");
    return;
  }

  WiFi.mode(WIFI_STA);
  USBSerial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    USBSerial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  // Add peer    
  for(uint8_t i = 0; i < connectedDevices; i++)
  {
    memcpy(peerInfo.peer_addr, deviceAddress[connectedDevices], 6);

    if (esp_now_add_peer(&peerInfo) != ESP_OK){
      USBSerial.println("Failed to add peer");
      return;
    }
  }


  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}

void loop()
{
  //Receive ESP-NOW packet

  //Figure out which command it is

  //If special key (ctrl / shift / other) than choose correct kb layout

  //if USB command  -> send 
  
}