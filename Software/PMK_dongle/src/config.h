#ifndef CONFIG_H
#define CONFIG_H

#include "Adafruit_TinyUSB.h"

#define MAX_NUMBER_OF_LEDS 32
#define MAX_NUMBER_OF_DEVICES 8
#define MAX_NUMBER_OF_LAYERS 8
#define MAX_NUMBER_OF_KEYS 32

uint16_t keyChar[MAX_NUMBER_OF_DEVICES][MAX_NUMBER_OF_LAYERS][MAX_NUMBER_OF_KEYS] = {0};

uint32_t ledColorProfile[MAX_NUMBER_OF_DEVICES][MAX_NUMBER_OF_LAYERS][MAX_NUMBER_OF_LEDS] = {0};

uint16_t dpi[MAX_NUMBER_OF_DEVICES][MAX_NUMBER_OF_LAYERS] = {0};

uint8_t keyIDtoLedID(uint8_t keyID)
{
  uint8_t ledID[32] = 
  {
    7, 6, 5, 4, 3, 2, 1, 0, 14, 15, 16, 17, 8, 9, 10, 11, 23, 22, 21, 20, 19, 18, 13, 12, 255, 255, 29, 24, 25, 26, 27, 28 
  };

  return ledID[keyID-1];
};



uint8_t ledIDtoKeyID(uint8_t ledID)
{
  uint8_t keyID[32] = 
  {
    8, 7, 6, 5, 4, 3, 2, 1, 13, 14, 15, 16, 24, 23, 9, 10, 11, 12, 22, 21, 20, 19, 18, 17, 28, 29, 30, 31, 32
  };

  return keyID[ledID];
};


//Make sure to load config to keyChar before calling keyIDtoHID()
uint16_t keyIDtoHID(uint8_t keyID, uint8_t layer, uint8_t deviceID)
{
  if(keyID == 0)
  {
    return 0xFFFF;
  }

  //Serial.printf("KeyID: %u, LED ID: %u\r\n", keyID, keyIDtoLedID(keyID));
  return keyChar[deviceID][layer][keyID-1];
};

#endif