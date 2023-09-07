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
    uint8_t macAddress[6];
    uint8_t error[5];
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
    uint8_t key;  //5 possible buttons, 8 bits be smart u dumbass, dont use an array.  
}   mouseStruct;

typedef struct gamepadStruct {
    uint8_t deviceID;
    const uint8_t packetType = 3;
    uint8_t leftX;
    uint8_t leftY;
    uint8_t rightX;
    uint8_t rightY;
    uint8_t leftTrigger;
    uint8_t rightTrigger;
    uint8_t dpad;   //0b000ldruc - Left - Down - Right - Up - Center
    uint8_t buttons[4];
}   gamepadStruct;

typedef struct ledStruct {
    uint8_t deviceID;
    const uint8_t packetType = 4;
    uint8_t function;
    uint8_t ledNumberStart;
    uint8_t ledNumberEnd;
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
    uint8_t function;   //Can be Torque, Speed, Position, Acceleration
    uint8_t command;    //Sent by Master
    uint8_t position;   //Returned by device
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
    const uint8_t packetType = 8;
    uint8_t packet[8];
}   serialStruct;

enum errorID {
    none, 
    tooManyKeysPressed,
    overTemperature,
    lowBattery,
    invalidPacket,
    internalSensorFailure
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
    for(uint8_t i = 0; i < 7; ++i)
    {
      serialPacket.packet[i] = packet.data[i];
    }
}

void handleKeyboard()
{

    //Figure out if any key layer change is pressed
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
                keyboardPacket.key[i] = 0xFF;
                i = 9;
                j = 2;
            }
        }
    }

    for(uint8_t i = 0; i < 8; i++) //Go through the 8 sent keys
    {
        if(keyboardPacket.key[i] != 0xFF && layerID != settingLayerID) //TODO change layer management
        {
            Keyboard.press(keyIDtoChar(keyboardPacket.key[i], layerID)); //TODO add deviceID to keyIDtoChar function
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

        //Press/Release management
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
    
    if(mousePacket.key && 0b00001)
    {
        Mouse.press(MOUSE_LEFT);
    }
    else
    {
        Mouse.release(MOUSE_LEFT);
    }

    if(mousePacket.key && 0b00010)
    {
        Mouse.press(MOUSE_RIGHT);
    }
    else
    {
        Mouse.release(MOUSE_RIGHT);
    }
    
    if(mousePacket.key && 0b00100)
    {
        Mouse.press(MOUSE_MIDDLE);
    }
    else
    {
        Mouse.release(MOUSE_MIDDLE);
    }

    if(mousePacket.key && 0b01000)
    {
        Mouse.press(MOUSE_BACKWARD);
    }
    else
    {
        Mouse.release(MOUSE_BACKWARD);
    }

    if(mousePacket.key && 0b10000)
    {
        Mouse.press(MOUSE_FORWARD);
    }
    else
    {
        Mouse.release(MOUSE_FORWARD);
    }
}

void handleGamepad()
{
    uint8_t gamepadHat;
    switch(gamepadPacket.dpad)
    {
        case 0b00000001:
            gamepadHat = HAT_CENTER;
            break;
        case 0b00000010:
            gamepadHat = HAT_UP;
            break;  
        case 0b00000110:
            gamepadHat = HAT_UP_RIGHT;
            break;
        case 0b00000100:
            gamepadHat = HAT_RIGHT;
            break;
        case 0b00001100:
            gamepadHat = HAT_DOWN_RIGHT;
            break;
        case 0b00001000:
            gamepadHat = HAT_DOWN;
            break;
        case 0b00011000:
            gamepadHat = HAT_DOWN_LEFT;
            break;
        case 0b00010000:
            gamepadHat = HAT_LEFT;
            break;
        case 0b00010010:
            gamepadHat = HAT_UP_LEFT;
            break;
    }

    uint32_t gamePadButtons = gamepadPacket.buttons[0] | (gamepadPacket.buttons[1] << 8) | (gamepadPacket.buttons[2] << 16) | (gamepadPacket.buttons[3] << 24);
    
    Gamepad.send(
        gamepadPacket.leftX, gamepadPacket.leftY, 
        gamepadPacket.rightX, gamepadPacket.rightY, 
        gamepadPacket.leftTrigger, gamepadPacket.rightTrigger, 
        gamepadHat, gamePadButtons
    );

}

void handleKnob()
{

}

#endif