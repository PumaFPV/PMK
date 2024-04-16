#ifndef pmk_h
#define pmk_h

#include "Arduino.h"
#include <vector>
#include <algorithm>

#include "Adafruit_TinyUSB.h"
#include "variables.h"
#include "config.h"

#define LAYER_PLUS 0xF1
#define LAYER_MINUS 0xF0

uint8_t ledBrightness = 0;

uint8_t layerID = 0;

uint8_t keyboardDeviceID = 255;

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
    int8_t x;
    int8_t y;
    int8_t w;
    int8_t p;
    uint8_t key;  //5 possible buttons, 8 bits be smart u dumbass, dont use an array. (I used an array before)
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

keyboardStruct  keyboardPacket[8];
mouseStruct     mousePacket;
gamepadStruct   gamepadPacket;
ledStruct       ledPacket;
knobStruct      knobPacket;
actuatorStruct  actuatorPacket;
displayStruct   displayPacket;
telemetryStruct telemetryPacket;
serialStruct    serialPacket;

packetStruct receivedPacket;

keyboardStruct previousKeyboardPacket[8];

void setupPMK()
{
    for(uint8_t deviceID = 0; deviceID < 8; deviceID++)
    {
        keyboardPacket[deviceID].deviceID = deviceID;

        for(uint8_t keyID = 0; keyID < 8; keyID++)
        {
            keyboardPacket[deviceID].key[keyID] = 0;
        }
    }
}

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
    keyboardPacket[packet.deviceID].deviceID = packet.deviceID;
    keyboardPacket[packet.deviceID].key[0] = packet.data[0];
    keyboardPacket[packet.deviceID].key[1] = packet.data[1];
    keyboardPacket[packet.deviceID].key[2] = packet.data[2];
    keyboardPacket[packet.deviceID].key[3] = packet.data[3];
    keyboardPacket[packet.deviceID].key[4] = packet.data[4];
    keyboardPacket[packet.deviceID].key[5] = packet.data[5];
    keyboardPacket[packet.deviceID].key[6] = packet.data[6];
    keyboardPacket[packet.deviceID].key[7] = packet.data[7];
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

uint8_t nonZeroSize(uint8_t arr[])
{
    uint8_t count = 0;

    for(uint8_t i = 0; i < 8; i++)
    {
        if(arr[i] == 0)
        {
            return i;
        }
    }
    return 255;
}

void handleKeyboard()
{
    if(TinyUSBDevice.suspended())
    {
        TinyUSBDevice.remoteWakeup();
    }

    if(!usb_hid.ready())
    {
        Serial.printf("not ready \r\n");
        return;
    } 
    //Serial.printf("usb ready\r\n");
    
    uint8_t keycode[6] = {0};
    uint8_t keycodeNumber = 0;
    uint8_t modifier = 0x00;
    std::vector<uint8_t> keyToPress{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
    uint8_t keyToPressID = 0;
    layerID = 0;

    //Need to check for layer change key before going further
    for(uint8_t deviceID = 0; deviceID < 8; deviceID++)
    {
        for(uint8_t i = 0; i < 8; i++)
        {
            switch(keyIDtoHID(keyboardPacket[deviceID].key[i], layerID, deviceID))
            {
                case LAYER_MINUS:
                    layerID = 1;
                    break;
                case LAYER_PLUS:
                    layerID = 2;
                    break;
            }
        }
    }

    //Convert key[8] to HID[64]
    for(uint8_t deviceID = 0; deviceID < 8; deviceID++)
    {
        for(uint8_t i = 0; i < 8; i++)
        {
            keyToPress[i + deviceID*8] = keyIDtoHID(keyboardPacket[deviceID].key[i], layerID, deviceID);
        }
    }

    //Delete duplicate HID
    std::sort(keyToPress.begin(), keyToPress.end());
    keyToPress.erase(std::unique(keyToPress.begin(), keyToPress.end()), keyToPress.end());

    //Serial.printf("%02X %02X %02X %02X %02X %02X\r\n", keyToPress[0], keyToPress[1], keyToPress[2], keyToPress[3], keyToPress[4], keyToPress[5]);

    uint8_t numberOfKeyToPress = std::find(keyToPress.begin(), keyToPress.end(), 255) - keyToPress.begin();

    for(uint8_t i = i; i < numberOfKeyToPress; i++) //Add the final keyToPress to the report / deal with modifiers
    {
        uint8_t HIDKey = keyToPress[i];
        if(0xDF < HIDKey && HIDKey < 0xE8)  //
        {
            //We have a modifier
            modifier = modifier | (1 << (HIDKey - 0xE0));   //First modifier is 0xE0 Left control
            //Serial.printf("Modifier is: %02X\r\n", modifier);
        }

        if(HIDKey < 0xA5 )
        {
            //We have a non modifier key
            //Serial.printf("Pressing: 0x%u, on layer: %i, Equivalent HID: %u\r\n", keyboardPacket.key[i], layerID, keyIDtoChar(keyboardPacket.key[i], layerID));
            if(keycodeNumber < 6)
            {
                keycode[keycodeNumber] = HIDKey;
            }

            keycodeNumber++;
        }

        if(HIDKey < 0xA5 && keycodeNumber > 6)
        {
            //6KRO error
            for(uint8_t j = 0; j < 6; j++)
            {
                keycode[j] = 0x01;
            }
        }

        //Deal here with special keys (layers, brightness, etc...)
        if(HIDKey > 0xE8)
        {

        }
    }

    Serial.printf("Report: %02X %02X %02X %02X %02X %02X modifier: %02X\r\n", keycode[0], keycode[1], keycode[2], keycode[3], keycode[4], keycode[5], modifier);
    usb_hid.keyboardReport(RID_KEYBOARD, modifier, keycode);

}

void handleMouse()
{
    if(usb_hid.ready())
    {
        Serial.printf("x: %i, y: %i, key: %u, wheel: %i, pan: %i", mousePacket.x, mousePacket.y, mousePacket.key,mousePacket.w, mousePacket.p);
        usb_hid.mouseReport(RID_MOUSE, mousePacket.key, mousePacket.x, mousePacket.y, mousePacket.w, mousePacket.p);
    }
}

void handleGamepad()
{
    uint8_t gamepadHat;
    switch(gamepadPacket.dpad)
    {
        case 0b00000001:
            gamepadHat = GAMEPAD_HAT_CENTERED;
            break;
        case 0b00000010:
            gamepadHat = GAMEPAD_HAT_UP;
            break;  
        case 0b00000110:
            gamepadHat = GAMEPAD_HAT_UP_RIGHT;
            break;
        case 0b00000100:
            gamepadHat = GAMEPAD_HAT_RIGHT;
            break;
        case 0b00001100:
            gamepadHat = GAMEPAD_HAT_DOWN_RIGHT;
            break;
        case 0b00001000:
            gamepadHat = GAMEPAD_HAT_DOWN;
            break;
        case 0b00011000:
            gamepadHat = GAMEPAD_HAT_DOWN_LEFT;
            break;
        case 0b00010000:
            gamepadHat = GAMEPAD_HAT_LEFT;
            break;
        case 0b00010010:
            gamepadHat = GAMEPAD_HAT_UP_LEFT;
            break;
    }

    uint32_t gamePadButtons = gamepadPacket.buttons[0] | (gamepadPacket.buttons[1] << 8) | (gamepadPacket.buttons[2] << 16) | (gamepadPacket.buttons[3] << 24);
    
    //Gamepad.send(
    //    gamepadPacket.leftX, gamepadPacket.leftY, 
    //    gamepadPacket.rightX, gamepadPacket.rightY, 
    //    gamepadPacket.leftTrigger, gamepadPacket.rightTrigger, 
    //    gamepadHat, gamePadButtons
    //);

}

void handleKnob()
{

}

#endif