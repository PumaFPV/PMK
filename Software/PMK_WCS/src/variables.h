#ifndef variables_h
#define variables_h

#include "config.h"

#include "WiFi.h"
#include "esp_now.h"
#include "pmk.h"


//--------------------------------------------------Define--------------------------------------------------
//----------Global values
#define MIRCOS2SECONDS 1000000

//----------GPI
#define annularPin 26
#define middlePin 16
#define indexPin 17
#define thumb1Pin 17
#define thumb2Pin 26
#define thumb3Pin 18
#define switchDownPin 19
#define switchUpPin 5

//----------GPO

//----------ADC
#define throttlePin 33

//----------PWM

//----------LED

//----------Peripherals
//---UART
//HardwareSerial debug = HardwareSerial(0);


//---I2C

//---SPI


static const int srSpiClk = 10000000; // 1MHz


//--------------------------------------------------Initialize libraries--------------------------------------------------
esp_now_peer_info_t peerInfo;

telemetryStruct telemetryPacket;
gamepadStruct gamepadPacket;

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

Func gpioTask = {0, 0, 0, 0, 0, 1000, 0, 0};
Func espnowTask = {0, 0, 0, 0, 0, 1000, 0, 0};
Func uartTask = {0, 0, 0, 0, 0, 20000, 0, 0};

struct NoDelay
{
  unsigned long interval;  //in ms
  unsigned long previousMillis;
  unsigned long currentMillis;
};

struct Debounce
{
  uint8_t state;
  uint8_t lastState;
  unsigned long lastDebounceTime;
  unsigned long debounceDelay;
  uint8_t reading;
};

//--------------------------------------------------Variables--------------------------------------------------
//84:F7:03:F0:EF:72
uint8_t dongleAddress[] = {0x58, 0xCF, 0x79, 0xA3, 0x98, 0xC2};

uint8_t receivedData[16];
String success;

#endif