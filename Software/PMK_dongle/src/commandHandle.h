#ifndef COMMANDHANDLE_H
#define COMMANDHANDLE_H

#include "SimpleCLI.h"

#include "Arduino.h"
#include "Esp.h"
#include "WiFi.h"

#include <SPI.h>
#include <SdFat.h>
#include <Adafruit_SPIFlash.h>
#include "rtc.h"

// Since SdFat doesn't fully support FAT12 such as format a new flash
// We will use Elm Cham's fatfs f_mkfs() to format
#include "ff.h"
#include "ff.c"
#include "diskio.h"

#include "variables.h"
#include "pmk.h"

#include "configHandle.h"



SimpleCLI cli;

Command cmdHelp;
Command cmdInfo;
Command cmdMacAddress;
Command cmdVersion;
Command cmdPower;
Command cmdCpu;
Command cmdFormat;
Command cmdSetCpuFreq;
Command cmdRestart;
Command cmdDeej;
Command cmdBattery;
Command cmdConfig;
Command cmdLayer;
Command cmdEscape;
Command cmdDebug;



void helpCallback(cmd* cmdPtr)
{
    Command cmd(cmdPtr);

    Serial.printf("List of available commands:\r\n");
    Serial.printf("   h / help : display available commands\r\n");
    Serial.printf("   info : display all info\r\n");
    Serial.printf("   macaddress / getmacaddress : display dongle MAC address\r\n");
    Serial.printf("   ver / version : display build date and version\r\n");
    Serial.printf("   power : display RF Tx power\r\n");
    Serial.printf("   cpu : display cpu info\r\n");
    Serial.printf("   format : format file system\r\n");
    Serial.printf("   setcpufreq : set new CPU frequency in MHz\r\n");
    Serial.printf("   restart / reboot : restart the dongle\r\n");
    Serial.printf("   deej : toggle on / off deej\r\n");
    Serial.printf("   battery / batt : display battery level of all connected devices\r\n");
    Serial.printf("   config / load: Reload configuration files\r\n");
    Serial.printf("   l 0 / L 0 / l 1 / L 1 / l 2... : Force the current layer to... If one keybind moves to new layer it will be ignored use l-1 command to reset force layer\r\n");
    Serial.printf("   l -1 / esc : disable forced layer\r\n");
    Serial.printf("   debug 1 / 2... : Toggle debug output. Different kind of debug available\r\n");
}



void cpuCallback(cmd* cmdPtr)
{
    Serial.printf("Chip model: %s, chip revision: %u\r\n", ESP.getChipModel(), ESP.getChipRevision());
    Serial.printf("Proc temp: %.1f °C\r\n", temperatureRead());
    Serial.printf("CPU freq: %u MHz\r\n", getCpuFrequencyMhz());
    Serial.printf("XTal freq: %u MHz\r\n", getXtalFrequencyMhz());
    Serial.printf("APB freq: %u Hz\r\n", getApbFrequency());
    Serial.printf("Flash size: %u\r\n", ESP.getFlashChipSize());
    Serial.printf("Sketch size: %u\r\n\r\n", ESP.getSketchSize());
}



void macaddressCallback(cmd* cmdPtr)
{
    Command cmd(cmdPtr);

    Serial.print("Device MAC address is: ");
    Serial.println(WiFi.macAddress());
    Serial.printf("\r\n");
}



void formatCallback(cmd* cmdPtr)
{
    Command cmd(cmdPtr);
    
    Serial.printf("You are about to format the content of the flash of the dongle. \r\n");
    Serial.printf("Are you sure? (y / n)\r\n");
    Serial.printf("Once formated, you will loose your layer configurations.\r\n");
    Serial.printf("Changes in layer configuration will only take effect once you reload config by either using the \"reload\" command or by restarting the dongle.\r\n");

    static char receivedCommand[64];
    static uint8_t i = 0;
    static String command;
    
    while(!Serial.available());

    uint8_t incominSerial = Serial.read();

    if (incominSerial == 0x79) // y
    {
        Serial.printf("Formatting the flash for FatFS use...\r\n");
        format_fat12();
        check_fat12();
        Serial.printf("Flash formatted successfully\r\n\r\n");
    }
    else
    {
        Serial.printf("Not formatting flash.\r\n\r\n");
    }    
}



void versionCallback(cmd* cmdPtr)
{
    Command cmd(cmdPtr);

    Serial.printf("Version: %s build the: %s at %s\r\n\r\n", FIRMWARE_REV,  __DATE__, __TIME__);
}



void powerCallback(cmd* cmdPtr)
{
    Command cmd(cmdPtr);

    Serial.printf("WiFi power: ");
    Serial.println(WiFi.getTxPower());
    Serial.printf("\r\n");
}



void setcpufreqCallback(cmd* cmdPtr)
{
    Command cmd(cmdPtr);
    Argument arg = cmd.getArgument(0);

    if(arg.isSet())
    {
        uint32_t cpuFreq = arg.getValue().toInt();
        switch (cpuFreq)
        {
            case 240:   // optimized by AI
            case 160:
            case 80:
            case 40:
            case 20:
            case 10:
                break;

            default:
                uint8_t xtal = getXtalFrequencyMhz();
                //Serial.printf("CPU clock could not be set to %u MHz. Supported frequencies: %s\r\n", cpuFreq, getSupportedCpuFrequencyMhz(xtal));
                Serial.printf("Invalid frequency entered. Can only be: 240, 160, 80, 40, 20, 10\r\n");
                return;
        }

        setCpuFrequencyMhz(cpuFreq);
        Serial.printf("New CPU frequency set to: %u MHZ\r\n\r\n", getCpuFrequencyMhz());
    }
    else
    {
        Serial.printf("Current CPU frequency: %u MHz\r\n\r\n", getCpuFrequencyMhz());
        Serial.printf("You can set custom CPU frequency by adding it as an argument.\r\n");
        Serial.printf("Valid frequencies are: 240, 160, 80, 40, 20, 10\r\n");
    }
    
}



void restartCallback(cmd* cmdPtr)
{
    Command cmd(cmdPtr);

    Serial.printf("Restarting...\r\n\r\n");
    ESP.restart();
}



void debugCallback(cmd* cmdPtr)
{
    Command cmd(cmdPtr);
    Argument arg = cmd.getArgument(0);
    
    uint8_t debugIndex = arg.getValue().toInt();
    
    if(arg.isSet())
    {
        if(0 < debugIndex && debugIndex < 9)
        {
            debug[debugIndex-1] = !debug[debugIndex-1];
            Serial.printf("debug %i swapped to %i\r\n", debugIndex, debug[debugIndex-1]);
        }
        else
        {
            Serial.printf("Invalid debug value.\r\nExpected values from 1 to 8.\r\n");
        }
    }

    Serial.printf("Debug state:\r\ndebug1: %i debug2: %i debug3: %i debug4: %i debug5: %i debug6: %i debug7: %i debug8: %i\r\n\r\n", debug[0], debug[1], debug[2], debug[3], debug[4], debug[5], debug[6], debug[7]); 
}



void deejCallback(cmd* cmdPtr)
{
    Command cmd(cmdPtr);

    Serial.printf("Toggling deej output\r\n");
    deejToggle = !deejToggle;
}



void batteryCallback(cmd* cmdPtr)
{
    Command cmd(cmdPtr);

    Serial.printf("Toggling battery info\r\n");
    battery = !battery;
}



void configCallback(cmd* cmdPtr)
{
    Command cmd(cmdPtr);

    loadConfiguration();
    configDeej();
}



void layerCallback(cmd* cmdPtr)
{
    Command cmd(cmdPtr);
    Argument arg = cmd.getArgument(0);

    if(arg.isSet())
    {
        uint8_t forceLayerArgument = arg.getValue().toInt();

        if(-1 <= forceLayerArgument && forceLayerArgument <= 7)
        {
            Serial.printf("Forced layer to %i\r\n\r\n", forceLayerArgument);
            forceLayer = forceLayerArgument;    
        }
        else
        {
            Serial.printf("Invalid layer value.\r\nExpected values from -1 to 7.\r\n\r\n");
        }
    }
    else
    {
        Serial.printf("Current layer is: %i\r\n\r\n", forceLayer);
    }

}



void errorCallback(cmd_error* errorPtr) {
    CommandError e(errorPtr);

    Serial.printf("ERROR command: %s unknown.\r\n", e.toString().c_str());  //.c_str() from AI

    if (e.hasCommand()) {
        Serial.println("Did you mean? " + e.getCommand().toString());
    } else {
        //Serial.println(cli.toString());
        Serial.printf("Type \"help\" for more help\r\n\r\n");
    }
}



void infoCallback(cmd* cmdPtr)
{
    Command cmd(cmdPtr);

    cpuCallback(cmdPtr);
    versionCallback(cmdPtr);
    powerCallback(cmdPtr);
    macaddressCallback(cmdPtr);
    debugCallback(cmdPtr);
}



void escapeCallback(cmd* cmdPtr)
{
    Command cmd(cmdPtr);

    forceLayer = -1;
    Serial.printf("  Turned off forced layer.\r\n\r\n");
}



void cliSetup()
{
    cmdHelp = cli.addCommand("help,h", helpCallback);
    cmdInfo = cli.addCommand("info", infoCallback);
    cmdMacAddress = cli.addCommand("macaddress,getmacaddress", macaddressCallback);
    cmdVersion = cli.addCommand("version,ver", versionCallback);
    cmdPower = cli.addCommand("power", powerCallback);
    cmdCpu = cli.addCommand("cpu", cpuCallback);
    cmdFormat = cli.addCommand("format", formatCallback);
    cmdDeej = cli.addCommand("deej", deejCallback);
    cmdBattery = cli.addCommand("battery,batt", batteryCallback);
    cmdConfig = cli.addCommand("config,load", configCallback);
    cmdLayer = cli.addSingleArgumentCommand("l,L,layer", layerCallback);
    cmdEscape = cli.addCommand("escape",escapeCallback);
    cmdSetCpuFreq = cli.addSingleArgumentCommand("setcpufreq", setcpufreqCallback);
    cmdRestart = cli.addCommand("restart,reboot", restartCallback);
    cmdDebug = cli.addSingleArgumentCommand("debug", debugCallback);

    cli.setOnError(errorCallback);
}



#endif