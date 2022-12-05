#include "Arduino.h"

esp_now_peer_info_t peerInfo;

String success;



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
  memcpy(&receivedPacket, incomingData, sizeof(receivedPacket));
  
  handleReceivedPacket(receivedPacket);

  
  //USBSerial.print("Bytes received: ");
  //USBSerial.println(len);

}

