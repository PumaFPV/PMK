#ifndef pmk_h
#define pmk_h

#include "Arduino.h"
#include "variables.h"

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

uint8_t settingLayerID = 2;

uint8_t layerKeyID[2] = { //If no layer, put 0xFE which is key 254, which doesnt exist. Key 255 is no key...
  0x10,
  0x1F
};

uint8_t keyIDtoLedID(uint8_t keyID)
{
  uint8_t ledID[NUM_LEDS+1] = 
  {
    7, 0, 1, 2, 3, 4, 5,
    6, 21, 13, 12, 11, 10, 9,
    8, 20, 38, 14, 15, 16, 17,
    18, 19, 22, 28, 27, 26, 25, 
    24, 23, 36, 37, 100, 32, 
    33, 34, 35, 29, 30, 31
  };

  return ledID[keyID];
}

uint8_t ledIDtoKeyID(uint8_t ledID)
{
  uint8_t keyID[40] = 
  {
    1, 2, 3, 4, 5, 6, 7, 0, 14, 13, 12, 11, 10, 9, 17, 18, 19, 20, 21, 22, 15, 8, 23, 29, 28, 27, 26, 25, 24, 37, 38, 39, 33, 34, 35, 36, 30, 31, 16
  };

  return keyID[ledID];
}

uint8_t keyIDtoChar(uint8_t keyID, uint8_t layer)
{
  uint8_t keyChar[8][40] = 
  {
    {
      '5','4','3','2','1','0',KEY_ESC,
      KEY_TAB,'z','q','w','e','r','t',
      'g','f','d','s','a',0,KEY_CAPS_LOCK,
      KEY_LEFT_SHIFT,'<','y','x','c','v','b',
      0,KEY_LEFT_GUI,KEY_LEFT_ALT,KEY_LEFT_CTRL,KEY_RETURN,' ',
      KEY_DELETE,KEY_BACKSPACE,0,0,0,0
    },
    {
      KEY_F5,KEY_F4,KEY_F3,KEY_F2,KEY_F1,0,KEY_ESC,
      KEY_TAB,0,0,KEY_UP_ARROW,0,0,0,
      0,0,KEY_RIGHT_ARROW,KEY_DOWN_ARROW,KEY_LEFT_ARROW,0,KEY_CAPS_LOCK,
      KEY_LEFT_SHIFT,0,0,0,0,0,0,
      0,KEY_LEFT_GUI,KEY_LEFT_ALT,KEY_LEFT_CTRL,KEY_RETURN,' ',
      KEY_DELETE,KEY_BACKSPACE,0,0,0,0
    },
    {
      0,0,0,0,0,0,KEY_ESC,
      KEY_TAB,0,0,0,0,0,0,
      0,0,0,0,0,0,KEY_CAPS_LOCK,
      KEY_LEFT_SHIFT,0,0,0,0,0,0,
      0,KEY_LEFT_GUI,KEY_LEFT_ALT,KEY_LEFT_CTRL,KEY_RETURN,' ',
      KEY_DELETE,KEY_BACKSPACE,0,0,0,0
    },
    {
      0,0,0,0,0,0,KEY_ESC,
      KEY_TAB,0,0,0,0,0,0,
      0,0,0,0,0,0,KEY_CAPS_LOCK,
      KEY_LEFT_SHIFT,0,0,0,0,0,0,
      0,KEY_LEFT_GUI,KEY_LEFT_ALT,KEY_LEFT_CTRL,KEY_RETURN,' ',
      KEY_DELETE,KEY_BACKSPACE,0,0,0,0
    },
    {
      0,0,0,0,0,0,KEY_ESC,
      KEY_TAB,0,0,0,0,0,0,
      0,0,0,0,0,0,KEY_CAPS_LOCK,
      KEY_LEFT_SHIFT,0,0,0,0,0,0,
      0,KEY_LEFT_GUI,KEY_LEFT_ALT,KEY_LEFT_CTRL,KEY_RETURN,' ',
      KEY_DELETE,KEY_BACKSPACE,0,0,0,0
    },
    {
      0,0,0,0,0,0,KEY_ESC,
      KEY_TAB,0,0,0,0,0,0,
      0,0,0,0,0,0,KEY_CAPS_LOCK,
      KEY_LEFT_SHIFT,0,0,0,0,0,0,
      0,KEY_LEFT_GUI,KEY_LEFT_ALT,KEY_LEFT_CTRL,KEY_RETURN,' ',
      KEY_DELETE,KEY_BACKSPACE,0,0,0,0
    },
    {
      0,0,0,0,0,0,KEY_ESC,
      KEY_TAB,0,0,0,0,0,0,
      0,0,0,0,0,0,KEY_CAPS_LOCK,
      KEY_LEFT_SHIFT,0,0,0,0,0,0,
      0,KEY_LEFT_GUI,KEY_LEFT_ALT,KEY_LEFT_CTRL,KEY_RETURN,' ',
      KEY_DELETE,KEY_BACKSPACE,0,0,0,0
    },
    {
      0,0,0,0,0,0,KEY_ESC,
      KEY_TAB,0,0,0,0,0,0,
      0,0,0,0,0,0,KEY_CAPS_LOCK,
      KEY_LEFT_SHIFT,0,0,0,0,0,0,
      0,KEY_LEFT_GUI,KEY_LEFT_ALT,KEY_LEFT_CTRL,KEY_RETURN,' ',
      KEY_DELETE,KEY_BACKSPACE,0,0,0,0
    },
  };
  
  return keyChar[layer][keyIDtoLedID(keyID)];
}

void setLedColorProfile(uint8_t profile)
{
  switch (profile)
  {
  case 0:
    for(uint8_t i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = ledColorProfile[profile][i];
    }
    break;
  
  case 1: 
    for(uint8_t i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = ledColorProfile[profile][i];
    }
    break;
  
  case 2: 
    for(uint8_t i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = ledColorProfile[profile][i];
    }
    break;

  case 3:
    pulsar(40, 250);
    break;

  }
}

uint32_t ledColorProfile[8][NUM_LEDS] = 
{
  {
    0x000000, 0x000000, 0xFF0000, 0xFF0000, 0xFF0000, 0xFF0000, 0xDEB887,
    0x9966CC, 0x32CD32, 0xFFFF00, 0x32CD32, 0xFFFF00, 0xFF0000, 0xDEB887, 
    0xFF0000, 0x0000FF, 0x32CD32, 0x32CD32, 0x32CD32, 0x000000, 0x32CD32, 
    0x32CD32, 0xFF0000, 0x000000, 0xEE82EE , 0x32CD32, 0x111111, 0xFF0000, 
    0x000000, 0x000000, 0x000000, 0x32CD32, 0x000000, 0x32CD32, 0x000000, 
    0x000000, 0x000000, 0x000000, 0x000000
  },
  {
    0x00FF00, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x00FF00,
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111
  },
  {
    0xFF0000, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0xFF0000,
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111
  },
  {
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111,
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111
  },
  {
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111,
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111
  },
  {
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111,
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111
  },
  {
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111,
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111
  },
  {
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111,
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x1111FF
  }
};

uint8_t keyCoordinatesToLedID(uint8_t x, uint8_t y)
{
  uint8_t ledID[6][9] =
    {
      {6,5,4,3,2,1,0,100,100},
      {7,8,9,10,11,12,13,100,100},
      {20,19,18,17,16,15,14,100,100},
      {21,22,23,24,25,26,27,28,29},
      {39,38,37,36,35,34,33,32,30},
      {100,100,100,100,100,100,100,100,31}

    };
    return ledID[x][y];
}



#endif