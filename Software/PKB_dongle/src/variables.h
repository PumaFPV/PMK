#ifndef variables_h
#define variables_h

#include "USB.h"
#include "USBHIDKeyboard.h"
#include "WiFi.h"
#include "esp_now.h"
#include "pmk.h"


//--------------------------------------------------Define--------------------------------------------------
//----------Global values
#define MIRCOS2SECONDS 1000000

//----------GPI

//----------GPO

//----------ADC

//----------PWM

//----------LED
#define LED_PIN 9



//----------Peripherals
//---UART
//HardwareSerial debug = HardwareSerial(0);


//---I2C

//---SPI

//--------------------------------------------------Initialize libraries--------------------------------------------------

USBHIDKeyboard Keyboard;

esp_now_peer_info_t peerInfo;

telemetryStruct telemetryPacket;

keyboardStruct keyboardPacket;
keyboardStruct previousKeyboardPacket;

//--------------------------------------------------Structs--------------------------------------------------
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

Func ledTask = {0, 0, 0, 0, 0, 10000, 0, 0};
Func espnowTask = {0, 0, 0, 0, 0, 1000, 0, 0};
Func keyboardTask = {0, 0, 0, 0, 0, 1000, 0, 0};
Func uartTask = {0, 0, 0, 0, 0, 20000, 0, 0};

//--------------------------------------------------Variables--------------------------------------------------
//84:F7:03:F0:EF:72
uint8_t dongleAddress[] = {0x58, 0xCF, 0x79, 0xA3, 0x98, 0xC2};

uint8_t receivedData[16];
String success;

uint8_t ledBrightness = 50;

uint8_t layerID = 0;

#endif
