#include "Arduino.h"

esp_now_peer_info_t peerInfo;

String success;

void handleReceivedPacket(packetStruct receivedPacket)
{
  switch(receivedPacket.packetType)
  {
  case 255: 
    //default packet
    break;
  case 0:
    Serial.println("Telem Packet");
    break;
  case 1: //Keyboard
    convertPacket2Keyboard(receivedPacket);
    break;
  case 2: //Mouse
    break;
  } 
}

// Callback when data is sent
void OnEspNowDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) 
{
  //USBSerial.print("\r\nLast Packet Send Status:\t");
  //USBSerial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == 0)
  {
    success = "Delivery Success :)";
  }
  else
  {
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnEspNowDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) 
{
  memcpy(&receivedPacket, incomingData, sizeof(receivedPacket));
  //Serial.println(receivedPacket.data[1]);
}

