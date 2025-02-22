#pragma once

#include "Arduino.h"
#include "Esp.h"
#include "WiFi.h"

#include <SPI.h>
#include <SdFat.h>
#include <Adafruit_SPIFlash.h>

// Since SdFat doesn't fully support FAT12 such as format a new flash
// We will use Elm Cham's fatfs f_mkfs() to format
#include "ff.h"
#include "ff.c"
#include "diskio.h"

#include "variables.h"
#include "pmk.h"

#include "configHandle.h"



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



void help()
{
  Serial.printf("List of available commands:\r\n");
  Serial.printf("   h / help : display available commands\r\n");
  Serial.printf("   info : display all info\r\n");
  Serial.printf("   macaddress / getmacaddress : display dongle MAC address\r\n");
  Serial.printf("   version : display build date and version\r\n");
  Serial.printf("   power : display RF Tx power\r\n");
  Serial.printf("   cpu : display cpu info\r\n");
  Serial.printf("   format : format file system\r\n");
  Serial.printf("   restart / reboot : restart the dongle\r\n");
  Serial.printf("   deej : toggle on / off deej\r\n");
  Serial.printf("   battery / batt : display battery level of all connected devices\r\n");
  Serial.printf("   config / load: Reload configuration files\r\n");
  Serial.printf("   l0 / l1 / l2... : Force the current layer to... If one keybind moves to new layer it will be ignored use l-1 command to reset force layer\r\n");
  Serial.printf("   l-1 : disable forced layer\r\n");
  Serial.printf("   debug1 / debug2... : Toggle debug output. Different kind of debug available\r\n");
}



void printMacAddress()
{
  Serial.print("Dongle MAC address is: ");
  Serial.println(WiFi.macAddress());
}



void printVersion()
{
  Serial.printf("Version: %s build the: %s at %s\r\n", FIRMWARE_REV,  __DATE__, __TIME__);
}



void printRfPower()
{
  Serial.printf("WiFi power: ");
  Serial.println(WiFi.getTxPower());
}



void printCpuInfo()
{
  Serial.printf("Chip model: %s, chip revision: %u\r\n", ESP.getChipModel(), ESP.getChipRevision());
  Serial.printf("Proc temp: %.1f °C\r\n", temperatureRead());
  Serial.printf("CPU freq: %u MHz\r\n", getCpuFrequencyMhz());
  Serial.printf("XTal freq: %u MHz\r\n", getXtalFrequencyMhz());
  Serial.printf("APB freq: %u Hz\r\n", getApbFrequency());
  Serial.printf("Flash size: %u\r\n", ESP.getFlashChipSize());
  Serial.printf("Sketch size: %u\r\n", ESP.getSketchSize());
}



void printDebugState()
{
  Serial.printf("Debug state:\r\ndebug1: %i debug2: %i debug3: %i debug4: %i debug5: %i debug6: %i debug7: %i debug8: %i\r\n", debug1, debug2, debug3, debug4, debug5, debug6, debug7, debug8); 
}



void handleUart()
{
  if(Serial.available())
  {
    String command = Serial.readString();

    // Switch is not string compatible so make a hash out of the string
    unsigned long commandHash = hash(command.c_str());
    Serial.printf("Received command hash: %lu\r\n", commandHash);

    switch(commandHash)
    {
      case 177677: // h
        help();
        break;

      case 2090324718: // help
        help();
        break;

      case 2090370257: // info
        printVersion();
        printCpuInfo();
        printRfPower();
        printMacAddress();
        printDebugState();
        break;

      case 1858127548: // getmacaddress
      case 2318435644: // macaddress
        printMacAddress();
        break;
      
      case 1929407563: // version
        printVersion();
        break;

      case 271097426: // power
        printRfPower();
        break;

      case 193488621: // cpu
        printCpuInfo();
        break;

      case 4259722414: // format
        Serial.printf("Formatting the flash for FatFS use...\r\n");
        format_fat12();
        check_fat12();
        Serial.printf("Flash formatted successfully\r\n");
        break;

      case 1059716234: // restart
      case 421948272: // reboot
        ESP.restart();
        break;

      case 2090180733: // deej
        deejToggle = !deejToggle;
        break;

      case 2090105008:  // batt
      case 1715713920:  // battery
        battery = !battery;
        break;
      
      case 4142165115: // config
      case 2090478981: // load
        loadConfiguration();
        configDeej();
        break;
        
      case 193496143: //l-1
        forceLayer = -1;
        Serial.printf("Forced layer to %i\r\n", forceLayer);
        break;

      case 5863521: // l0
        forceLayer = 0;
        Serial.printf("Forced layer to %i\r\n", forceLayer);
        break;

      case 5863522: // l1
        forceLayer = 1;
        Serial.printf("Forced layer to %i\r\n", forceLayer);
        break;

      case 5863523: // l2
        forceLayer = 2;
        Serial.printf("Forced layer to %i\r\n", forceLayer);
        break;

      case 5863524: // l3
        forceLayer = 3;
        Serial.printf("Forced layer to %i\r\n", forceLayer);
        break;

      case 5863525: // l4
        forceLayer = 4;
        Serial.printf("Forced layer to %i\r\n", forceLayer);
        break;

      case 5863526: // l5
        forceLayer = 5;
        Serial.printf("Forced layer to %i\r\n", forceLayer);
        break;

      case 5863527: // l6
        forceLayer = 6;
        Serial.printf("Forced layer to %i\r\n", forceLayer);
        break;

      case 5863528: // l7
        forceLayer = 7;
        Serial.printf("Forced layer to %i\r\n", forceLayer);
        break;

      case 4169026269: // debug1
        debug1 = !debug1;
        Serial.printf("debug1 swapped to %i\r\n", debug1);
        break;

      case 4169026270: // debug2
        debug2 = ! debug2;
        Serial.printf("debug2 swapped to %i\r\n", debug2);
        break;

      case 4169026271: // debug3
        debug3 = !debug3;
        Serial.printf("debug3 swapped to %i\r\n", debug3);
        break;

      case 4169026272: // debug4
        debug4 = !debug4;
        Serial.printf("debug4 swapped to %i\r\n", debug4);
        break;

      case 4169026273: // debug5
        debug5 = !debug5;
        Serial.printf("debug5 swapped to %i\r\n", debug5);
        break;

      case 4169026274: // debug6
        debug6 = !debug6;
        Serial.printf("debug6 swapped to %i\r\n", debug6);
        break;

      case 4169026275: // debug7
        debug7 = !debug7;
        Serial.printf("debug7 swapped to %i\r\n", debug7);
        break;

      case 4169026276: // debug8
        debug8 = !debug8;
        Serial.printf("debug8 swapped to %i\r\n", debug8);
        break;

    }
  }
}