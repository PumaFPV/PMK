#ifndef variables_h
#define variables_h



#include "SPI.h"
#include "WiFi.h"
#include "esp_now.h"
#include "pmk.h"


//--------------------------------------------------Define--------------------------------------------------
#define NUMBER_OF_SR 4  //TODO Add to config file
#define MAX_NUMBER_OF_KEYS 8
#define EEPROM_SIZE 8

//----------Global values
#define MIRCOS2SECONDS 1000000

//----------GPI

//----------GPO

//----------ADC

//----------PWM

//----------LED


//----------Peripherals
//---UART

//---I2C

//---SPI

//--------------------------------------------------Initialize libraries--------------------------------------------------
esp_now_peer_info_t peerInfo;

telemetryStruct telemetryPacket;
//DEFINE YOUR DEVICEPACKET HERE

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

Func ioTask = {0, 0, 0, 0, 0, 10000, 0, 0};
Func espnowTask = {0, 0, 0, 0, 0, 5000, 0, 0};
Func uartTask = {0, 0, 0, 0, 0, 20000, 0, 0};

struct NoDelay
{
  unsigned long interval;  //in ms
  unsigned long previousMillis;
  unsigned long currentMillis;
};



//--------------------------------------------------Variables--------------------------------------------------
uint8_t dongleAddress[] = {};

uint8_t receivedData[16];
String success;



#endif