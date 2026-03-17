#ifndef COMMANDHANDLE_H
#define COMMANDHANDLE_H

#include "SimpleCLI.h"

#include "Arduino.h"
#include "WiFi.h"
#include "i2cHandle.h"
#include "rtc.h"

static bool debug[8] = {0,0,0,0,0,0,0,0};

#include "espnowHandle.h"


SimpleCLI cli;

Command cmdHelp;
Command cmdInfo;
Command cmdMacAddress;
Command cmdSetDongleMacaddress;
Command cmdGetDongleMacaddress;
Command cmdSetDeviceId;
Command cmdGetDeviceId;
Command cmdVersion;
Command cmdPower;
Command cmdCpu;
Command cmdSetCpuFreq;
Command cmdRestart;
Command cmdScan;
Command cmdLedSleep;
Command cmdDebug;
Command cmdKnob;



void helpCallback(cmd* cmdPtr)
{
    Command cmd(cmdPtr);

    Serial.printf("List of available commands:\r\n");
    Serial.printf("   h / help : returns available commands\r\n");
    Serial.printf("   info : display all info\r\n");
    Serial.printf("   macaddress / getmacaddress : returns device MAC address\r\n");
    Serial.printf("   setdonglemacaddress : update dongle MAC address, be sure to send values in HEX mode\r\n");
    Serial.printf("   getdonglemacaddress : returns the current dongle MAC address\r\n");
    Serial.printf("   setdeviceid : update deviceID to new value\r\n");
    Serial.printf("   getdeviceid : returns currently set deviceID");
    Serial.printf("   ver / version : returns build date and version\r\n");
    Serial.printf("   power : returns RF Tx power\r\n");
    Serial.printf("   cpu : returns CPU info\r\n");
    Serial.printf("   setcpufreq : set new CPU frequency in MHz\r\n");
    Serial.printf("   restart : restart the dongle\r\n");
    Serial.printf("   scan : Scan for side module presence\r\n");
    Serial.printf("   ledsleep : Set LED sleep time in minutes\r\n");
    Serial.printf("   knob / midi / deej: arg1: knob number (0-7), arg2: volume value (0-255) \r\n");
    Serial.printf("   debug 1 / 2... : Toggle debug output. Different kind of debug available\r\n\r\n");
}



void macaddressCallback(cmd* cmdPtr)
{
    Command cmd(cmdPtr);

    Serial.print("Device MAC address is: ");
    Serial.println(WiFi.macAddress());
    Serial.printf("\r\n");
}



uint8_t convertTwoChar2Byte(char first, char second)
{
    uint8_t output = 0;
    uint8_t character[2] = {first, second};
    
    for(uint8_t i = 0; i < 2; i++)
    {
        switch (character[i])
        {
            case 49:
                character[i] = 0x1;
                break;
            case 50:
                character[i] = 0x2;
                break;
            case 51:
                character[i] = 0x3;
                break;
            case 52:
                character[i] = 0x4;
                break;
            case 53:
                character[i] = 0x5;
                break;
            case 54:
                character[i] = 0x6;
                break;
            case 55:
                character[i] = 0x7;
                break;
            case 56:
                character[i] = 0x8;
                break;
            case 57:
                character[i] = 0x9;
                break;
            case 97:
            case 65:
                // A
                character[i] = 0xA;
                break;
            case 98:
            case 66:
                // B
                character[i] = 0xB;
                break;
            case 99:
            case 67:
                // C
                character[i] = 0xC;
                break;
            case 100:
            case 68:
                // D
                character[i] = 0xD;
                break;
            case 101:
            case 69:
                // E
                character[i] = 0xE;
                break;
            case 102:
            case 70:
                // F
                character[i] = 0xF;
                break;
            default:
                Serial.printf("Out of hex\r\n");
                return 0x00;
        }
        
    }
    output = (character[0] << 4) | character[1];

    return output;
}



uint8_t convertString2Hex(String string)   //Converts 2 char to hex value
{
    uint8_t hex = 0;

    if(string.length() == 2)
    {
        char first = string.charAt(0);
        char second = string.charAt(1);
        hex = convertTwoChar2Byte(first, second);
    }
    else
    {
        Serial.printf("Input string of wrong size. Expected 2 char, got %i.\r\n", string.length());
    }
    return hex;
}



void setdonglemacaddressCallback(cmd* cmdPtr)
{
    Command cmd(cmdPtr);

    uint8_t argNum = cmd.countArgs();
    if(argNum == 6)
    {
        for(uint8_t i = 0; i < 6; i++)
        {
            Argument arg = cmd.getArg(i);
            dongleAddress[i] = convertString2Hex(arg.getValue());
        }
    }
    else
    {
        Serial.printf("Invalid MAC address size. Entered MAC address: \r\n");
    }
    
    for(uint8_t i = 0; i < 6; i++)
    {
        EEPROM.write(DONGLE_MACADDRESS_ADDRESS + i, dongleAddress[i]);
        EEPROM.commit();
    }

    Serial.printf("Recevied dongle MAC address: ");
    for(uint8_t i = 0; i < 6; i++)
    {
        Serial.print(dongleAddress[i], HEX);
        Serial.printf(" ");
    }
    Serial.printf("\r\n\r\n");
    
    registerDongle();
}



void getdonglemacaddressCallback(cmd* cmdPtr)
{
    Command cmd(cmdPtr);

    Serial.printf("Current dongle MAC address is set to: ");
    for(uint8_t i = 0; i < MAC_ADDRESS_SIZE; i++)
    {
        Serial.print(dongleAddress[i], HEX);
        Serial.printf(" ");
    }
    Serial.printf("\r\n\r\n");
}



void setdeviceidCallback(cmd* cmdPtr)
{
    Command cmd(cmdPtr);
    Argument arg = cmd.getArgument(0);

    if(arg.isSet())
    {
        deviceID = arg.getValue().toInt();
        Serial.printf("Setting deviceID to: %u\r\n\r\n", deviceID);
        EEPROM.write(DEVICEID_ADDRESS, deviceID);
        EEPROM.commit();
        keyboardPacket.deviceID = deviceID;
        mousePacket.deviceID = deviceID;
    }
    else
    {
        Serial.printf("Current device ID is: %u\r\n", deviceID);
    }
}



void getdeviceidCallback(cmd* cmdPtr)
{
    Command cmd(cmdPtr);
    
    Serial.printf("Device ID is set to: %u\r\n\r\n", deviceID);
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



void cpuCallback(cmd* cmdPtr)
{
    Command cmd(cmdPtr);

    Serial.printf("Chip model: %s, chip revision: %u\r\n", ESP.getChipModel(), ESP.getChipRevision());
    Serial.printf("Proc temp: %.1f °C\r\n", temperatureRead());
    Serial.printf("CPU freq: %u MHz\r\n", getCpuFrequencyMhz());
    Serial.printf("XTal freq: %u MHz\r\n", getXtalFrequencyMhz());
    Serial.printf("APB freq: %u Hz\r\n", getApbFrequency());
    Serial.printf("Flash size: %u\r\n", ESP.getFlashChipSize());
    Serial.printf("Sketch size: %u\r\n\r\n", ESP.getSketchSize());
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
                Serial.printf("Can be: 240, 160, 80, 40, 20, 10\r\n");
                return;
        }

        setCpuFrequencyMhz(cpuFreq);
        Serial.printf("New CPU frequency set to: %u MHZ\r\n\r\n", getCpuFrequencyMhz());
    }
    else
    {
        Serial.printf("Current CPU frequency: %u MHz\r\n\r\n", getCpuFrequencyMhz());
    }
    
}



void restartCallback(cmd* cmdPtr)
{
    Command cmd(cmdPtr);

    Serial.printf("Restarting...\r\n\r\n");
    ESP.restart();
}



void scanCallback(cmd* cmdPtr)
{
    Command cmd(cmdPtr);

    checkSideModule();
}



void ledsleepCallback(cmd* cmdPtr)
{
    Command cmd(cmdPtr);
    Argument arg = cmd.getArgument(0);
    
    if(arg.isSet())
    {
        ledSleepDelay = arg.getValue().toInt();

        if(0 <= ledSleepDelay && ledSleepDelay <= 255)
        {
            Serial.printf("Setting LED sleep duration to %u minutes\r\n\r\n", ledSleepDelay);

            EEPROM.write(LED_SLEEP_DELAY_ADDRESS, ledSleepDelay);
            EEPROM.commit();
        }
        else
        {
            Serial.printf("Invalid value entered: %i\r\n\r\n");
        }
    }
    else
    {
        Serial.printf("Current LED sleep duration is: %u minutes.\r\n", ledSleepDelay);
        Serial.printf("Use argument after command to set LED sleep duration in minutes (0 is disabled, max 255 minutes):\r\n\r\n");
    }
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



void knobCallback(cmd* cmdPtr)
{
    Command cmd(cmdPtr);
    Argument knobArgument = cmd.getArgument(0);
    Argument volumeArgument = cmd.getArgument(1);
    
    uint8_t knobValue = knobArgument.getValue().toInt();
    uint8_t volumeValue = volumeArgument.getValue().toInt();

    if(0 <= knobValue && knobValue < 8)
    {
        if (0 <= volumeValue && volumeValue < 256)
        {
            Serial.printf("Setting knob %i to %i\r\n", knobValue, volumeValue);
            knobPacket.knob[knobValue] = volumeValue;
        }
        else
        {
            Serial.printf("Volume value outside of expected range (0-127)\r\n");
        }
    }
    else
    {
        Serial.printf("Knob value outside of expected range (0-7)\r\n");
    }
}

void cliSetup()
{
    cmdHelp = cli.addCommand("help,h", helpCallback);
    cmdInfo = cli.addCommand("info", infoCallback);
    cmdMacAddress = cli.addCommand("macaddress,getmacaddress", macaddressCallback);
    cmdSetDongleMacaddress = cli.addBoundlessCommand("setdonglemacaddress,donglemacaddress,sdma", setdonglemacaddressCallback);
    cmdGetDongleMacaddress = cli.addCommand("getdonglemacaddress", getdonglemacaddressCallback);
    cmdSetDeviceId = cli.addSingleArgumentCommand("setdeviceid,deviceid", setdeviceidCallback);
    cmdGetDeviceId = cli.addCommand("getdeviceid", getdeviceidCallback);
    cmdVersion = cli.addCommand("version,ver", versionCallback);
    cmdPower = cli.addCommand("power", powerCallback);
    cmdCpu = cli.addCommand("cpu", cpuCallback);
    cmdSetCpuFreq = cli.addSingleArgumentCommand("setcpufreq", setcpufreqCallback);
    cmdRestart = cli.addCommand("restart", restartCallback);
    cmdScan = cli.addCommand("scan", scanCallback);
    cmdLedSleep = cli.addSingleArgumentCommand("ledsleep", ledsleepCallback);
    cmdDebug = cli.addSingleArgumentCommand("debug", debugCallback);
    cmdKnob = cli.addBoundlessCommand("deej,knob,midi", knobCallback);

    cli.setOnError(errorCallback);

}



#endif