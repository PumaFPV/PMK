#include <Arduino.h>
#include "WiFi.h"
#include "esp_now.h"
#include "USB.h"
#include "FirmwareMSC.h"

FirmwareMSC MSC_Update;

USBCDC USBSerial;

#include "USBHandle.h"
//84:F7:03:F0:F0:BE
uint8_t dongleAddress[] = {0x84, 0xF7, 0x03, 0xF0, 0xF0, 0xBE};
uint8_t receivedData[16];

esp_now_peer_info_t peerInfo;

String success;

uint8_t keyboardPacket[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) 
{
  //USBSerial.print("\r\nLast Packet Send Status:\t");
  //USBSerial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == 0)
  {
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}


// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData));
  USBSerial.print("Bytes received: ");
  USBSerial.println(len);

}


void setup() 
{

  USB.onEvent(usbEventCallback);
  MSC_Update.onEvent(usbEventCallback);
  MSC_Update.begin();
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
  memcpy(peerInfo.peer_addr, dongleAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);

  pinMode(0, INPUT_PULLUP);
  pinMode(15, OUTPUT);
}



void loop() 
{
  //Read SPI from shift register
  uint32_t spiPacket = 0b00000000000000000000000000000000;
  spiPacket = spiPacket | ((!digitalRead(0)) << 5);
  USBSerial.println(spiPacket);

  //Process spiPacket
  uint8_t pos = 0;

  for(uint8_t i = 0; i < 32; i++)
  {
    bool isKeyPressed = spiPacket & (0b1 << i);
    
    if(isKeyPressed && pos < 8)
    {
      keyboardPacket[pos] = i;
      pos++;
    }
    if(pos == 8)
    {
      USBSerial.println("maximum 8 keys pressed");
    }
  }

  //Send all pressed keys to packet
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(dongleAddress, (uint8_t *) &keyboardPacket, sizeof(keyboardPacket));
   
  if (result == ESP_OK) 
  {
    //USBSerial.println("Sent with success");
  }
  else 
  {
    //USBSerial.println("Error sending the data");
  }
  //send packet

  //Deal with LED

  //Deal with other stuff
  //USBSerial.println(getXtalFrequencyMhz());
  //USBSerial.println(getApbFrequency());
  //USBSerial.println(getCpuFrequencyMhz());
}