#ifndef pmk_h
#define pmk_h

#include "Arduino.h"
#include "USBHIDKeyboard.h"
#include "USBHIDMouse.h"
#include "USBHIDGamepad.h"

#include "config.h"

USBHIDKeyboard Keyboard;
USBHIDMouse Mouse;
USBHIDGamepad Gamepad;


uint8_t ledBrightness = 0;

uint8_t layerID = 0;

typedef struct packetStruct {
    uint8_t deviceID;
    uint8_t packetType = 255;
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
    uint8_t key[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
}   keyboardStruct;

typedef struct mouseStruct {
    uint8_t deviceID;
    const uint8_t packetType = 2;
    uint8_t x;
    uint8_t y;
    uint8_t w;
    uint8_t p;
    uint8_t key;  //5 possible buttons, 8 bits be smart dumbass, dont use an array.  
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

keyboardStruct  keyboardPacket;
mouseStruct     mousePacket;
gamepadStruct   gamepadPacket;
ledStruct       ledPacket;
knobStruct      knobPacket;
actuatorStruct  actuatorPacket;
displayStruct   displayPacket;
telemetryStruct telemetryPacket;
serialStruct    serialPacket;

packetStruct receivedPacket;

keyboardStruct previousKeyboardPacket;


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
    gamepadPacket.x = packet.data[0];
    gamepadPacket.y = packet.data[1];
    gamepadPacket.z = packet.data[2];
    gamepadPacket.rz = packet.data[3];
    gamepadPacket.rx = packet.data[4];
    gamepadPacket.ry = packet.data[5];
    gamepadPacket.hh = packet.data[6];

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

void convertPacket2Telemetry(packetStruct packet)
{
    telemetryPacket.deviceID = packet.deviceID;
    telemetryPacket.battery = packet.data[0];
    telemetryPacket.temperature = packet.data[1];
    telemetryPacket.macAddress = packet.data[2];
    telemetryPacket.error = packet.data[3];
}   

void convertPacket2Serial(packetStruct packet)
{
    serialPacket.deviceID = packet.deviceID;
    for(uint8_t i = 0; i < 7; ++i)
    {
      serialPacket.packet[i] = packet.data[i];
    }
}

uint8_t packet2key(uint8_t deviceID, uint8_t key[8])
{
    /*
    if(modifierKeyIsPressed)
    */
   return 0;
}

void handleKeyboard()
{
    for(uint8_t i = 0; i < 8; i++)
    {
        for(uint8_t j = 0; j < 2; j++)
        {
            if(keyboardPacket.key[i] == layerKeyID[j])
            {
            if(j == 0 && layerID > 0)
            {
                layerID--;
                delay(200);
            }
            else if (j == 1 && layerID < 7)
            {
                layerID++;
                delay(200);
            }
                //layerID = j;
                keyboardPacket.key[i] = 0xFF;
                i = 9;
                j = 2;
            }
        }
    }

    for(uint8_t i = 0; i < 8; i++)
    {
        if(keyboardPacket.key[i] != 255 && layerID != settingLayerID)
        {
            Keyboard.press(keyIDtoChar(keyboardPacket.key[i], layerID));
            //Serial.print("Pressing: 0x");
            //Serial.print(keyboardPacket.key[i]);
            //Serial.println(keyIDtoChar(keyboardPacket.key[i], layerID), HEX);
        }
        else if (keyboardPacket.key[i] != 0xFF && layerID == settingLayerID)
        {
            switch (keyboardPacket.key[i])
            {
            case 0x09:
            if(ledBrightness < 255)
            {
                ledBrightness++;
            }   
            break;

            case 0x11:
            if(ledBrightness > 0)
            {
                ledBrightness--;
            }
            break;
            }
        }        
        }

        uint8_t releaseKeys[8];
        memcpy(releaseKeys, previousKeyboardPacket.key, 8);

        for(uint8_t i = 0; i < 8; i++)
        {
        for(uint8_t j = 0; j < 8; j++)
        {
            if(previousKeyboardPacket.key[i] == keyboardPacket.key[j])
            {
            releaseKeys[i] = 255;
            }
        }
        }

        for(uint8_t i = 0; i < 8; i++)
        {
            if(releaseKeys[i] != 255)
            {
                Keyboard.release(keyIDtoChar(releaseKeys[i], layerID));
            }
        }   

    memcpy(previousKeyboardPacket.key, keyboardPacket.key, 8);

}

void handleMouse()
{
    Mouse.move(mousePacket.x, mousePacket.y, mousePacket.w, mousePacket.p);
    //Mouse.press(mousePacket.key[0]);
}

#endif