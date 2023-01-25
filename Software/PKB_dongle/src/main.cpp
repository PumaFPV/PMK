#include <Arduino.h>

#include "WiFi.h"
#include "esp_now.h"

#include "USB.h"
#include "USBHIDKeyboard.h"
#include "USBMSC.h"
#include "FirmwareMSC.h"
#include "FS.h"
#include "LittleFS.h"

USBHIDKeyboard Keyboard;
USBCDC USBSerial;
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


void setup(){

  //===========================================
  //====================USB====================
  //===========================================

  Keyboard.begin();
  USB.begin();

  USB.onEvent(usbEventCallback);

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

  USBSerial.onEvent(usbEventCallback);
  USBSerial.begin();


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
  USBSerial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    USBSerial.println("Error initializing ESP-NOW");
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
      USBSerial.print("Failed to add peer ");
      for(uint8_t i = 0; i < 6; ++i)
      {
        USBSerial.println(deviceAddress[connectedDevices][i]);
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
  while(USBSerial.available() > 0) 
  {
    // read incoming serial data:
    char inChar = USBSerial.read();
    // Type the next ASCII value from what you received:
    Keyboard.write(inChar + 1);
  }

  if(USBSerial.available())
  {
    String macAddressCommand = "macaddress";
    String command = USBSerial.readString();
    USBSerial.println(command);
    
    if(command == macAddressCommand)
    {
      USBSerial.print("Device MAC address is: ");
      USBSerial.println(WiFi.macAddress());
    }
    else
    {
      USBSerial.println("Invalid command");
    }
  }
  
}
