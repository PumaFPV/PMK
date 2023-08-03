#ifndef CONFIGHANDLE_H
#define CONFIGHANDLE_H

#include "variables.h"
#include "fsHandle.h"

uint8_t getNumberOfDevices()
{
    uint8_t numberOfDevices = 0;

    File root = LittleFS.open("/");
    File directory = root.openNextFile();
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

void configureDeviceMacAddress(JsonDocument& configJson)
{
    //deviceAddress[];

    JsonObject device1 = configJson["device1"];
    String _value = device1["deviceName"];
    JsonArray macAddress = device1["deviceAddress"]; 

    for (const auto& item : macAddress) {
        const char* value = item.as<const char*>();
    }
}

void configureKeyboard()
{

}

void readAttribute(const char* filename, const char* objectName, const char* attributeName) {
  File file = LittleFS.open(filename, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  size_t size = file.size();
  std::unique_ptr<char[]> buf(new char[size]);

  file.readBytes(buf.get(), size);
  file.close();

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, buf.get());
  if (error) {
    Serial.println("Failed to parse JSON");
    return;
  }

  const char* value = doc[objectName][attributeName].as<const char*>();
  
  Serial.print("Attribute value: ");
  Serial.println(value);
}

void readAttribute(const char* filename, const char* attributeName) {
  File file = LittleFS.open(filename, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  size_t size = file.size();
  std::unique_ptr<char[]> buf(new char[size]);

  file.readBytes(buf.get(), size);
  file.close();

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, buf.get());
  if (error) {
    Serial.println("Failed to parse JSON");
    return;
  }

  const char* value = doc[attributeName].as<const char*>();
  
  Serial.print("Attribute value: ");
  Serial.println(value);
}

#endif