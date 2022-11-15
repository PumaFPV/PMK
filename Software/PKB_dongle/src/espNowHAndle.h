esp_now_peer_info_t peerInfo;

String success;

uint8_t keyboardPacket[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

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
  memcpy(&keyboardPacket, incomingData, sizeof(keyboardPacket));
  //USBSerial.print("Bytes received: ");
  //USBSerial.println(len);
  for(uint8_t i = 0; i < 8; i++)
  {
    if(keyboardPacket[i] =! 0)
    {
      //USBSerial.println(keyboardPacket[i]);
      //Keyboard.print(keyboardPacket[i]);
    }
  }
}