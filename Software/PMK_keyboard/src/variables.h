#ifndef VARIABLES_H
#define VARIABLES_H



#include "SPI.h"
#include "WiFi.h"
#include "esp_now.h"
#include "FastLED.h"
#include "pmk.h"

#ifdef HW01
//--------------------------------------------------Define--------------------------------------------------
#define NUMBER_OF_SR 4  //TODO Add to config file
#define MAX_NUMBER_OF_KEYS 8
#define EEPROM_SIZE 8

#define LEFT_KB 1
#define RIGHT_KB 2
//----------Global values
#define MIRCOS2SECONDS 1000000
#define NUM_LEDS 29

//----------GPI
#define RE_A 15
#define RE_B 16

//----------GPO
#define LED_DATA_PIN 4

//----------ADC

//----------PWM

//----------LED
#define UPDATES_PER_SECOND 30


//----------Peripherals
//---UART

//---I2C

//---SPI

#define SR_SPI_BUS 1 //Which SPI bus to use for this SPI object
#define SR_MISO 3
#define SR_CLK 7
#define SR_CE 9
#define SR_PL 5
#endif

#ifdef HW02
//--------------------------------------------------Define--------------------------------------------------
#define NUMBER_OF_SR 4  //TODO Add to config file
#define MAX_NUMBER_OF_KEYS 8
#define EEPROM_SIZE 8

#define LEFT_KB 1
#define RIGHT_KB 2
//----------Global values
#define MIRCOS2SECONDS 1000000
#define NUM_LEDS 30

//----------GPI
#define RE_B 15
#define RE_A 16
#define DR 10
#define GP_CE 11
#define GPIO1 12
#define GPIO2 13
#define GPIO3 14

//----------GPO
#define LED_DATA_PIN 21

//----------ADC

//----------PWM

//----------LED
#define UPDATES_PER_SECOND 30


//----------Peripherals
//---UART

//---I2C
#define SDA 6
#define SCL 7

//---SPI
#define SR_SPI_BUS 1 //Which SPI bus to use for this SPI object
#define SPI_MOSI 2
#define SR_MISO 3
#define SR_CLK 4
#define SR_CE 5
#define SR_PL 8

#endif
static const int i2cClk = 400000; // 400kHz 
static const int srSpiClk = 10000000; // 1MHz

#define TRACKPAD_ADDRESS 0x2A
#define LDC1614_ADDRESS 0x2A
#define LDC1612_ADDRESS 0x2B

//--------------------------------------------------Initialize libraries--------------------------------------------------
SPIClass * srSpi = NULL;
SPISettings settingsA(srSpiClk, MSBFIRST, SPI_MODE2);

CRGB leds[NUM_LEDS];
CRGBPalette16 currentPalette;
TBlendType    currentBlending;

esp_now_peer_info_t peerInfo;

//-----Cirque trackpad init
PinnacleTouchI2C trackpad(DR);

AbsoluteReport data;
AbsoluteReport prevData;



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

typedef enum {
  pulsarPurple = 0xfd26e9,
  pulsarBlue = 0x00f1ff,
  pulsarPurpleBlue = 0xada8f0
} HTMLColorCode;

Func ledTask = {0, 0, 0, 0, 0, 20000, 0, 0};
Func srTask = {0, 0, 0, 0, 0, 10000, 0, 0};
Func espnowTask = {0, 0, 0, 0, 0, 5000, 0, 0};
Func reTask = {0, 0, 0, 0, 0, 100000, 0, 0};
Func uartTask = {0, 0, 0, 0, 0, 20000, 0, 0};
Func sideModuleTask = {0, 0, 0, 0, 0, 10000, 0, 0};

struct NoDelay
{
  unsigned long interval;  //in ms
  unsigned long previousMillis;
  unsigned long currentMillis;
};

NoDelay pulsarNoDelay = {6, 0, 0};



//--------------------------------------------------Variables--------------------------------------------------
uint8_t dongleAddress[MAC_ADDRESS_SIZE] = {};

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

uint8_t receivedData[16];
String success;

uint8_t spiPacket[4] = {0x00, 0x00, 0x00, 0x00};
uint8_t numberOfPressedKeys = 0;

uint8_t ledNumber = 0;

uint8_t rotary = 0;

uint8_t deviceID;

//--------------------------------------------------SideModule
bool trackpadIsPresent = 0;
bool spacemouseIsPresent = 0;

#endif