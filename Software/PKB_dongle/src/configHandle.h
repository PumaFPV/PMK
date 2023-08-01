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

#endif