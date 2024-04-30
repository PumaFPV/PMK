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



// const char formatName[] = "PMK Dongle";
#define DISK_LABEL "PMK Dongle"


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
  //r = f_setlabel(DISK_LABEL);
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
        case 2318435644: //macaddress
          Serial.print("Dongle MAC address is: ");
          Serial.println(WiFi.macAddress());
          break;
        
        case 1929407563:  //version
          Serial.printf("Version: %s build the: %s at %s\r\n", FIRMWARE_REV,  __DATE__, __TIME__);
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

        case 271097426: // power
          Serial.printf("WiFi power: ");
          Serial.println(WiFi.getTxPower());
          break;

        case 193488621: // cpu
          Serial.printf("Chip model: %s, chip revision: %u\r\n", ESP.getChipModel(), ESP.getChipRevision());
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
    }
  }
}

#endif