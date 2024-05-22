#ifndef VARIABLE_H
#define VARIABLE_H

//#include "USB.h"
//#include "USBHIDKeyboard.h"
//#include "USBMSC.h"
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

// USB HID object. For ESP32 these values cannot be changed after this declaration
// desc report, desc len, protocol, interval, use out endpoint
// Report ID
enum
{
  RID_KEYBOARD = 1,
  RID_MOUSE,
  RID_CONSUMER_CONTROL, // Media, volume etc ..
  RID_GAMEPAD,
};


/*
#define HID_REPORT_DESC_MAGELLAN \
  0x05, 0x01, 0x09, 0x08, 0xA1, 0x01, 0xA1, 0x00, 0x85, 0x01, 0x16, 0xA2, 0xFE, 0x26, 0x5E, 0x01, 0x36, 0x88, 0xFA, \
  0x46, 0x78, 0x05, 0x55, 0x0C, 0x65, 0x11, 0x09, 0x30, 0x09, 0x31, 0x09, 0x32, 0x75, 0x10, 0x95, 0x03, 0x81, 0x06, \
  0xC0, 0xA1, 0x00, 0x85, 0x02, 0x09, 0x33, 0x09, 0x34, 0x09, 0x35, 0x75, 0x10, 0x95, 0x03, 0x81, 0x06, 0xC0, 0xA1, \
  0x02, 0x85, 0x03, 0x05, 0x01, 0x05, 0x09, 0x19, 0x01, 0x29, 0x03, 0x15, 0x00, 0x25, 0x01, 0x35, 0x00, 0x45, 0x01, \
  0x75, 0x01, 0x95, 0x03, 0x81, 0x02, 0x95, 0x01, 0x81, 0x03, 0x19, 0x05, 0x29, 0x06, 0x95, 0x02, 0x81, 0x02, 0x95, \
  0x02, 0x81, 0x03, 0x09, 0x09, 0x95, 0x01, 0x81, 0x02, 0x95, 0x03, 0x81, 0x03, 0x19, 0x0D, 0x29, 0x10, 0x95, 0x04, \
  0x81, 0x02, 0x95, 0x06, 0x81, 0x03, 0x19, 0x17, 0x29, 0x1B, 0x95, 0x05, 0x81, 0x02, 0x95, 0x15, 0x81, 0x03, 0xC0, \
  0x01, 0x75, 0x07, 0x91, 0x03, 0xC0, 0x06, 0x00, 0x09, 0x01, 0xA1, 0x02, 0x15, 0x80, 0x25, 0x7F, 0x75, 0x08, 0x09, \
  0x3A, 0xA1, 0x02, 0x85, 0x05, 0x09, 0x20, 0x95, 0x01, 0xB1, 0x02, 0xC0, 0xA1, 0x02, 0x85, 0x06, 0x09, 0x21, 0x95, \
  0x01, 0xB1, 0x02, 0xC0, 0xA1, 0x02, 0x85, 0x07, 0x09, 0x22, 0x95, 0x01, 0xB1, 0x02, 0xC0, 0xA1, 0x02, 0x85, 0x08, \
  0x09, 0x23, 0x95, 0x07, 0xB1, 0x02, 0xC0, 0xA1, 0x02, 0x85, 0x09, 0x09, 0x24, 0x95, 0x07, 0xB1, 0x02, 0xC0, 0xA1, \
  0x02, 0x85, 0x0A, 0x09, 0x25, 0x95, 0x07, 0xB1, 0x02, 0xC0, 0xA1, 0x02, 0x85, 0x0B, 0x09, 0x26, 0x95, 0x01, 0xB1, \
  0x02, 0xC0, 0xA1, 0x02, 0x85, 0x13, 0x09, 0x2E, 0x95, 0x01, 0xB1, 0x02, 0xC0, 0xA1, 0x02, 0x85, 0x14, 0x09, 0x2F, \
  0x95, 0x04, 0xB1, 0x02, 0xC0, 0xA1, 0x02, 0x85, 0x15, 0x09, 0x30, 0x95, 0x01, 0xB1, 0x02, 0xC0, 0xA1, 0x02, 0x85, \
  0x16, 0x19, 0x01, 0x29, 0x1F, 0x15, 0x00, 0x25, 0x01, 0x35, 0x00, 0x45, 0x01, 0x75, 0x01, 0x95, 0x1F, 0x81, 0x02, \
  0x95, 0x01, 0x81, 0x03, 0xC0, 0xC0, 0xC0,
*/

#define HID_REPORT_DESC_MAGELLAN \
  HID_USAGE_PAGE (HID_USAGE_PAGE_DESKTOP),\
  HID_USAGE(HID_USAGE_DESKTOP_MULTI_AXIS_CONTROLLER),\
  HID_COLLECTION (HID_COLLECTION_APPLICATION),\
    HID_COLLECTION (HID_COLLECTION_PHYSICAL),\
      /* Maybe need to be report id 1*/ \
      HID_REPORT_ID(1) \ 
      HID_LOGICAL_MIN_N(-350, 2),\
      HID_LOGICAL_MAX_N(350, 2),\
      HID_PHYSICAL_MIN_N(-1400, 2),\
      HID_PHYSICAL_MAX_N(1400, 2),\
      HID_UNIT_EXPONENT(0x0C),\
      HID_UNIT(0x11),\
      HID_USAGE(HID_USAGE_DESKTOP_X),\
      HID_USAGE(HID_USAGE_DESKTOP_Y),\
      HID_USAGE(HID_USAGE_DESKTOP_Z),\
      HID_REPORT_SIZE(0x10),\
      HID_REPORT_COUNT(0x03),\
      HID_INPUT(0x06),\
    HID_COLLECTION_END,\
    HID_COLLECTION(HID_COLLECTION_PHYSICAL),\
    \
      HID_REPORT_ID(2) \
      HID_USAGE(HID_USAGE_DESKTOP_RX),\
      HID_USAGE(HID_USAGE_DESKTOP_RY),\
      HID_USAGE(HID_USAGE_DESKTOP_RZ),\
      HID_REPORT_SIZE(0x10),\
      HID_REPORT_COUNT(0x03),\
      HID_INPUT(0x06),\
    HID_COLLECTION_END,\
    HID_COLLECTION(HID_COLLECTION_PHYSICAL),\
      HID_REPORT_ID(3) \
      HID_USAGE_PAGE(HID_USAGE_PAGE_DESKTOP),\
      HID_USAGE_PAGE(HID_USAGE_PAGE_BUTTON),\
      HID_USAGE_MIN(0x01),\
      HID_USAGE_MAX(0x03),\
      HID_LOGICAL_MIN(0),\
      HID_LOGICAL_MAX(1),\
      HID_PHYSICAL_MIN(0),\
      HID_PHYSICAL_MAX(1),\
      HID_REPORT_SIZE(0x01),\
      HID_REPORT_COUNT(0x03),\
      HID_INPUT(0x02),\
      HID_REPORT_COUNT(0x01),\
      HID_INPUT(0x03),\
      HID_USAGE_MIN(0x05),\
      HID_USAGE_MAX(0x06),\
      HID_REPORT_COUNT(0x02),\
      HID_INPUT(0x02),\
      HID_REPORT_COUNT(0x02),\
      HID_INPUT(0x03),\
      HID_USAGE(0x09),\
      HID_REPORT_COUNT(0x01),\
      HID_INPUT(0x02),\
      HID_REPORT_COUNT(0x03),\
      HID_INPUT(0x03),\
      HID_USAGE_MIN(0x0D),\
      HID_USAGE_MAX(0x10),\
      HID_REPORT_COUNT(0x04),\
      HID_INPUT(0x02),\
      HID_REPORT_COUNT(0x06),\
      HID_INPUT(0x03),\
      HID_USAGE_MIN(0x17),\
      HID_USAGE_MAX(0x1B),\
      HID_REPORT_COUNT(0x05),\
      HID_INPUT(0x02),\
      HID_REPORT_COUNT(0x15),\
      HID_INPUT(0x03),\
    HID_COLLECTION_END,\
    HID_REPORT_ID(4) \
    HID_USAGE_PAGE(0x08),\
    HID_USAGE(0x4B),\
    HID_LOGICAL_MIN(0),\
    HID_LOGICAL_MAX(1),\
    HID_REPORT_COUNT(0x01),\
    HID_REPORT_SIZE(0x01),\
    HID_OUTPUT(0x02),\
    HID_REPORT_COUNT(0x01),\
    HID_REPORT_SIZE(0x07),\
    HID_OUTPUT(0x03),\
  HID_COLLECTION_END,\
  HID_USAGE_PAGE(0xFF),\
  HID_USAGE(0x01),\
  HID_COLLECTION(HID_COLLECTION_LOGICAL),\
    HID_LOGICAL_MIN(0x80),\
    HID_LOGICAL_MAX(0x7F),\
    HID_REPORT_SIZE(0x08),\
    HID_USAGE(0x3A),\
    HID_COLLECTION(HID_COLLECTION_LOGICAL),\
      HID_REPORT_ID(5) \
      HID_USAGE(0x20),\
      HID_REPORT_COUNT(0x01),\
      HID_FEATURE(0x02),\
    HID_COLLECTION_END,\
    HID_COLLECTION(HID_COLLECTION_LOGICAL),\
      HID_REPORT_ID(6) \
      HID_USAGE(0x21),\
      HID_REPORT_COUNT(0x01),\
      HID_FEATURE(0x02),\
    HID_COLLECTION_END,\
    HID_COLLECTION(HID_COLLECTION_LOGICAL),\
      HID_REPORT_ID(7) \
      HID_USAGE(0x22),\
      HID_REPORT_COUNT(0x01),\
      HID_FEATURE(0x02),\
    HID_COLLECTION_END,\
    HID_COLLECTION(HID_COLLECTION_LOGICAL),\
      HID_REPORT_ID(8) \
      HID_USAGE(0x23),\
      HID_REPORT_COUNT(0x07),\
      HID_FEATURE(0x02),\
    HID_COLLECTION_END,\
    HID_COLLECTION(HID_COLLECTION_LOGICAL),\
      HID_REPORT_ID(9) \
      HID_USAGE(0x24),\
      HID_REPORT_COUNT(0x07),\
      HID_FEATURE(0x02),\
    HID_COLLECTION_END,\
    HID_COLLECTION(HID_COLLECTION_LOGICAL),\
      HID_REPORT_ID(10) \
      HID_USAGE(0x25),\
      HID_REPORT_COUNT(0x07),\
      HID_FEATURE(0x02),\
    HID_COLLECTION_END,\
    HID_COLLECTION(HID_COLLECTION_LOGICAL),\
      HID_REPORT_ID(11) \
      HID_USAGE(0x26),\
      HID_REPORT_COUNT(0x07),\
      HID_FEATURE(0x02),\
    HID_COLLECTION_END,\
    HID_COLLECTION(HID_COLLECTION_LOGICAL),\
      HID_REPORT_ID(19) \
      HID_USAGE(0x2E),\
      HID_REPORT_COUNT(0x01),\
      HID_FEATURE(0x02),\
    HID_COLLECTION_END,\
    HID_COLLECTION(HID_COLLECTION_LOGICAL),\
      HID_REPORT_ID(20) \
      HID_USAGE(0x2F),\
      HID_REPORT_COUNT(0x04),\
      HID_FEATURE(0x02),\
    HID_COLLECTION_END,\
    HID_COLLECTION(HID_COLLECTION_LOGICAL),\
      HID_REPORT_ID(21) \
      HID_USAGE(0x30),\
      HID_REPORT_COUNT(0x01),\
      HID_FEATURE(0x02),\
    HID_COLLECTION_END,\
    HID_COLLECTION(HID_COLLECTION_LOGICAL),\
      HID_REPORT_ID(22) \
      HID_USAGE_MIN(0x01),\
      HID_USAGE_MAX(0x1F),\
      HID_LOGICAL_MIN(0x00),\
      HID_LOGICAL_MAX(0x01),\
      HID_PHYSICAL_MIN(0x00),\
      HID_PHYSICAL_MAX(0x01),\
      HID_REPORT_SIZE(0x01),\
      HID_REPORT_COUNT(0x1F),\
      HID_INPUT(0x02),\
      HID_REPORT_COUNT(0x01),\
      HID_INPUT(0x03),\
      HID_COLLECTION_END,\
    HID_COLLECTION_END,\
  HID_COLLECTION_END \
  


// HID report descriptor using TinyUSB's template

uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(RID_KEYBOARD)),
  TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(RID_MOUSE)),
  TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(RID_CONSUMER_CONTROL)),
  TUD_HID_REPORT_DESC_GAMEPAD(HID_REPORT_ID(RID_GAMEPAD)),
  //HID_REPORT_DESC_MAGELLAN
};

// USB HID object
Adafruit_USBD_HID usb_hid;

hid_gamepad_report_t gp;

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
Func keyboardTask = {0, 0, 0, 0, 0, 10000, 0, 0};
Func uartTask = {0, 0, 0, 0, 0, 20000, 0, 0};
Func fsTask = {0, 0, 0, 0, 0, 2000000, 0, 0};
Func telemetryTask = {0, 0, 0, 0, 0, 2000000, 0, 0};

    
//--------------------------------------------------Variables--------------------------------------------------

uint8_t receivedData[16];

#define MAC_ADDRESS_SIZE 6

#endif