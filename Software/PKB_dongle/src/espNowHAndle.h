#include "Arduino.h"

esp_now_peer_info_t peerInfo;

String success;



void handleReceivedPacket(packetStruct receivedPacket)
{
    switch(receivedPacket.packetType)
    {
    case 0:
        Serial.println("Non existant packetType");
        break;
    case 1: //Keyboard
        convertPacket2Keyboard(receivedPacket);
        for(uint8_t i = 0; i < 8; i++)
        {
            if(keyboardPacket.key[i] =! 0)
            {
                //packet2key(keyboardPacket.deviceID, keyboardPacket.key);
                Keyboard.print(keyboardPacket.key[i]);
                
            }
        }
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
  else{
    success = "Delivery Fail :(";
  }
}

// Callback when data is received
void OnEspNowDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&receivedPacket, incomingData, sizeof(receivedPacket));
  
  handleReceivedPacket(receivedPacket);

  
  //USBSerial.print("Bytes received: ");
  //USBSerial.println(len);

}

