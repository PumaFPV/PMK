#include <Arduino.h>

#include "config.h"

#define FIRMWARE_REV "MSC_ver"
#include "WiFi.h"
#include "esp_now.h"
//#include "USB.h"
//#include "USBHIDKeyboard.h"
//#include "FirmwareMSC.h"
#include "SPI.h"
#include "SdFat.h"
#include "Adafruit_SPIFlash.h"
#include "Adafruit_TinyUSB.h"

#include "variables.h"

#include "pmk.h"
#include "USBHandle.h"
#include "espNowHandle.h"
#include "fsHandle.h"
#include "MSCHandle.h"


//bool msc_ready_callback(void);
//void msc_flush_cb (void);
//int32_t msc_write_cba (uint32_t lba, uint8_t* buffer, uint32_t bufsize);
//int32_t msc_read_cba (uint32_t lba, void* buffer, uint32_t bufsize);
//void refreshMassStorage(void);
//void setupMassStorage(void);

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and 
// return number of copied bytes (must be multiple of block size) 
int32_t msc_read_cba (uint32_t lba, void* buffer, uint32_t bufsize)
{
  // Note: SPIFLash Bock API: readBlocks/writeBlocks/syncBlocks
  // already include 4K sector caching internally. We don't need to cache it, yahhhh!!
  return flash.readBlocks(lba, (uint8_t*) buffer, bufsize/512) ? bufsize : -1;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and 
// return number of written bytes (must be multiple of block size)
int32_t msc_write_cba (uint32_t lba, uint8_t* buffer, uint32_t bufsize)
{
#ifdef LED_BUILTIN
  digitalWrite(LED_BUILTIN, HIGH);
#endif

  // Note: SPIFLash Bock API: readBlocks/writeBlocks/syncBlocks
  // already include 4K sector caching internally. We don't need to cache it, yahhhh!!
  return flash.writeBlocks(lba, buffer, bufsize/512) ? bufsize : -1;
}

// Callback invoked when WRITE10 command is completed (status received and accepted by host).
// used to flush any pending cache.
void msc_flush_cba (void)
{
  // sync with flash
  flash.syncBlocks();

  // clear file system's cache to force refresh
  fatfs.cacheClear();

#ifdef LED_BUILTIN
  digitalWrite(LED_BUILTIN, LOW);
#endif
}

// Invoked when received Test Unit Ready command.
// return true allowing host to read/write this LUN e.g SD card inserted
bool msc_ready_callbacka(void)
{
  // if fs has changed, mark unit as not ready temporarily to force PC to flush cache
  bool ret = !fs_changed;
  fs_changed = false;
  return ret;
}

void refreshMassStorage(void)
{
  fs_changed = true;
}



void loopCount();

const char compileDate[] = __DATE__ " " __TIME__;

void setup()
{
  Serial.begin(115200);
  USB.onEvent(usbEventCallback);
  //MSC_Update.onEvent(usbEventCallback);
  //MSC_Update.begin();  
  USB.begin();

  while(!Serial){}
  pinMode(LED_DATA_PIN, OUTPUT);
  digitalWrite(LED_DATA_PIN, 1);
  delay(10);
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
  //====================LittleFS===============
  //===========================================
  /*
  if(!LittleFS.begin())
  {
    Serial.printf("An Error has occurred while mounting LittleFS\r\n");
    return;
  }
  else
  {
    Serial.printf("File system is ok\r\n");
  }*/
  flash.begin();

  // Set disk vendor id, product id and revision with string up to 8, 16, 4 characters respectively
  usb_msc.setID("PMK", "Configure Flash", "0.1");

  // Set callback
  usb_msc.setReadWriteCallback(msc_read_cba, msc_write_cba, msc_flush_cba);

  // Set disk size, block size should be 512 regardless of spi flash page size
  usb_msc.setCapacity(flash.size()/512, 512);

  // MSC is ready for read/write
  fs_changed = false;
  usb_msc.setReadyCallback(0, msc_ready_callbacka);

  usb_msc.begin();

  // Init file system on the flash
  fs_formatted = fatfs.begin(&flash);

  if ( !fs_formatted )
  {
    Serial.println("Failed to init files system, flash may not be formatted");
  }

  //===========================================
  //====================USB====================
  //===========================================
  Serial.print("Starting keyboard\r\n");
  Keyboard.begin();
  Serial.print("Starting mouse\r\n");
  Mouse.begin();
  Serial.print("Starting gamepad\r\n");
  Gamepad.begin();


  //====================================================
  //====================Wifi/ESP Now====================
  //====================================================
  Serial.printf("Starting WiFi\r\n");
  WiFi.mode(WIFI_STA);
  Serial.print("Dongle MAC address: " + WiFi.macAddress() + "\r\n");

  if (esp_now_init() != ESP_OK) 
  {
    Serial.printf("Error initializing ESP-NOW\r\n");
    return;
  }
  else
  {
    Serial.printf("Succesfully initiated ESP-NOW\r\n");
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
  
  File32 root = fatfs.open("/");
  for(uint8_t deviceNumber = 0; deviceNumber < numberOfDeviceToConfig; deviceNumber++)
  {
    Serial.printf("Configuring device number %d\r\n", deviceNumber);

    File32 directory = root.openNextFile();
    
    if(!directory.isDirectory())
    {
      Serial.printf("There is a file in root, please reupload file system");
    }

    File32 configRoot = fatfs.open(directory.name(), O_READ);

    //Now inside device config folder
    String deviceName = directory.name();
    Serial.printf("Device name to configure: %s \r\n", deviceName);
    uint8_t deviceNameLength = deviceName.length();

    File32 folder = directory.openNextFile();

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
  
  listDir(fatfs, "/", 3);

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