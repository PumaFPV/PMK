#ifndef UARTHANDLE_H
#define UARTHANDLE_H

#include "Arduino.h"
#include "Esp.h"
#include "WiFi.h"

#include <SPI.h>
#include <SdFat.h>
#include <Adafruit_SPIFlash.h>

// Since SdFat doesn't fully support FAT12 such as format a new flash
// We will use Elm Cham's fatfs f_mkfs() to format
#include "ff.h"
#include "diskio.h"

#include "variables.h"
#include "pmk.h"


// const char formatName[] = "PMK Dongle";
#define DISK_LABEL "PMK Dongle"

static bool debug1 = 0;
static bool debug2 = 0;
static bool debug3 = 0;
static bool debug4 = 0;
static bool debug5 = 0;
static bool debug6 = 0;
static bool debug7 = 0;
static bool debug8 = 0;


unsigned long hash(const char *str) 
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) 
    {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}



void format_fat12(void)
{
  // Working buffer for f_mkfs.
  uint8_t workbuf[4096];

  // Elm Cham's fatfs objects
  FATFS elmchamFatfs;

  // Make filesystem.
  FRESULT r = f_mkfs("", FM_FAT, 0, workbuf, sizeof(workbuf));
  if (r != FR_OK) {
    Serial.print(F("Error, f_mkfs failed with error code: ")); 
    Serial.println(r, DEC);
    while(1) yield();
  }

  // mount to set disk label
  r = f_mount(&elmchamFatfs, "0:", 1);
  if (r != FR_OK) {
    Serial.print(F("Error, f_mount failed with error code: ")); 
    Serial.println(r, DEC);
    while(1) yield();
  }

  // Setting label
  Serial.printf("Setting disk label to: %c\r\n", DISK_LABEL);
  // r = f_setlabel(DISK_LABEL);  // TODO to fix...
  if (r != FR_OK) {
    Serial.print(F("Error, f_setlabel failed with error code: ")); 
    Serial.println(r, DEC);
    while(1) yield();
  }

  // unmount
  f_unmount("0:");

  // sync to make sure all data is written to flash
  flash.syncBlocks();

  Serial.println(F("Formatted flash!"));
}



void check_fat12(void)
{
  // Check new filesystem
  if (!fatfs.begin(&flash)) {
    Serial.println(F("Error, failed to mount newly formatted filesystem!"));
    while(1) delay(1);
  }
}

void help()
{
  Serial.printf("List of available commands:\r\n");
  Serial.printf("   h / help : display available commands\r\n");
  Serial.printf("   macaddress : display dongle MAC address\r\n");
  Serial.printf("   version : display build date and version\r\n");
  Serial.printf("   power : display RF Tx power\r\n");
  Serial.printf("   format : format file system\r\n");
  Serial.printf("   restart : restart the dongle\r\n");
  Serial.printf("   l0 / l1 / l2... : Force the current layer to... If one keybind moves to new layer it will be ignored use l-1 command to reset force layer\r\n");
  Serial.printf("   l-1 : disable forced layer\r\n");
  Serial.printf("   debug1 / debug2... : Toggle debug output. Different kind of debug available\r\n");
}


void handleUart() //TODO
{
  if(Serial.available())
  {
    String command = Serial.readString();

    // Switch is not string compatible so make a hash out of the string
    unsigned long hashCommand = hash(command.c_str());
    Serial.printf("Hash: %lu\r\n", hashCommand);

    switch(hashCommand)
    {
      case 177677: // h
        help();
        break;

      case 2090324718: // help
        help();
        break;

      case 1311181436: //macaddress
        Serial.print("Dongle MAC address is: ");
        Serial.println(WiFi.macAddress());
        break;
      
      case 1929407563:  //version
        Serial.printf("Version: %s build the: %s at %s\r\n", FIRMWARE_REV,  __DATE__, __TIME__);
        break;

      case 271097426: // power
        Serial.printf("WiFi power: ");
        Serial.println(WiFi.getTxPower());
        break;

      case 193488621: // cpu
        Serial.printf("Chip model: %s, chip revision: %u\r\n", ESP.getChipModel(), ESP.getChipRevision());
        Serial.printf("Proc temp: %.1f °C\r\n", temperatureRead());
        Serial.printf("CPU freq: %u MHz\r\n", getCpuFrequencyMhz());
        Serial.printf("XTal freq: %u MHz\r\n", getXtalFrequencyMhz());
        Serial.printf("APB freq: %u Hz\r\n", getApbFrequency());
        Serial.printf("Flash size: %u\r\n", ESP.getFlashChipSize());
        Serial.printf("Sketch size: %u\r\n", ESP.getSketchSize());
        break;

      case 4259722414: // format
        Serial.printf("Formatting the flash for FatFS use...\r\n");
        format_fat12();
        check_fat12();
        Serial.printf("Flash formatted successfully\r\n");
        break;

      case 1059716234: // restart
        ESP.restart();
        break;
        
      case 193496143: //l-1
        forceLayer = -1;
        break;

      case 5863521: // l0
        forceLayer = 0;
        break;

      case 5863522: // l1
        forceLayer = 1;
        break;

      case 5863523: // l2
        forceLayer = 2;
        break;

      case 5863524: // l3
        forceLayer = 3;
        break;

      case 5863525: // l4
        forceLayer = 4;
        break;

      case 5863526: // l5
        forceLayer = 5;
        break;

      case 5863527: // l6
        forceLayer = 6;
        break;

      case 5863528: // l7
        forceLayer = 7;
        break;

      case 4169026269: // debug1
        debug1 = !debug1;
        Serial.printf("debug1 swapped to %i\r\n", debug1);
        break;

      case 4169026270: // debug2
        debug2 = ! debug2;
        break;

      case 4169026271: // debug3
        debug3 = !debug3;
        break;

      case 4169026272: // debug4
        debug4 = !debug4;
        break;

      case 4169026273: // debug5
        debug5 = !debug5;
        break;

      case 4169026274: // debug6
        debug6 = !debug6;
        break;

      case 4169026275: // debug7
        debug7 = !debug7;
        break;

      case 4169026276: // debug8
        debug8 = !debug8;
        break;


    }
  }
}

#endif