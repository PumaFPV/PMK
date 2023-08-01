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
#define LED_DATA_PIN 4



//----------Peripherals
//---UART
//HardwareSerial debug = HardwareSerial(0);


//---I2C

//---SPI

//--------------------------------------------------Initialize libraries--------------------------------------------------

USBHIDKeyboard Keyboard;

packetStruct receivedPacket;

telemetryStruct telemetryPacket;

//keyboardStruct keyboardPacket;
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
Func fsTask = {0, 0, 0, 0, 0, 2000000, 0, 0};
Func telemetryTask = {0, 0, 0, 0, 0, 2000000, 0, 0};


//--------------------------------------------------Variables--------------------------------------------------

uint8_t receivedData[16];

uint8_t ledBrightness = 0;

uint8_t layerID = 0;

const uint8_t connectedDevices = 2;
#define MAC_ADDRESS_SIZE 6

uint8_t deviceAddress[][MAC_ADDRESS_SIZE] = 
{
  {0x84, 0xF7, 0x03, 0xF0, 0xF0, 0xB8}, //leftKeyboard 84:F7:03:F0:F0:B8
  {0x84, 0xF7, 0x03, 0xF0, 0xF0, 0xB0}  //rightKeyboard 84:F7:03:F0:F0:B0
};


#endif
