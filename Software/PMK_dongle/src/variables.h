#ifndef variables_h
#define variables_h

#include "USB.h"
#include "USBHIDKeyboard.h"
//#include "USBMSC.h"
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
#define LED_DATA_PIN 9



//----------Peripherals
//---UART
//HardwareSerial debug = HardwareSerial(0);


//---I2C

//---SPI

//--------------------------------------------------Initialize libraries--------------------------------------------------

//USBMSC MSC;
FirmwareMSC MSC_Update;

// ESP32 use same flash device that store code.
// Therefore there is no need to specify the SPI and SS
Adafruit_FlashTransport_ESP32 flashTransport;
Adafruit_SPIFlash flash(&flashTransport);

// file system object from SdFat
FatVolume fatfs;

// USB Mass Storage object
Adafruit_USBD_MSC usb_msc;

bool fs_formatted;  // Check if flash is formatted
bool fs_changed;    // Set to true when browser write to flash

File32 fsUploadFile;


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

#define MAC_ADDRESS_SIZE 6

#endif