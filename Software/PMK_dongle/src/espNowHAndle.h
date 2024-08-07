#ifndef ESPNOWHANDLE_H
#define ESPNOWHANDLE_H

#include "Arduino.h"
#include "pmk.h"

esp_now_peer_info_t peerInfo;

String success;

packetStruct receivedPacket;



void handleReceivedPacket(packetStruct receivedPacket)
{
  if(debug1)
  {
    Serial.printf("Received packet from: %i, type :%i \r\n", receivedPacket.deviceID, receivedPacket.packetType);
  }

  if(debug3)
  {
    Serial.printf("RSSI: %u\r\n", WiFi.RSSI());
  }

  switch(receivedPacket.packetType)
  {
    case 255:
      //default packet
      break;
    case 0:
      Serial.printf("Telem Packet\r\n");
      convertPacket2Telemetry(receivedPacket);
      break;
    case 1: //Keyboard
      convertPacket2Keyboard(receivedPacket);
      handleKeyboard(); //measured at 270us
      break;
    case 2: //Mouse
      convertPacket2Mouse(receivedPacket);
      handleMouse();
      break;
    case 3: //GamePad
      convertPacket2Gamepad(receivedPacket);
      handleGamepad();
      break;
    case 4: //LED
      convertPacket2Led(receivedPacket);
      break;
    case 5: //Knob
      convertPacket2Knob(receivedPacket);
      handleKnob();
      break;
    case 6: //Actuator
      convertPacket2Actuator(receivedPacket);
      break;
    case 7: //Display
      convertPacket2Display(receivedPacket);
      break;
    case 8: //Serial
      convertPacket2Serial(receivedPacket);
      break;
    case 9: //SpaceMouse
      convertPacket2SpaceMouse(receivedPacket);
      handleSpaceMouse();
      break;
  }
}



// Callback when data is sent
void OnEspNowDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) 
{
  if((status == 0) && debug1)
  {
    success = "Delivery Success :)";
    Serial.printf("Delivery Success\r\n");
  }
  else
  {
    success = "Delivery Fail :(";
    Serial.printf("Delivery fail\r\n");
  }
}



// Callback when data is received
void OnEspNowDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) 
{
  memcpy(&receivedPacket, incomingData, sizeof(receivedPacket));
  handleReceivedPacket(receivedPacket);
}



#endif