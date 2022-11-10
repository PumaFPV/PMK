#include <Arduino.h>
#include "WiFi.h"
#include "esp_now.h"
#include "USB.h"
#include "USBHIDKeyboard.h"
#include "USBMSC.h"
#include "FirmwareMSC.h"

uint8_t leftKeyboardAddress[] = {0x84, 0xF7, 0x03, 0xF0, 0xEF, 0x72};

USBHIDKeyboard Keyboard;
USBCDC USBSerial;
USBMSC MSC;
FirmwareMSC MSC_Update;


#include "MSCHandle.h"

#include "USBHandle.h"

#include "espNowHandle.h"


void setup() 
{
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

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    USBSerial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, leftKeyboardAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
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

