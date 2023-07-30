#ifndef CONFIGHANDLE_H
#define CONFIGHANDLE_H

#include "variables.h"
#include "fsHandle.h"

uint8_t getNumberOfDevices(JsonDocument& configJson)
{
    JsonObject root = configJson.as<JsonObject>();
    uint8_t numberOfDevices = root.size();
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