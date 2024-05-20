#pragma once

//Define Trackballer Breakout pin connections to Arduino
#define PIN_TRACKBALL_RIGHT     37
#define PIN_TRACKBALL_UP        33
#define PIN_TRACKBALL_LEFT      35
#define PIN_TRACKBALL_DOWN      18

#define PIN_TRACKBALL_BUTTON    16

#define PIN_TRACKBALL_LED_WHITE 39
#define PIN_TRACKBALL_LED_GREEN 36
#define PIN_TRACKBALL_LED_RED   38
#define PIN_TRACKBALL_LED_BLUE  40
#define EEPROM_SIZE 1


mouseStruct mousePacket;


uint8_t dongleAddress[] = {0x58, 0xCF, 0x79, 0xA3, 0x98, 0xC8}; //C2 - C8



int xPosition;
int yPosition;
bool ledState = 0;
//int16_t deg = 0;
//int8_t radius = 5;



struct Func
{
  uint16_t counter;
  unsigned long beginTime;
  unsigned long startCounterTime;
  unsigned long endTime;
  unsigned long duration;
  const    long interval;
  uint8_t frequency;
  unsigned long inBetweenTime;
};

Func gpioTask = {0, 0, 0, 0, 0, 10000, 0, 0};
Func pmkTask = {0, 0, 0, 0, 0, 10000, 0, 0};
Func uartTask = {0, 0, 0, 0, 0, 20000, 0, 0};

