#include <Arduino.h>
#include <string>
#include <cstring>
#include <cstdint>

//TO DO delete this file
#include "config.h"

#define FIRMWARE_REV "dongle-dev"

//Include library
#include "WiFi.h"
#include "esp_now.h"
#include "SPI.h"
#include "SdFat.h"
#include "Adafruit_SPIFlash.h"
#include "Adafruit_TinyUSB.h"

//Include files
#include "variables.h"

#include "pmk.h"
#include "USBHandle.h"
#include "espNowHandle.h"
#include "fsHandle.h"
#include "MSCHandle.h"
#include "uartHandle.h"

// Default USB Settings



void loopCount(); //For function telemetry purpose / Homemade Ultra lite RTOS



//===========================================
//===========================================
//====================Setup==================
//===========================================
//===========================================
void setup()
{

  Serial.begin(115200);



  //===========================================
  //====================FatFS==================
  //===========================================
  //Initiate flash
  if(!flash.begin())
  {
    Serial.printf("An Error has occurred while mounting FatFS\r\n");
    return;
  }
  else
  {
    Serial.printf("File system is ok\r\n");
  }
  
  // Set disk vendor id, product id and revision with string up to 8, 16, 4 characters respectively
  usb_msc.setID("PMK", "Config Flash", "0.1");

  // Set read callback
  usb_msc.setReadWriteCallback(msc_read_cba, msc_write_cba, msc_flush_cba);

  // Set disk size, block size should be 512 regardless of spi flash page size
  usb_msc.setCapacity(flash.size()/512, 512);

  // MSC is ready for read/write
  usb_msc.setReadyCallback(0, msc_ready_callbacka);

  usb_msc.begin();

  // Init file system on the flash
  while(!fatfs.begin(&flash))
  {
    Serial.printf("Failed to init files system, flash may not be formatted\r\n");
    Serial.printf("Formatting the flash for FatFS use...\r\n");
    format_fat12();
    check_fat12();
    Serial.printf("Flash formatted successfully\r\n");
    ESP.restart();
  }

  pinMode(LED_DATA_PIN, OUTPUT);
  digitalWrite(LED_DATA_PIN, 1);

  Serial.printf("Dongle is booting\r\n");
  Serial.printf("Firmware rev: %s\r\n", FIRMWARE_REV);
  Serial.printf("Firmware was built the: %s at %s\r\n\r\n", __DATE__, __TIME__);

  Serial.printf("__          __  _                            _          _____  __  __ _  __ \r\n");
  Serial.printf("\\ \\        / / | |                          | |        |  __ \\|  \\/  | |/ / \r\n");
  Serial.printf(" \\ \\  /\\  / /__| | ___ ___  _ __ ___   ___  | |_ ___   | |__) | \\  / | ' /  \r\n");
  Serial.printf("  \\ \\/  \\/ / _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\ | __/ _ \\  |  ___/| |\\/| |  <   \r\n");
  Serial.printf("   \\  /\\  /  __/ | (_| (_) | | | | | |  __/ | || (_) | | |    | |  | | . \\  \r\n");
  Serial.printf("    \\/  \\/ \\___|_|\\___\\___/|_| |_| |_|\\___| \\__ \\___/  |_|    |_|  |_|_|\\_\\ \r\n\r\n\r\n");



  //===========================================
  //====================USB====================
  //===========================================
  // Set up HID
  //usb_hid.setBootProtocol(HID_ITF_PROTOCOL_KEYBOARD);
  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.setStringDescriptor("PMK");

  // Set up output report (on control endpoint) for Capslock indicator
  usb_hid.setReportCallback(NULL, hid_report_callback);
  
  usb_hid.begin();

  //Serial.printf("Waiting for USB HID to be mounted...\r\n");
  while( !TinyUSBDevice.mounted() ) delay(1);



  //====================================================
  //====================Wifi/ESP Now====================
  //====================================================
  Serial.printf("Starting WiFi\r\n");
  WiFi.mode(WIFI_STA);
  WiFi.setTxPower(WIFI_POWER_11dBm);
  Serial.printf("New WiFi power: ");
  Serial.println(WiFi.getTxPower());
  Serial.print("Dongle MAC address: " + WiFi.macAddress() + "\r\n");

  if(esp_now_init() != ESP_OK) 
  {
    Serial.printf("Error initializing ESP-NOW\r\n");
    return;
  }
  else
  {
    Serial.printf("Succesfully initiated ESP-NOW\r\n\r\n");
  }

  esp_now_register_send_cb(OnEspNowDataSent);

  //Register peer
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;



  //===========================================
  //====================Config=================
  //===========================================
  uint8_t numberOfDeviceToConfig = getNumberOfDevices();
  Serial.printf("Number of devices to config: %d \r\n\r\n", numberOfDeviceToConfig);

  File32 root = fatfs.open("/");

  for(uint8_t deviceNumber = 0; deviceNumber <= numberOfDeviceToConfig; deviceNumber++)
  {

    Serial.printf("Configuring device number %d\r\n", deviceNumber);

    File32 deviceDirectory = root.openNextFile();

    if(!deviceDirectory.isDirectory())
    {
      Serial.printf("There is a file in root, please reupload file system\r\n");
    }

    char deviceName[32];
    deviceDirectory.getName(deviceName, sizeof(deviceName));
    bool isConfigFolder = strcmp(deviceName, "System Volume Information");  //Will output something else than 0 if not sys volume info

    if(isConfigFolder) //Current folder is a device config folder
    {
      Serial.printf("Device name to configure: %s \r\n", deviceName);

      File32 configFolder = deviceDirectory.openNextFile();

      //Now inside device config folder
      while(configFolder)
      {
        //Serial.printf("The folder is dir: %i\r\n", configFolderisDir());
        char name[32];
        configFolder.getName(name, sizeof(name));
        Serial.printf("File name: %s\r\n", name);

        char filePath[64] = "\0";
        strcat(filePath, "/");  // TODO make this clean one day
        strcat(filePath, deviceName);
        strcat(filePath, "/");
        strcat(filePath, deviceName);
        strcat(filePath, ".json");

        if(!configFolder.isDirectory())
        {
          //We are at the config json file that contains MAC address, etc...
          Serial.printf("File path: %s \r\n", filePath);
          Serial.printf("Device ID: %s \r\n", getAttribute(filePath, "deviceID"));  //Used to check if we can properly read attributes
          Serial.printf("Config name: %s \r\n", getAttribute(filePath, "deviceName"));
          addDeviceAddress(filePath); //Add the device address to the array of ESP-NOW devices
        }
        else
        {
          //We have a directory, most likely keyboard or led
          if(strcmp(name, "keyboard") == 0)
          {
            //Serial.printf("We have a keyboard config folder\r\n");
            for(uint8_t currentLayer = 0; currentLayer < MAX_NUMBER_OF_LAYERS; currentLayer++)
            {
              char kbLayerFile[64] ="\0";
              char currentLayerChar[1];
              sprintf(currentLayerChar, "%hhu", currentLayer);  // TODO make this clean one day
              strcat(kbLayerFile, "/");
              strcat(kbLayerFile, deviceName);
              strcat(kbLayerFile, "/keyboard/kb-l");
              strcat(kbLayerFile, currentLayerChar);
              strcat(kbLayerFile, ".json");

              File32 layerFolder;
              if(layerFolder.open(kbLayerFile))
              {
                Serial.printf("Successfully opened %s\r\n", kbLayerFile);
                uint8_t deviceID = static_cast<uint8_t>(std::atoi(getAttribute(filePath, "deviceID"))); // TODO make this clean one day
                loadKeyConfig(kbLayerFile, deviceID, currentLayer);
              }
              else
              {
                //Serial.printf("Couldnt open %s\r\n", kbLayerFile);
              }
            }
          }
          if(strcmp(name, "led") == 0)
          {
            //Serial.printf("We have a led config folder\r\n");
            for(uint8_t currentLayer = 0; currentLayer < MAX_NUMBER_OF_LAYERS; currentLayer++)
            {
              char ledLayerFile[64] ="\0";
              char currentLayerChar[1];
              sprintf(currentLayerChar, "%hhu", currentLayer);
              strcat(ledLayerFile, "/");
              strcat(ledLayerFile, deviceName);
              strcat(ledLayerFile, "/led/led-l");
              strcat(ledLayerFile, currentLayerChar);
              strcat(ledLayerFile, ".json");

              File32 layerFolder;
              if(layerFolder.open(ledLayerFile))
              {
                Serial.printf("Successfully opened %s\r\n", ledLayerFile);
                uint8_t deviceID = static_cast<uint8_t>(std::atoi(getAttribute(filePath, "deviceID")));
                loadLedConfig(ledLayerFile, deviceID, currentLayer);
              }
              else
              {
                //Serial.printf("Couldnt open %s\r\n", ledLayerFile);
              }
            }
          }
          if(strcmp(name, "mouse") == 0)
          {
            Serial.printf("We have a mouse config folder\r\n");
            for(uint8_t currentLayer = 0; currentLayer < MAX_NUMBER_OF_LAYERS; currentLayer++)
            {
              char mouseLayerFile[64] ="\0";
              char currentLayerChar[1];
              sprintf(currentLayerChar, "%hhu", currentLayer);
              strcat(mouseLayerFile, "/");
              strcat(mouseLayerFile, deviceName);
              strcat(mouseLayerFile, "/mouse/m-l");
              strcat(mouseLayerFile, currentLayerChar);
              strcat(mouseLayerFile, ".json");

              File32 layerFolder;
              if(layerFolder.open(mouseLayerFile))
              {
                Serial.printf("Successfully opened %s\r\n", mouseLayerFile);
                uint8_t deviceID = static_cast<uint8_t>(std::atoi(getAttribute(filePath, "deviceID")));
                loadMouseConfig(mouseLayerFile, deviceID, currentLayer);
              }
              else
              {
                //Serial.printf("Couldnt open %s\r\n", mouseLayerFile);
              }
            }
          }
        }

        configFolder = deviceDirectory.openNextFile();
      }
      
      Serial.printf("\n\r");
    }
    else
    {
      Serial.printf("This is system volume information configFolder.. Skipping config\r\n\r\n");
    }

  }


  configDeej();

  setupPMK();

  esp_now_register_recv_cb(OnEspNowDataRecv);

}



void loop()
{

  //------------------------------------------------------ledTask
  if(micros() - ledTask.beginTime >= ledTask.interval)
  {
    ledTask.beginTime = micros();
    ledTask.inBetweenTime = ledTask.beginTime - ledTask.endTime;

    //**functions



    ledTask.endTime = micros();
    ledTask.counter++;
    ledTask.duration = ledTask.endTime - ledTask.beginTime;
  }

  //------------------------------------------------------espnowTask
  if(micros() - espnowTask.beginTime >= espnowTask.interval)
  {
    espnowTask.beginTime = micros();
    espnowTask.inBetweenTime = espnowTask.beginTime - espnowTask.endTime;

    //**functions

    espnowTask.endTime = micros();
    espnowTask.counter++;
    espnowTask.duration = espnowTask.endTime - espnowTask.beginTime;

  }

  //------------------------------------------------------keyboardTask
  if(micros() - keyboardTask.beginTime >= keyboardTask.interval)
  {
    keyboardTask.beginTime = micros();
    keyboardTask.inBetweenTime = keyboardTask.beginTime - keyboardTask.endTime;



    //Keyboard functions end

    keyboardTask.endTime = micros();
    keyboardTask.counter++;
    keyboardTask.duration = keyboardTask.endTime - keyboardTask.beginTime;

  }

  //------------------------------------------------------uartTask
  if(micros() - uartTask.beginTime >= uartTask.interval)
  {
    uartTask.beginTime = micros();
    uartTask.inBetweenTime = uartTask.beginTime - uartTask.endTime;

    //**functions
    handleUart();


    uartTask.endTime = micros();
    uartTask.counter++;
    uartTask.duration = uartTask.endTime - uartTask.beginTime;

  }

  //------------------------------------------------------fsTask
  if(micros() - fsTask.beginTime >= fsTask.interval)
  {
    fsTask.beginTime = micros();
    fsTask.inBetweenTime = fsTask.beginTime - fsTask.endTime;

    //**functions
    
    //listDir(LittleFS, "/", 1);

    //readConfig();

    fsTask.endTime = micros();
    fsTask.counter++;
    fsTask.duration = fsTask.endTime - fsTask.beginTime;
  }

  //------------------------------------------------------telemetryTask
  if(micros() - telemetryTask.beginTime >= telemetryTask.interval)
  {
    telemetryTask.beginTime = micros();
    telemetryTask.inBetweenTime = telemetryTask.beginTime - telemetryTask.endTime;

    //**functions
    //Serial.printf("Proc temp: %.1f °C\r\n", temperatureRead());

    telemetryTask.endTime = micros();
    telemetryTask.counter++;
    telemetryTask.duration = telemetryTask.endTime - telemetryTask.beginTime;
  }
}



void loopCount()
{
  //Task frequency counter
  if(ledTask.counter == 0)
  {
    ledTask.startCounterTime = micros();
  }
  if(micros() - ledTask.startCounterTime > 1000000)
  {
    ledTask.frequency = ledTask.counter;
    //Serial.println(ledTask.counter);
    ledTask.counter = 0;
  }

  //Task frequency counter
  if(espnowTask.counter == 0)
  {
    espnowTask.startCounterTime = micros();
  }
  if(micros() - espnowTask.startCounterTime > 1000000)
  {
    espnowTask.frequency = espnowTask.counter;
    //Serial.println(espnowTask.counter);
    espnowTask.counter = 0;
  }

  //Task frequency counter
  if(keyboardTask.counter == 0)
  {
    keyboardTask.startCounterTime = micros();
  }
  if(micros() - keyboardTask.startCounterTime > 1000000)
  {
    keyboardTask.frequency = keyboardTask.counter;
    //Serial.println(keyboardTask.counter);
    keyboardTask.counter = 0;
  }

  //Task frequency counter
  if(uartTask.counter == 0)
  {
    uartTask.startCounterTime = micros();
  }
  if(micros() - uartTask.startCounterTime > 1000000)
  {
    uartTask.frequency = uartTask.counter;
    //Serial.println(uartTask.counter);
    uartTask.counter = 0;
  }

  //Task frequency counter
  if(fsTask.counter == 0)
  {
    fsTask.startCounterTime = micros();
  }
  if(micros() - fsTask.startCounterTime > 1000000)
  {
    fsTask.frequency = fsTask.counter;
    //Serial.println(fsTask.counter);
    fsTask.counter = 0;
  }

  //Task frequency counter
  if(telemetryTask.counter == 0)
  {
    telemetryTask.startCounterTime = micros();
  }
  if(micros() - telemetryTask.startCounterTime > 1000000)
  {
    telemetryTask.frequency = telemetryTask.counter;
    //Serial.println(fsTask.counter);
    telemetryTask.counter = 0;
  }
}