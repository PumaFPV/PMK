#ifndef VARIABLE_H
#define VARIABLE_H

#include "WiFi.h"
#include "esp_now.h"



//--------------------------------------------------Define--------------------------------------------------
//----------Global values
#define MIRCOS2SECONDS 1000000

//----------GPI

//----------GPO

//----------ADC

//----------PWM

//----------LED
#define LED_DATA_PIN 9
#define LED_BUILDIN LED_DATA_PIN


//----------Peripherals
//---UART
//HardwareSerial debug = HardwareSerial(0);


//---I2C

//---SPI

//--------------------------------------------------Initialize libraries--------------------------------------------------

// USB HID object
// desc report, desc len, protocol, interval, use out endpoint
// Report ID
enum
{
  RID_3DMOUSE = 0,
  RID_KEYBOARD = 0x03,
  RID_MOUSE,
  RID_CONSUMER_CONTROL, // Media, volume etc ..
  RID_GAMEPAD,
};



#define HID_REPORT_DESC_SPACE_MOUSE \
  HID_USAGE_PAGE(HID_USAGE_PAGE_DESKTOP),\
  HID_USAGE(HID_USAGE_DESKTOP_MULTI_AXIS_CONTROLLER),\
  HID_COLLECTION(HID_COLLECTION_APPLICATION),\
    HID_COLLECTION(HID_COLLECTION_PHYSICAL),\
      HID_REPORT_ID(1)\
      HID_LOGICAL_MIN_N(0xA2FE, 2),\
      HID_LOGICAL_MAX_N(0x5E01, 2),\
      HID_PHYSICAL_MIN_N(0x88FA, 2),\
      HID_PHYSICAL_MAX_N(0x7805, 2),\
      HID_UNIT_EXPONENT(0x0C),\
      HID_UNIT(0x11),\
      HID_USAGE(HID_USAGE_DESKTOP_X),\
      HID_USAGE(HID_USAGE_DESKTOP_Y),\
      HID_USAGE(HID_USAGE_DESKTOP_Z),\
      HID_REPORT_SIZE(16),\
      HID_REPORT_COUNT(3),\
      HID_INPUT(0x06),\
    HID_COLLECTION_END,\
    HID_COLLECTION(HID_COLLECTION_PHYSICAL),\
      HID_REPORT_ID(2)\
      HID_USAGE(HID_USAGE_DESKTOP_RX),\
      HID_USAGE(HID_USAGE_DESKTOP_RY),\
      HID_USAGE(HID_USAGE_DESKTOP_RZ),\
      HID_REPORT_SIZE(16),\
      HID_REPORT_COUNT(3),\
      HID_INPUT(0x06),\
    HID_COLLECTION_END,\
  HID_COLLECTION_END
 


uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(RID_KEYBOARD)),
  TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(RID_MOUSE)),
  TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(RID_CONSUMER_CONTROL)),
  TUD_HID_REPORT_DESC_GAMEPAD(HID_REPORT_ID(RID_GAMEPAD)),
  HID_REPORT_DESC_SPACE_MOUSE
};

// USB Device
//Adafruit_USBD_Device usb;

// USB Mass Storage object
Adafruit_USBD_MSC usb_msc;

// USB HID object
Adafruit_USBD_HID usb_hid;

hid_gamepad_report_t gp;

// ESP32 use same flash device that store code.
// Therefore there is no need to specify the SPI and SS
Adafruit_FlashTransport_ESP32 flashTransport;
Adafruit_SPIFlash flash(&flashTransport);

// file system object from SdFat
FatVolume fatfs;

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
Func espnowTask = {0, 0, 0, 0, 0, 100000, 0, 0};
Func uartTask = {0, 0, 0, 0, 0, 100000, 0, 0};
Func fsTask = {0, 0, 0, 0, 0, 2000000, 0, 0};
Func telemetryTask = {0, 0, 0, 0, 0, 2000000, 0, 0};



//--------------------------------------------------Variables--------------------------------------------------

uint8_t receivedData[16];

static bool deejToggle = 0;
static bool debug1 = 0;
static bool debug2 = 0;
static bool debug3 = 0;
static bool debug4 = 0;
static bool debug5 = 0;
static bool debug6 = 0;
static bool debug7 = 0;
static bool debug8 = 0;

#define MAC_ADDRESS_SIZE 6

#endif