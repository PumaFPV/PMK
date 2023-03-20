#include "Arduino.h"

#include "WiFi.h"
#include "esp_now.h"

#include "variables.h"

esp_now_peer_info_t peerInfo;

uint8_t receivedData[16];
String success;

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void onDataReceive(const uint8_t * mac, const uint8_t *incomingData, int len);


void espNowSetup()
{
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


// Callback when data is sent
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) 
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
void onDataReceive(const uint8_t * mac, const uint8_t *incomingData, int len) 
{
  memcpy(&receivedData, incomingData, sizeof(receivedData));
  //USBSerial.print("Bytes received: ");
  //USBSerial.println(len);
}

