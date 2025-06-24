#ifndef UARTHANDLE_H
#define UARTHANDLE_H

#include "Arduino.h"
#include "WiFi.h"
#include "i2cHandle.h"


static bool debug1 = 0;
static bool debug2 = 0;
static bool debug3 = 0;
static bool debug4 = 0;
static bool debug5 = 0;
static bool debug6 = 0;
static bool debug7 = 0;
static bool debug8 = 0;



#include "espnowHandle.h"



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
  Serial.printf("   h / help : returns available commands\r\n");
  Serial.printf("   info : display all info\r\n");
  Serial.printf("   macaddress / getmacaddress : returns device MAC address\r\n");
  Serial.printf("   setdonglemacaddress : update dongle MAC address, be sure to send values in HEX mode\r\n");
  Serial.printf("   getdonglemacaddress : returns the current dongle MAC address\r\n");
  Serial.printf("   setdeviceid : update deviceID to new value\r\n");
  Serial.printf("   getdeviceid : returns currently set deviceID");
  Serial.printf("   version : returns build date and version\r\n");
  Serial.printf("   power : returns RF Tx power\r\n");
  Serial.printf("   cpu : returns CPU info\r\n");
  Serial.printf("   setcpufreq : set new CPU frequency\r\n");
  Serial.printf("   restart : restart the dongle\r\n");
  Serial.printf("   scan : Scan for side module presence\r\n");
  Serial.printf("   debug1 / debug2... : Toggle debug output. Different kind of debug available\r\n");
}



void printMacAddress()
{
  Serial.print("Device MAC address is: ");
  Serial.println(WiFi.macAddress());
}



void setdonglemacaddress()
{
  Serial.printf("Enter dongle MAC address:\r\n");
  Serial.printf("Be carefull to send in HEX mode, not ASCII\r\n");
  
  for(uint8_t i = 0; i < 6; i++)
  {
    Serial.printf("New value:\r\n");
    while(!Serial.available()){}
    dongleAddress[i] = Serial.read();
    Serial.print(dongleAddress[i], HEX);
    EEPROM.write(DONGLE_MACADDRESS_ADDRESS + i, dongleAddress[i]);
    EEPROM.commit();
  }
  Serial.printf("Recevied dongle MAC address: ");
  for(uint8_t i = 0; i < 6; i++)
  {
    Serial.print(dongleAddress[i], HEX);
    Serial.printf(" ");
  }
  Serial.printf("\r\n");
  registerDongle();
}



void printDongleMacAddress()
{
  Serial.printf("Current dongle MAC address is set to: ");
  for(uint8_t i = 0; i < MAC_ADDRESS_SIZE; i++)
  {
    Serial.print(dongleAddress[i], HEX);
    Serial.printf(" ");
  }
  Serial.printf("\r\n");
}



void setdeviceid()
{
  Serial.printf("Current device ID is: %u\r\n", deviceID);
  Serial.printf("Enter new device ID:\r\n");
  while(!Serial.available()){}
  deviceID = Serial.read() - 48;
  Serial.printf("Setting deviceID to: %u\r\n", deviceID);
  EEPROM.write(DEVICEID_ADDRESS, deviceID);
  EEPROM.commit();
  keyboardPacket.deviceID = deviceID;
  mousePacket.deviceID = deviceID;
}



void printDeviceId()
{
  Serial.printf("Device ID is set to: %u\r\n", deviceID);
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



void setCpuFreq()
{
  Serial.printf("Current CPU frequency: %u MHz\r\n", getCpuFrequencyMhz());
  Serial.printf("Enter new frequency in MHz (MUST BE DEC mode):\r\n");
  Serial.printf("Can be: 240, 160, 80, 40, 20, 10\r\n");
  while(!Serial.available()){}
  setCpuFrequencyMhz(Serial.read());
  Serial.printf("New CPU frequency set to: %u MHZ\r\n", getCpuFrequencyMhz());
}



void printDebugState()
{
  Serial.printf("Debug state:\r\ndebug1: %i debug2: %i debug3: %i debug4: %i debug5: %i debug6: %i debug7: %i debug8: %i\r\n", debug1, debug2, debug3, debug4, debug5, debug6, debug7, debug8); 
}



void restart()
{
  ESP.restart();
}



void scan()
{
  Serial.printf("Scanning for side module\r\n");
  checkSideModule();
}



void handleUart()
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
      case 2090324718: // help
        help();
        break;

      case 2090370257: // info
        printVersion();
        printDeviceId();
        printDongleMacAddress();
        printCpuInfo();
        printRfPower();
        printMacAddress();
        printDebugState();
        break;

      case 1858127548: // getmacaddress
      case 1311181436: // macaddress
        printMacAddress();
        break;

      case 2910204353: // setdonglemacaddress
        setdonglemacaddress();
        break;

      case 1664706229:  // getdonglemacaddress
        printDongleMacAddress();
        break;

      case 1929407563:  // version
        printVersion();
        break;

      case 271097426: // power
        printRfPower();
        break;

      case 193488621: // cpu
        printCpuInfo();
        break;

      case 3688920455: // setcpufreq
        setCpuFreq();
        break;

      case 1059716234: // restart
        restart();
        break;

      case 4159936206:  // setdeviceid
        setdeviceid();
        break;

      case 3080394690:  // getdeviceid
        printDeviceId();
        break;

      case 2090717482: // scan
        scan();
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

#endif