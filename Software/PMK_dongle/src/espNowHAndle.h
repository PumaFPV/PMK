#ifndef ESPNOWHANDLE_H
#define ESPNOWHANDLE_H

#include "Arduino.h"
#include "pmk.h"

esp_now_peer_info_t peerInfo;

String success;

packetStruct receivedPacket;



void handleReceivedPacket(packetStruct receivedPacket)
{
  #ifdef DEBUG
  if(debug[0])
  {
    Serial.printf("Received packet from: %i, type :%i \r\n", receivedPacket.deviceID, receivedPacket.packetType);
  }
  #endif

  #ifdef DEBUG
  if(debug[2])
  {
    Serial.printf("RSSI: %u\r\n", WiFi.RSSI());
  }
  #endif


  switch(receivedPacket.packetType)
  {
    case 255:
      //default packet
      break;
    case 0:
      //Serial.printf("Telem Packet\r\n");
      convertPacket2Telemetry(receivedPacket);
      if(battery)
      {
        Serial.printf("dId: %u, battery: %u\r\n", telemetryPacket.deviceID, telemetryPacket.battery);
      }

      #ifdef DEBUG
      if(debug[3])
      {
        uint32_t calculatedTime = telemetryPacket.error[0] | telemetryPacket.error[1] << 8 | telemetryPacket.error[2] << 16 | telemetryPacket.error[3] << 24;
        Serial.printf(">Received time: %u\r\n", calculatedTime);
      }
      #endif

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
void OnEspNowDataSent(const uint8_t *mac_addr, esp_now_send_status_t status/*const esp_now_send_info_t *tx_info, esp_now_send_status_t status*/) 
{
  //const uint8_t *mac_addr = tx_info->des_addr;
  //Serial.printf(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success\r\n" : "Delivery Fail\r\n");
}



// Callback when data is received
void OnEspNowDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len/*const esp_now_recv_info_t * esp_now_info, const uint8_t *incomingData, int len*/) 
{
  esp_now_unregister_recv_cb();
  
  //uint8_t srcAddress = *esp_now_info->src_addr;
  //uint8_t destAddress = *esp_now_info->des_addr;
  //wifi_pkt_rx_ctrl_t espNowPacketInfo = *esp_now_info->rx_ctrl;
  memcpy(&receivedPacket, incomingData, sizeof(receivedPacket));
  handleReceivedPacket(receivedPacket);

  esp_now_register_recv_cb(OnEspNowDataRecv);
}



#endif