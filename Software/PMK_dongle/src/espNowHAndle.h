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
      //Serial.printf("Telem Packet\r\n");
      convertPacket2Telemetry(receivedPacket);
      if(debug6)
      {
        Serial.printf("dId: %u, battery: %u\r\n", telemetryPacket.deviceID, telemetryPacket.battery);
      }
      if(debug4)
      {
        uint32_t calculatedTime = telemetryPacket.error[0] | telemetryPacket.error[1] << 8 | telemetryPacket.error[2] << 16 | telemetryPacket.error[3] << 24;
        Serial.printf(">Received time: %u\r\n", calculatedTime);
      }
      break;
    case 1: //Keyboard
      convertPacket2Keyboard(receivedPacket);
      handleKeyboard(); //measured at 180us
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
  esp_now_unregister_recv_cb();

  memcpy(&receivedPacket, incomingData, sizeof(receivedPacket));
  handleReceivedPacket(receivedPacket);

  esp_now_register_recv_cb(OnEspNowDataRecv);
}



#endif