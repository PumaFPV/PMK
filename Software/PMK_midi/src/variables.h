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
#define slider1 39
#define slider2 36
#define slider3 32
#define slider4 34
#define slider5 35

uint8_t sliderPin[5] = {slider1, slider2, slider3, slider4, slider5};


//----------PWM

//----------LED


//----------Peripherals
//---UART

//---I2C

//---SPI

//--------------------------------------------------Initialize libraries--------------------------------------------------
esp_now_peer_info_t peerInfo;

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

Func ioTask = {0, 0, 0, 0, 0, 100000, 0, 0};
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



bool areArraysEqual(uint8_t* arr1, uint8_t* arr2, int size) {
    for (int i = 0; i < size; i++) {
        if (arr1[i] != arr2[i]) {
            return false;
        }
    }
    return true;
}



#endif