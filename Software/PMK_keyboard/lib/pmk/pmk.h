#ifndef pmk_h
#define pmk_h

#define MAX_NUMBER_OF_DEEJ_KNOBS 8

#include "Arduino.h"
#include <vector>
#include <algorithm>

int8_t forceLayer = -1;

#define DEVICEID_ADDRESS 0
#define DONGLE_MACADDRESS_ADDRESS 1 // 6 bits long
#define MAC_ADDRESS_SIZE 6

#define PMK_DEVICE_ID_BYTE 0
#define PMK_PACKET_TYPE_BYTE 1
#define PMK_DATA_BYTE 2

#define LAYER_PLUS 0xF1
#define LAYER_MINUS 0xF0



uint8_t ledBrightness = 0;

uint8_t layerID = 0;

uint8_t keyboardDeviceID = 255;

uint8_t knobIDToDeej[MAX_NUMBER_OF_DEEJ_KNOBS] = {0};



typedef struct packetStruct {
    uint8_t deviceID;
    uint8_t packetType = 255;
    uint8_t data[16];
} packetStruct;



typedef struct telemetryStruct {
    uint8_t deviceID;
    const uint8_t packetType = 0;
    uint8_t battery;    // in % (value can only from 0d0 to 0d100), 0% is 3.5v, 100% is 4.2v. 
    uint8_t temperature;
    uint8_t macAddress[6];
    uint8_t error[5];
} telemetryStruct;



typedef struct keyboardStruct {
    uint8_t deviceID;
    const uint8_t packetType = 1;
    uint8_t key[8] = {0};
} keyboardStruct;



typedef struct mouseStruct {
    uint8_t deviceID;
    const uint8_t packetType = 2;
    int8_t x;
    int8_t y;
    int8_t w;
    int8_t p;
    uint8_t key;  //5 possible buttons, 8 bits be smart u dumbass, dont use an array. (I used an array before)
} mouseStruct;



typedef struct gamepadStruct {
    uint8_t deviceID;
    const uint8_t packetType = 3;
    int8_t leftX;
    int8_t leftY;
    int8_t rightX;
    int8_t rightY;
    int8_t leftTrigger;
    int8_t rightTrigger;
    uint8_t dpad;   //0b000ldruc - Left - Down - Right - Up - Center
    uint8_t buttons[4];
} gamepadStruct;



typedef struct ledStruct {
    uint8_t deviceID;
    const uint8_t packetType = 4;
    uint8_t function;
    uint8_t ledNumberStart;
    uint8_t ledNumberEnd;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} ledStruct;



typedef struct knobStruct {
    uint8_t deviceID;
    const uint8_t packetType = 5;
    uint8_t knob[8];
} knobStruct;



typedef struct actuatorStruct {
    uint8_t deviceID;
    const uint8_t packetType = 6;
    uint8_t function;   //Can be Torque, Speed, Position, Acceleration
    uint8_t command;    //Sent by Master
    uint8_t position;   //Returned by device
} actuatorStruct;



typedef struct displayStruct {
    uint8_t deviceID;
    const uint8_t packetType = 7;
    uint8_t image;
    uint8_t x;
    uint8_t y;
    uint8_t brightness;
} displayStruct;



typedef struct serialStruct {
    uint8_t deviceID;
    const uint8_t packetType = 8;
    uint8_t packet[8];
} serialStruct;



typedef struct spaceMouseStruct {
    uint8_t deviceID;
    const uint8_t packetType = 9;
    int8_t trans[3];
    int8_t rot[3];
} spaceMouseStruct;


enum errorID {
    none, 
    tooManyKeysPressed,
    overTemperature,
    lowBattery,
    invalidPacket,
    internalSensorFailure
};



keyboardStruct   keyboardPacket;
mouseStruct      mousePacket;
gamepadStruct    gamepadPacket;
ledStruct        ledPacket;
knobStruct       knobPacket;
actuatorStruct   actuatorPacket;
displayStruct    displayPacket;
telemetryStruct  telemetryPacket;
serialStruct     serialPacket;
spaceMouseStruct spaceMousePacket;


packetStruct receivedPacket;







void convertPacket2Telemetry(packetStruct packet)
{
    telemetryPacket.deviceID = packet.deviceID;
    telemetryPacket.battery = packet.data[0];
    telemetryPacket.temperature = packet.data[1];
    telemetryPacket.macAddress[0] = packet.data[2]; //MAC address might not stay, maybe useless TODO
    telemetryPacket.macAddress[1] = packet.data[3];
    telemetryPacket.macAddress[2] = packet.data[4];
    telemetryPacket.macAddress[3] = packet.data[5];
    telemetryPacket.macAddress[4] = packet.data[6];
    telemetryPacket.macAddress[5] = packet.data[7];
    telemetryPacket.error[0] = packet.data[8];
    telemetryPacket.error[1] = packet.data[9];
    telemetryPacket.error[2] = packet.data[10];
    telemetryPacket.error[3] = packet.data[11];
    telemetryPacket.error[4] = packet.data[12];
}



void convertPacket2Keyboard(packetStruct packet)
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



void convertPacket2Mouse(packetStruct packet)
{
    mousePacket.deviceID = packet.deviceID;
    mousePacket.x = packet.data[0];
    mousePacket.y = packet.data[1];
    mousePacket.w = packet.data[2];
    mousePacket.p = packet.data[3];
    mousePacket.key = packet.data[4];
}



void convertPacket2Gamepad(packetStruct packet)
{
    gamepadPacket.deviceID = packet.deviceID;
    gamepadPacket.leftX = packet.data[0];
    gamepadPacket.leftY = packet.data[1];
    gamepadPacket.rightX = packet.data[2];
    gamepadPacket.rightY = packet.data[3];
    gamepadPacket.leftTrigger = packet.data[4];
    gamepadPacket.rightTrigger = packet.data[5];
    gamepadPacket.dpad = packet.data[6];

    for(uint8_t i = 0; i < 4; ++i)
    {
        gamepadPacket.buttons[i] = packet.data[7+i];
    }
}



void convertPacket2Led(packetStruct packet)
{
    ledPacket.deviceID = packet.deviceID;
    ledPacket.function = packet.data[0];
    ledPacket.red = packet.data[1];
    ledPacket.green = packet.data[2];
    ledPacket.blue = packet.data[3];
}



void convertPacket2Knob(packetStruct packet)
{
    knobPacket.deviceID = packet.deviceID;  
    for(uint8_t i = 0; i < 7; ++i)
    {
      knobPacket.knob[i] = packet.data[i];
    }
}



void convertPacket2Actuator(packetStruct packet)
{
    actuatorPacket.deviceID = packet.deviceID;
    actuatorPacket.function = packet.data[0];
    actuatorPacket.position = packet.data[1];
    actuatorPacket.command = packet.data[2];
}



void convertPacket2Display(packetStruct packet)
{
    displayPacket.deviceID = packet.deviceID;
    displayPacket.image = packet.data[0];
    displayPacket.x = packet.data[1];
    displayPacket.y = packet.data[2];
    displayPacket.brightness = packet.data[3];
} 



void convertPacket2Serial(packetStruct packet)
{
    serialPacket.deviceID = packet.deviceID;
    for(uint8_t i = 0; i < 8; i++)
    {
      serialPacket.packet[i] = packet.data[i];
    }
}



void convertPacket2SpaceMouse(packetStruct packet)
{
    spaceMousePacket.deviceID = packet.deviceID;
    for(uint8_t i = 0; i < 3; i++)
    {
        spaceMousePacket.trans[i] = packet.data[i];
        spaceMousePacket.rot[i] = packet.data[i + 3];
    }
}



#endif