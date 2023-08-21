#include <Arduino.h>

#include "config.h"

#define LED 9

#include "WiFi.h"
#include "esp_now.h"
#include "USB.h"
#include "USBHIDKeyboard.h"
#include "LittleFS.h"

#include "variables.h"

#include "pmk.h"
#include "USBHandle.h"
#include "espNowHandle.h"
#include "fsHandle.h"
#include "MSCHandle.h"

void loopCount();

void setup()
{

  Serial.begin(115200);
  //while(!Serial)
  //{
  //}
  Serial.printf("Dongle is booting\r\n");

  //===========================================
  //====================LittleFS===============
  //===========================================
  if(!LittleFS.begin())
  {
    Serial.printf("An Error has occurred while mounting LittleFS\r\n");
    return;
  }
  else
  {
    Serial.printf("File system is ok\r\n");
  }

  //===========================================
  //====================USB====================
  //===========================================

  Keyboard.begin();

  //====================================================
  //====================Wifi/ESP Now====================
  //====================================================

  WiFi.mode(WIFI_STA);
  Serial.print("Dongle MAC address: " + WiFi.macAddress() + "\r\n");

  if (esp_now_init() != ESP_OK) 
  {
    Serial.printf("Error initializing ESP-NOW\r\n");
    return;
  }

  esp_now_register_send_cb(OnEspNowDataSent);

  esp_now_register_recv_cb(OnEspNowDataRecv);
  
  //Register peer
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  //===========================================
  //====================Config=================
  //===========================================

  uint8_t numberOfDeviceToConfig = getNumberOfDevices();

  Serial.printf("Number of devices to config: %d \r\n\r\n", numberOfDeviceToConfig);
  
  File root = LittleFS.open("/");
  for(uint8_t deviceNumber = 0; deviceNumber < numberOfDeviceToConfig; deviceNumber++)
  {
    Serial.printf("Configuring device number %d\r\n", deviceNumber);

    File directory = root.openNextFile();
    
    if(!directory.isDirectory())
    {
      Serial.printf("There is a file in root, please reupload file system");
    }

    File configRoot = LittleFS.open(directory.path());

    //Now inside device config folder
    String deviceName = directory.name();
    Serial.printf("Device name to configure: %s \r\n", deviceName);
    uint8_t deviceNameLength = deviceName.length();

    File folder = directory.openNextFile();

    while(folder)
    {
      String fileName = folder.name();

      Serial.printf("File name: %s\r\n", folder.name());
      String filePathString = "/" + deviceName + "/" + deviceName + ".json";
      const char* filePath = new char[filePathString.length() + 1];
      strcpy(const_cast<char*>(filePath), filePathString.c_str());

      if(fileName == String(deviceName + ".json"))
      {
        Serial.printf("Config name: %s \r\n", getAttribute(filePath, "deviceName"));
        Serial.printf("Device ID: %s \r\n", getAttribute(filePath, "deviceID"));

        addDeviceAddress(filePath); 
      }

      else if (fileName == String(deviceName + "-key.json"))
      {


      }

      else if (fileName == String(deviceName + "-led.json"))
      {


      }
      
      folder = directory.openNextFile();
    }
    
    Serial.printf("\n\r");

  }
  
  //listDir(LittleFS, "/", 3);

}



void loop()
{
  //Receive ESP-NOW packet

  //Figure out which command it is

  //If special key (shift / fn / other) than choose correct kb layout

  //if USB command  -> send

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

      for(uint8_t i = 0; i < 8; i++)
      {
        for(uint8_t j = 0; j < 2; j++)
        {
          if(keyboardPacket.key[i] == layerKeyID[j])
          {
            if(j == 0 && layerID > 0)
            {
              layerID--;
              delay(200);
            }
            else if (j == 1 && layerID < 7)
            {
              layerID++;
              delay(200);
            }
            //layerID = j;
            keyboardPacket.key[i] = 0xFF;
            i = 9;
            j = 2;
          }
        }
      }

      for(uint8_t i = 0; i < 8; i++)
      {
        if(keyboardPacket.key[i] != 255 && layerID != settingLayerID)
        {
          Keyboard.press(keyIDtoChar(keyboardPacket.key[i], layerID));
          //Serial.print("Pressing: 0x");
          //Serial.print(keyboardPacket.key[i]);
          //Serial.println(keyIDtoChar(keyboardPacket.key[i], layerID), HEX);
        }
        else if (keyboardPacket.key[i] != 0xFF && layerID == settingLayerID)
        {
          switch (keyboardPacket.key[i])
          {
          case 0x09:
            if(ledBrightness < 255)
            {
              ledBrightness++;
            }   
            break;

          case 0x11:
            if(ledBrightness > 0)
            {
              ledBrightness--;
            }
            break;
          }
        }        
      }

      uint8_t releaseKeys[8];
      memcpy(releaseKeys, previousKeyboardPacket.key, 8);

      for(uint8_t i = 0; i < 8; i++)
      {
        for(uint8_t j = 0; j < 8; j++)
        {
          if(previousKeyboardPacket.key[i] == keyboardPacket.key[j])
          {
            releaseKeys[i] = 255;
          }
        }
      }

      for(uint8_t i = 0; i < 8; i++)
      {
        if(releaseKeys[i] != 255)
        {
          Keyboard.release(keyIDtoChar(releaseKeys[i], layerID));
        }
      }

      memcpy(previousKeyboardPacket.key, keyboardPacket.key, 8);

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

void uartHandle()
{
  if(Serial.available())
  {
    String macAddressCommand = "macaddress";
    String command = Serial.readString();
    Serial.println(command);
    
    if(command == macAddressCommand)
    {
      Serial.print("Device MAC address is: ");
      Serial.println(WiFi.macAddress());
    }
    else
    {
      Serial.println("Invalid command");
    }
  }
}