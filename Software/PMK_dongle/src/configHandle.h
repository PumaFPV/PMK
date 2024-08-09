#pragma once

#include "variables.h"
#include "fsHandle.h"
#include "config.h"
#include "pmk.h"
#include "espNowHandle.h"


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



const char* getAttribute(const char* filename, const char* attributeName) 
{
  File32 file = fatfs.open(filename, O_READ);
  if(!file)
  {
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



void loadKeyConfig(const char* filename, uint8_t deviceID, uint8_t layerID)
{
  Serial.printf("Loading Key config for %s, deviceID: %u, layer ID: %u\r\n", filename, deviceID, layerID);
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
    Serial.printf("Failed to parse JSON\r\n");
    return;
  }

  JsonArray layerJsonArray = doc["Layer"];
  
  if(layerJsonArray.size() > MAX_NUMBER_OF_KEYS) 
  {
    Serial.printf("Too many keys configured, not configuring it\r\n");
    return;
  }

  for(uint8_t i = 0; i < layerJsonArray.size(); i++) 
  {
    String hexStr = layerJsonArray[i].as<String>();
    keyChar[deviceID][layerID][i] = strtoul(hexStr.c_str(), NULL, 16);
    //Serial.printf("Loading %u\r\n", keyChar[deviceID-1][layerID][i]);
  }
}



void loadLedConfig(const char* filename, uint8_t deviceID, uint8_t layerID)
{
  Serial.printf("Loading LED config for %s, deviceID: %u, layer ID: %u\r\n", filename, deviceID, layerID);
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
    Serial.printf("Failed to parse JSON\r\n");
    return;
  }

  JsonArray layerJsonArray = doc["Layer"];
  
  if(layerJsonArray.size() > MAX_NUMBER_OF_LEDS) 
  {
    Serial.printf("Too many LED configured, not configuring it\r\n");
    return;
  }

  for(uint8_t i = 0; i < layerJsonArray.size(); i++) 
  {
    String hexStr = layerJsonArray[i].as<String>();
    ledColorProfile[deviceID-1][layerID][i] = strtoul(hexStr.c_str(), NULL, 16);
    //Serial.printf("Loading %u\r\n", keyChar[deviceID-1][layerID][i]);
  }
}



void loadMouseConfig(const char* filename, uint8_t deviceID, uint8_t layerID)
{
  Serial.printf("Loading mouse config from %s, deviceID: %u, layer ID: %u\r\n", filename, deviceID, layerID);
  File32 file = fatfs.open(filename, O_READ);
  if(!file) 
  {
    Serial.println("Failed to open file for reading");
    return;
  }

  dpi[deviceID][layerID] = static_cast<int8_t>(std::atoi(getAttribute(filename, "dpi")));
  Serial.printf("Setting DPI to %i for device: %u layer %u\r\n", dpi[deviceID][layerID], deviceID, layerID);

}



void configDeej()
{
  File32 deejConfigFile = fatfs.open("/deej.json", O_READ);
  if(!deejConfigFile)
  {
    Serial.printf("No deej config file\r\n");
    return;
  }

  size_t size = deejConfigFile.size();
  std::unique_ptr<char[]> buf(new char[size]);

  deejConfigFile.readBytes(buf.get(), size);
  deejConfigFile.close();

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, buf.get());
  if(error) 
  {
    Serial.println("Failed to parse JSON");
    return;
  }

  JsonArray deejJsonArray = doc["deej"];

  Serial.printf("Number of knobs configured for deej: %u\r\n", deejJsonArray.size());
  Serial.printf("Knobs to use:");
  for(int i = 0; i < deejJsonArray.size(); i++) 
  {
    String hexStr = deejJsonArray[i].as<String>();
    knobIDToDeej[i] = strtoul(hexStr.c_str(), NULL, 16);
    Serial.printf(" %u,", knobIDToDeej[i]);
  }
  Serial.printf("\r\n");
}



void configureKeyboard(char* deviceName, char* filePath)
{
  for(uint8_t currentLayer = 0; currentLayer < MAX_NUMBER_OF_LAYERS; currentLayer++)
  {
    char kbLayerFile[64] ="\0";
    char currentLayerChar[1];
    sprintf(currentLayerChar, "%hhu", currentLayer);  // TODO make this clean one day
    strcat(kbLayerFile, "/");
    strcat(kbLayerFile, deviceName);
    strcat(kbLayerFile, "/keyboard/kb-l");
    strcat(kbLayerFile, currentLayerChar);
    strcat(kbLayerFile, ".json");

    File32 layerFolder;
    if(layerFolder.open(kbLayerFile))
    {
      Serial.printf("Successfully opened %s\r\n", kbLayerFile);
      uint8_t deviceID = static_cast<uint8_t>(std::atoi(getAttribute(filePath, "deviceID"))); // TODO make this clean one day
      loadKeyConfig(kbLayerFile, deviceID, currentLayer);
    }
    else
    {
      //Serial.printf("Couldnt open %s\r\n", kbLayerFile);
    }
  }
}



void configureLed(char* deviceName, char* filePath)
{
  for(uint8_t currentLayer = 0; currentLayer < MAX_NUMBER_OF_LAYERS; currentLayer++)
  {
    char ledLayerFile[64] ="\0";
    char currentLayerChar[1];
    sprintf(currentLayerChar, "%hhu", currentLayer);
    strcat(ledLayerFile, "/");
    strcat(ledLayerFile, deviceName);
    strcat(ledLayerFile, "/led/led-l");
    strcat(ledLayerFile, currentLayerChar);
    strcat(ledLayerFile, ".json");

    File32 layerFolder;
    if(layerFolder.open(ledLayerFile))
    {
      Serial.printf("Successfully opened %s\r\n", ledLayerFile);
      uint8_t deviceID = static_cast<uint8_t>(std::atoi(getAttribute(filePath, "deviceID")));
      loadLedConfig(ledLayerFile, deviceID, currentLayer);
    }
    else
    {
      //Serial.printf("Couldnt open %s\r\n", ledLayerFile);
    }
  }
}



void configureMouse(char* deviceName, char* filePath)
{
  for(uint8_t currentLayer = 0; currentLayer < MAX_NUMBER_OF_LAYERS; currentLayer++)
  {
    char mouseLayerFile[64] ="\0";
    char currentLayerChar[1];
    sprintf(currentLayerChar, "%hhu", currentLayer);
    strcat(mouseLayerFile, "/");
    strcat(mouseLayerFile, deviceName);
    strcat(mouseLayerFile, "/mouse/m-l");
    strcat(mouseLayerFile, currentLayerChar);
    strcat(mouseLayerFile, ".json");

    File32 layerFolder;
    if(layerFolder.open(mouseLayerFile))
    {
      Serial.printf("Successfully opened %s\r\n", mouseLayerFile);
      uint8_t deviceID = static_cast<uint8_t>(std::atoi(getAttribute(filePath, "deviceID")));
      loadMouseConfig(mouseLayerFile, deviceID, currentLayer);
    }
    else
    {
      //Serial.printf("Couldnt open %s\r\n", mouseLayerFile);
    }
  }
}



void configureDevices(File32 devicePath, char* deviceName)
{
  Serial.printf("Device name to configure: %s \r\n", deviceName);

  File32 configFolder = devicePath.openNextFile();

  //Now inside device config folder
  while(configFolder)
  {
    //Serial.printf("The folder is dir: %i\r\n", configFolderisDir());
    char name[32];
    configFolder.getName(name, sizeof(name));
    Serial.printf("File name: %s\r\n", name);

    char filePath[64] = "\0";
    strcat(filePath, "/");
    strcat(filePath, deviceName);
    strcat(filePath, "/");
    strcat(filePath, deviceName);
    strcat(filePath, ".json");

    if(!configFolder.isDirectory())
    {
      //We are at the config json file that contains MAC address, etc...
      Serial.printf("File path: %s \r\n", filePath);
      Serial.printf("Device ID: %s \r\n", getAttribute(filePath, "deviceID"));  //Used to check if we can properly read attributes
      Serial.printf("Config name: %s \r\n", getAttribute(filePath, "deviceName"));
      addDeviceAddress(filePath); //Add the device address to the array of ESP-NOW devices
    }
    else
    {
      //We have a directory
      if(strcmp(name, "keyboard") == 0)
      {
        //Serial.printf("We have a keyboard config folder\r\n");
        configureKeyboard(deviceName, filePath);
      }

      if(strcmp(name, "led") == 0)
      {
        //Serial.printf("We have a led config folder\r\n");
        configureLed(deviceName, filePath);
      }

      if(strcmp(name, "mouse") == 0)
      {
        //Serial.printf("We have a mouse config folder\r\n");
        configureMouse(deviceName, filePath);
      }

    }

    configFolder = devicePath.openNextFile();
  }

  Serial.printf("\n\r");
}



void loadConfiguration()
{
  uint8_t numberOfDeviceToConfig = getNumberOfDevices();
  Serial.printf("Number of devices to config: %d \r\n\r\n", numberOfDeviceToConfig);

  File32 root = fatfs.open("/");

  for(uint8_t deviceNumber = 0; deviceNumber <= numberOfDeviceToConfig; deviceNumber++)
  {

    Serial.printf("Configuring device number %d\r\n", deviceNumber);

    File32 deviceDirectory = root.openNextFile();

    if(!deviceDirectory.isDirectory())
    {
      Serial.printf("There is a file in root, please reupload file system\r\n");
    }

    char deviceName[32];
    deviceDirectory.getName(deviceName, sizeof(deviceName));
    bool isConfigFolder = strcmp(deviceName, "System Volume Information");  //Will output something else than 0 if not sys volume info

    if(isConfigFolder) //Current folder is a device config folder
    {
      configureDevices(deviceDirectory, deviceName);
    }
    else
    {
      Serial.printf("This is system volume information configFolder.. Skipping config\r\n\r\n");
    }
  }
}