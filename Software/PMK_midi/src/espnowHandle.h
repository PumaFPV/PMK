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
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len)
{
  memcpy(&receivedData, incomingData, sizeof(receivedData));
}



void espnowLoop()
{
  static bool kb = 0;

  esp_err_t result;

  
  result = esp_now_send(dongleAddress, (uint8_t *) &/*PUT YOUR PACKETHERE*/, sizeof(/*PUT YOUR PACKETHERE*/));
    
  
  if(debug2)
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


#endif