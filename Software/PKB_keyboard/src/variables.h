#ifndef variables_h
#define variables_h

#include "SPI.h"
#include "USB.h"
#include "USBHIDKeyboard.h"
#include "WiFi.h"
#include "esp_now.h"
#include "FastLED.h"
#include "pmk.h"


//--------------------------------------------------Define--------------------------------------------------
//----------Global values
#define MIRCOS2SECONDS 1000000

//----------GPI

//----------GPO

//----------ADC

//----------PWM

//----------LED
#define NUM_LEDS 39
#define LED_DATA_PIN 4
#define UPDATES_PER_SECOND 30


//----------Peripherals
//---UART
//HardwareSerial debug = HardwareSerial(0);


//---I2C

//---SPI
#define SR_SPI_BUS 1 //Which SPI bus to use for this SPI object
#define SR_MISO 3
#define SR_CLK 7
#define SR_CE 9
#define SR_PL 5

static const int srSpiClk = 10000000; // 10MHz


//--------------------------------------------------Initialize libraries--------------------------------------------------
SPIClass * srSpi = NULL;
SPISettings settingsA(srSpiClk, MSBFIRST, SPI_MODE0);

USBHIDKeyboard Keyboard;

CRGB leds[NUM_LEDS];
CRGBPalette16 currentPalette;
TBlendType    currentBlending;

esp_now_peer_info_t peerInfo;

telemetryStruct telemetryPacket;
keyboardStruct keyboardPacket;
keyboardStruct previousKeyboardPacket;
keyboardStruct releaseKeyboardPacket;

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
Func srTask = {0, 0, 0, 0, 0, 1000, 0, 0};
Func espnowTask = {0, 0, 0, 0, 0, 1000, 0, 0};
Func uartTask = {0, 0, 0, 0, 0, 20000, 0, 0};

struct NoDelay
{
  unsigned long interval;  //in ms
  unsigned long previousMillis;
  unsigned long currentMillis;
};

NoDelay pulsarNoDelay = {6, 0, 0};

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

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

uint8_t receivedData[16];
String success;

uint8_t spiPacket[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t numberOfPressedKeys = 0;

uint8_t ledNumber = 0;
uint8_t ledBrightness = 50;

uint8_t layerID = 0;

//------------------------------------------------No delay



#endif