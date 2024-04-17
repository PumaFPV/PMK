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
  RID_GAMEPAD
};

// HID report descriptor using TinyUSB's template
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(RID_KEYBOARD)),
  TUD_HID_REPORT_DESC_MOUSE   (HID_REPORT_ID(RID_MOUSE)),
  TUD_HID_REPORT_DESC_CONSUMER( HID_REPORT_ID(RID_CONSUMER_CONTROL) ),
  TUD_HID_REPORT_DESC_GAMEPAD( HID_REPORT_ID(RID_GAMEPAD)) //Not yet working...
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
