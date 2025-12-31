#ifndef ESPNOWHANDLE_H
#define ESPNOWHANDLE_H



#include "Arduino.h"
#include "esp_now.h"
#include "uartHandle.h"


// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) 
{
  //Serial.printf(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success\r\n" : "Delivery Fail\r\n");
}



// Callback when data is received
void OnDataRecv(const esp_now_recv_info_t * esp_now_info, const uint8_t *incomingData, int len)
{
  uint8_t srcAddress = *esp_now_info->src_addr;
  uint8_t destAddress = *esp_now_info->des_addr;
  wifi_pkt_rx_ctrl_t espNowPacketInfo = *esp_now_info->rx_ctrl;
  memcpy(&receivedData, incomingData, sizeof(receivedData));
}



void espnowLoop()
{

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