#ifndef ESPNOWHANDLE_H
#define ESPNOWHANDLE_H



#include "Arduino.h"
#include "esp_now.h"
#include "commandHandle.h"


// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) 
{
  //Serial.printf(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success\r\n" : "Delivery Fail\r\n");
}



// Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len)
{
  memcpy(&receivedData, incomingData, sizeof(receivedData));
}



void espnowLoop()
{
  static bool kb = 0;

  esp_err_t result;

  bool knobContentIsEqual = areArraysEqual(knobPacket.knob, previousKnobPacket.knob, 8);

  if(!knobContentIsEqual)
  {
    result = esp_now_send(dongleAddress, (uint8_t *) &knobPacket, sizeof(knobPacket));
  }
    
  for (uint8_t i = 0; i < 8; i++)
  {
    previousKnobPacket.knob[i] = knobPacket.knob[i];
  }
  
  
  
  if(debug[1])
  {
    if (result == ESP_OK) 
    {
      Serial.println("Sent with success");
    }
    else 
    {
      Serial.println("Error sending the data");
    }
  }
}



void registerDongle()
{
  for(uint8_t i = 0; i < 1; i++)  // TODO add support for multiple dongles
  {
    memcpy(peerInfo.peer_addr, dongleAddress, MAC_ADDRESS_SIZE);
    peerInfo.ifidx = WIFI_IF_STA;
    peerInfo.channel = 1;
    peerInfo.encrypt = false;

    // Add dongle
    if(esp_now_add_peer(&peerInfo) != ESP_OK)
    {
      Serial.printf("Failed to add dongle: %i\r\n", i);
      return;
    }
  }
}


#endif