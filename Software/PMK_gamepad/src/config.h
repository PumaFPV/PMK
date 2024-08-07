#ifndef CONFIG_H
#define CONFIG_H

#include "USBHIDKeyboard.h"


#define NUM_LEDS 39
#define LED_DATA_PIN 4

uint8_t settingLayerID = 2;

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
};

uint8_t ledIDtoKeyID(uint8_t ledID)
{
  uint8_t keyID[40] = 
  {
    1, 2, 3, 4, 5, 6, 7, 0, 14, 13, 12, 11, 10, 9, 17, 18, 19, 20, 21, 22, 15, 8, 23, 29, 28, 27, 26, 25, 24, 37, 38, 39, 33, 34, 35, 36, 30, 31, 16
  };

  return keyID[ledID];
};

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
};

#endif