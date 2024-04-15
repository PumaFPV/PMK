#ifndef CONFIGHANDLE_H
#define CONFIGHANDLE_H

#include "variables.h"
#include "fsHandle.h"
#include "config.h"



char systemVolumeInformation[32] = "System Volume Information";



uint8_t getNumberOfDevices()
{
    uint8_t numberOfDevices = 0;

    File32 root = fatfs.open("/");
    File32 directory = root.openNextFile();
    while(directory)
    {
      char directoryName[32]; //At least 26 needed for System Volume Information
      directory.getName(directoryName, sizeof(directoryName));
        if(directory.isDirectory() && strcmp(directoryName, systemVolumeInformation))
        {
            Serial.printf("Directory: %s \r\n", directoryName);
            numberOfDevices++;
        }
        directory = root.openNextFile();
    }

    return numberOfDevices;
}



const char* getAttribute(const char* filename, const char* attributeName) {
  File32 file = fatfs.open(filename, O_READ);
  if (!file) {
    Serial.printf("Failed to open file for reading\r\n");
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
    Serial.printf("Failed to add peer\r\n");
  }
  else
  {
    Serial.printf("Successfully added %s to ESP NOW list\r\n", filename);
  }
}



void loadConfig(const char* filename, uint8_t deviceID, uint8_t layerID)
{
  Serial.printf("Loading config for %s, deviceID: %u, layer ID: %u\r\n", filename, deviceID, layerID);
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

  JsonArray layerJsonArray = doc["Layer"];
  if(layerJsonArray.size() != 32) 
  {
    Serial.println("Invalid layer config size");
    return;
  }

  uint8_t layerArray[32];
  for(int i = 0; i < 32; i++) 
  {
    String hexStr = layerJsonArray[i].as<String>();
    layerArray[i] = strtoul(hexStr.c_str(), NULL, 16);
  }

  for(uint8_t i = 0; i < 32; i++)
  {
    keyChar[deviceID-1][layerID-1][i] = layerArray[i];
    Serial.printf("Loading %u\r\n", layerArray[i]);
  }

}


#endif