#ifndef pmk_h
#define pmk_h

#include "Arduino.h"

#include "config.h"

typedef struct packetStruct {
    uint8_t deviceID;
    uint8_t packetType;
    uint8_t data[16];
}   packetStruct;

typedef struct telemetryStruct {
    uint8_t deviceID;
    const uint8_t packetType = 0;
    uint8_t battery;
    uint8_t temperature;
    uint8_t macAddress;
    uint8_t error;
}   telemetryStruct;

typedef struct keyboardStruct {
    uint8_t deviceID;
    const uint8_t packetType = 1;
    uint8_t key[8];
}   keyboardStruct;

typedef struct mouseStruct {
    uint8_t deviceID;
    const uint8_t packetType = 2;
    uint8_t x;
    uint8_t y;
    uint8_t w;
    uint8_t p;
    uint8_t k;  
}   mouseStruct;

typedef struct gamepadStruct {
    uint8_t deviceID;
    const uint8_t packetType = 3;
    uint8_t x;
    uint8_t y;
    uint8_t z;
    uint8_t rz;
    uint8_t rx;
    uint8_t ry;
    uint8_t hh;
    uint8_t buttons[5];
}   gamepadStruct;

typedef struct ledStruct {
    uint8_t deviceID;
    const uint8_t packetType = 4;
    uint8_t function;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
}   ledStruct;

typedef struct knobStruct {
    uint8_t deviceID;
    const uint8_t packetType = 5;
    uint8_t knob[8];
}   knobStruct;

typedef struct actuatorStruct {
    uint8_t deviceID;
    const uint8_t packetType = 6;
    uint8_t function;
    uint8_t position;
    uint8_t command;
}   actuatorStruct;

typedef struct displayStruct {
    uint8_t deviceID;
    const uint8_t packetType = 7;
    uint8_t image;
    uint8_t x;
    uint8_t y;
    uint8_t brightness;
}   displayStruct;

typedef struct serialStruct {
    uint8_t deviceID;
    const uint8_t packetType = 9;
    uint8_t packet[8];
}   serialStruct;

enum errorID {
    none, 
    tooManyKeysPressed
};

keyboardStruct keyboardPacketConvert;
mouseStruct mousePacketConvert;
gamepadStruct gamepadPacketConvert;
ledStruct ledPacketConvert;
knobStruct knobPacketConvert;
actuatorStruct actuatorPacketConvert;
displayStruct displayPacketConvert;
telemetryStruct telemetryPacketConvert;
serialStruct serialPacketConvert;


void convertPacket2Keyboard(packetStruct packet)
{
    keyboardPacketConvert.deviceID = packet.deviceID;
    keyboardPacketConvert.key[0] = packet.data[0];
    keyboardPacketConvert.key[1] = packet.data[1];
    keyboardPacketConvert.key[2] = packet.data[2];
    keyboardPacketConvert.key[3] = packet.data[3];
    keyboardPacketConvert.key[4] = packet.data[4];
    keyboardPacketConvert.key[5] = packet.data[5];
    keyboardPacketConvert.key[6] = packet.data[6];
    keyboardPacketConvert.key[7] = packet.data[7];
}

void convertPacket2Mouse(packetStruct packet)
{
    mousePacketConvert.deviceID = packet.deviceID;
    mousePacketConvert.x = packet.data[0];
    mousePacketConvert.y = packet.data[1];
    mousePacketConvert.w = packet.data[2];
    mousePacketConvert.p = packet.data[3];
    mousePacketConvert.k = packet.data[4];
}

void convertPacket2Gamepad(packetStruct packet)
{
    gamepadPacketConvert.deviceID = packet.deviceID;
    gamepadPacketConvert.x = packet.data[0];
    gamepadPacketConvert.y = packet.data[1];
    gamepadPacketConvert.z = packet.data[2];
    gamepadPacketConvert.rz = packet.data[3];
    gamepadPacketConvert.rx = packet.data[4];
    gamepadPacketConvert.ry = packet.data[5];
    gamepadPacketConvert.hh = packet.data[6];

    for(uint8_t i = 0; i < 4; ++i)
    {
        gamepadPacketConvert.buttons[i] = packet.data[7+i];
    }
}

void convertPacket2Led(packetStruct packet)
{
    ledPacketConvert.deviceID = packet.deviceID;
    ledPacketConvert.function = packet.data[0];
    ledPacketConvert.red = packet.data[1];
    ledPacketConvert.green = packet.data[2];
    ledPacketConvert.blue = packet.data[3];
}

void convertPacket2Knob(packetStruct packet)
{
    knobPacketConvert.deviceID = packet.deviceID;  
    for(uint8_t i = 0; i < 7; ++i)
    {
      knobPacketConvert.knob[i] = packet.data[i];
    }
}

void convertPacket2Actuator(packetStruct packet)
{
    actuatorPacketConvert.deviceID = packet.deviceID;
    actuatorPacketConvert.function = packet.data[0];
    actuatorPacketConvert.position = packet.data[1];
    actuatorPacketConvert.command = packet.data[2];
}

void convertPacket2Display(packetStruct packet)
{
    displayPacketConvert.deviceID = packet.deviceID;
    displayPacketConvert.image = packet.data[0];
    displayPacketConvert.x = packet.data[1];
    displayPacketConvert.y = packet.data[2];
    displayPacketConvert.brightness = packet.data[3];
}

void convertPacket2Telemetry(packetStruct packet)
{
    telemetryPacketConvert.deviceID = packet.deviceID;
    telemetryPacketConvert.battery = packet.data[0];
    telemetryPacketConvert.temperature = packet.data[1];
    telemetryPacketConvert.macAddress = packet.data[2];
    telemetryPacketConvert.error = packet.data[3];
}   

void convertPacket2Serial(packetStruct packet)
{
    serialPacketConvert.deviceID = packet.deviceID;
    for(uint8_t i = 0; i < 7; ++i)
    {
      serialPacketConvert.packet[i] = packet.data[i];
    }
}

uint8_t packet2key(uint8_t deviceID, uint8_t key[8])
{
    /*
    if(modifierKeyIsPressed)
    */
}


#endif