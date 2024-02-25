#ifndef CONFIGHANDLE_H
#define CONFIGHANDLE_H

#include "variables.h"
#include "fsHandle.h"

uint8_t getNumberOfDevices()
{
    uint8_t numberOfDevices = 0;

    File32 root = fatfs.open("/");
    File32 directory = root.openNextFile();
    while(directory)
    {
        if(directory.isDirectory())
        {
            numberOfDevices++;
        }
        directory = root.openNextFile();
    }

    return numberOfDevices;
}

void configureKeyboard()
{

}

const char* getAttribute(const char* filename, const char* attributeName) {
  File32 file = fatfs.open(filename, O_READ);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return 0;
  }

  size_t size = file.size();
  std::unique_ptr<char[]> buf(new char[size]);

  file.readBytes(buf.get(), size);
  file.close();

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, buf.get());
  if (error) {
    Serial.println("Failed to parse JSON");
    return 0;
  }

  const char* value = doc[attributeName].as<const char*>();
  return value;
}

void addDeviceAddress(const char* filename) 
{
  File32 file = fatfs.open(filename, O_READ);
  if(!file) 
  {
    Serial.println("Failed to open file for reading");
    return;
  }

  size_t size = file.size();
  std::unique_ptr<char[]> buf(new char[size]);

  file.readBytes(buf.get(), size);
  file.close();

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, buf.get());
  if(error) 
  {
    Serial.println("Failed to parse JSON");
    return;
  }

  JsonArray addressJsonArray = doc["deviceAddress"];
  if(addressJsonArray.size() != 6) 
  {
    Serial.println("Invalid device address size");
    return;
  }

  uint8_t addressArray[6];
  for(int i = 0; i < 6; i++) 
  {
    String hexStr = addressJsonArray[i].as<String>();
    addressArray[i] = strtoul(hexStr.c_str(), NULL, 16);
  }
  Serial.printf("Device MAC address: %02X:%02X:%02X:%02X:%02X:%02X\r\n", addressArray[0],addressArray[1], addressArray[2], addressArray[3], addressArray[4], addressArray[5]);
  memcpy(peerInfo.peer_addr, addressArray, 6);

  if(esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.printf("Failed to add peer ");
  }
  Serial.printf("\r\n");
}

#endif