#include "Arduino.h"
#include "esp_now.h"


void espnowLoop()
{
  //Send all pressed keys to packet
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(dongleAddress, (uint8_t *) &keyboardPacket, sizeof(keyboardPacket));
   
  if (result == ESP_OK) 
  {
    //Serial.println("Sent with success");
  }
  else 
  {
    Serial.println("Error sending the data");
  }
}


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
  //USBSerial.print("Bytes received: ");
  //USBSerial.println(len);

}

