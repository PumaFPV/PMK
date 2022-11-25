esp_now_peer_info_t peerInfo;

String success;

packetStruct receivedPacket;

keyboardStruct keyboardPacket;
mouseStruct mousePacket;
gamepadStruct gamepadPacket;
ledStruct ledPacket;
knobStruct knobPacket;
actuatorStruct actuatorPacket;
displayStruct displayPacket;
telemetryStruct telemetryPacket;
serialStruct serialPacket;

keyboardStruct  convertPacket2Keyboard(packetStruct packet);
mouseStruct     convertPacket2Mouse(packetStruct packet);
gamepadStruct   convertPacket2Gamepad(packetStruct packet);
ledStruct       convertPacket2Led(packetStruct packet);
knobStruct      convertPacket2Knob(packetStruct packet);
actuatorStruct  convertPacket2Actuato(packetStruct packet);
displayStruct   convertPacket2Display(packetStruct packet);
telemetryStruct convertPacket2Telemetry(packetStruct packet);
serialStruct    convertPacket2Serial(packetStruct packet);

keyboardStruct convertPacket2Keyboard(packetStruct packet)
{
  keyboardPacket.deviceID = packet.deviceID;
  keyboardPacket.key[0] = packet.data[0];
  keyboardPacket.key[1] = packet.data[1];
  keyboardPacket.key[2] = packet.data[2];
  keyboardPacket.key[3] = packet.data[3];
  keyboardPacket.key[4] = packet.data[4];
  keyboardPacket.key[5] = packet.data[5];
  keyboardPacket.key[6] = packet.data[6];
  keyboardPacket.key[7] = packet.data[7];
}

mouseStruct convertPacket2Mouse(packetStruct packet)
{
  mousePacket.deviceID = packet.deviceID;
  mousePacket.x = packet.data[0];
  mousePacket.y = packet.data[1];
  mousePacket.w = packet.data[2];
  mousePacket.p = packet.data[3];
  mousePacket.k = packet.data[4];
}

gamepadStruct convertPacket2Gamepad(packetStruct packet)
{
  gamepadPacket.deviceID = packet.deviceID;
  gamepadPacket.x = packet.data[0];
  gamepadPacket.y = packet.data[1];
  gamepadPacket.
}

ledStruct convertPacket2Led(packetStruct packet)
{

}

knobStruct convertPacket2Knob(packetStruct packet)
{

}

actuatorStruct convertPacket2Actuato(packetStruct packet)
{

}

displayStruct convertPacket2Display(packetStruct packet)
{

}

telemetryStruct convertPacket2Telemetry(packetStruct packet)
{

}

serialStruct convertPacket2Serial(packetStruct packet)
{

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
  memcpy(&receivedPacket, incomingData, sizeof(receivedPacket));
  
  switch(receivedPacket.packetType)
  {
    case 0:
      USBSerial.println("Error on packetType");
      break;
    case 1: //Keyboard
      convertPacket2Keyboard();
      for(uint8_t i = 0; i < 8; i++)
      {
        if(keyboardPacket.key[i] =! 0)
        {
          //USBSerial.println(keyboardPacket[i]);
          //Keyboard.print(keyboardPacket[i]);
        }
      }
      break;
  }
  
  //USBSerial.print("Bytes received: ");
  //USBSerial.println(len);

}