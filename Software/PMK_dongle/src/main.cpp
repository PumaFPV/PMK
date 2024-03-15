#include <Arduino.h>

//TO DO delete this file
#include "config.h"

#define FIRMWARE_REV "dev"

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



void loopCount(); //For function telemetry purpose / Homemade Ultra lite RTOS

const char compileDate[] = __DATE__ " " __TIME__; //For Serial info



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
  fs_changed = false;
  usb_msc.setReadyCallback(0, msc_ready_callbacka);

  usb_msc.begin();

  // Init file system on the flash
  fs_formatted = fatfs.begin(&flash);

  if ( !fs_formatted )
  {
    Serial.printf("Failed to init files system, flash may not be formatted\r\n");
  }  

  //===========================================
  //====================INIT===================
  //===========================================  
  //while(!Serial){}  //Optional debug help

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
  //====================USB====================
  //===========================================
  //===========================================
  //====================HID====================
  //===========================================
  // Set up HID
  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.setStringDescriptor("TinyUSB HID Composite");

  usb_hid.begin();

  // Set up output report (on control endpoint) for Capslock indicator
  Serial.printf("Waiting for USB HID to be mounted...\r\n");
  while( !TinyUSBDevice.mounted() ) delay(1);


  Serial.print("Starting keyboard\r\n");
  //Keyboard.begin();
  Serial.print("Starting mouse\r\n");
  //Mouse.begin();
  Serial.print("Starting gamepad\r\n");
  //Gamepad.begin();


  //====================================================
  //====================Wifi/ESP Now====================
  //====================================================
  //while(!Serial){}  //Optional debug helpw
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
    Serial.printf("Succesfully initiated ESP-NOW\r\n\r\n");
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
  //root.ls(&Serial, 0, 0);

  for(uint8_t deviceNumber = 0; deviceNumber <= numberOfDeviceToConfig; deviceNumber++)
  {

    Serial.printf("Configuring device number %d\r\n", deviceNumber);

    File32 directory = root.openNextFile();
    
    if(!directory.isDirectory())
    {
      Serial.printf("There is a file in root, please reupload file system\r\n");
    }

    char deviceName[32];
    directory.getName(deviceName, sizeof(deviceName));

    if(strcmp(deviceName, "System Volume Information"))
    {
      Serial.printf("Device name to configure: %s \r\n", deviceName);
            
      //File32 configRoot = fatfs.open(directory.name(), O_READ);

      File32 folder = directory.openNextFile();
      folder.ls(&Serial, 0, 1);


      //Now inside device config folder
      while(folder)
      {
        //Serial.printf("The folder is dir: %i\r\n", folder.isDir());
        char name[32];
        folder.getName(name, sizeof(name));
        Serial.printf("File name: %s\r\n", name);
        //Serial.printf("Current position: %i\r\n", folder.curPosition());  //Seems useless for now...
        //String deviceNameString = convertToString(deviceName, sizeof(deviceName) / sizeof(char));
        //String filePathString = "/" + deviceNameString + "/" + deviceName + ".json";
        char filePath[64] = "\0";
        strcat(filePath, "/");
        strcat(filePath, deviceName);
        strcat(filePath, "/");
        strcat(filePath, deviceName);
        strcat(filePath, ".json");
        //const char* filePath = new char[filePathString.length() + 1];
        //strcpy(const_cast<char*>(filePath), filePathString.c_str());
        if(!folder.isDirectory())
        {
          //We are at the config json file that contains MAC address, etc...
          Serial.printf("File path: %s \r\n", filePath);
          Serial.printf("Device ID: %s \r\n", getAttribute(filePath, "deviceID"));
          Serial.printf("Config name: %s \r\n", getAttribute(filePath, "deviceName"));
          addDeviceAddress(filePath);
        }
        else
        {

          Serial.printf("File is not file, it is directory\r\n\r\n");
        }

        if(name == deviceName)
        {
          Serial.printf("Config name: %s \r\n", getAttribute(filePath, "deviceName"));
          Serial.printf("Device ID: %s \r\n", getAttribute(filePath, "deviceID"));

          addDeviceAddress(filePath); 
        }

        else if (name == deviceName)
        {


        }

        else if (name == deviceName)
        {


        }
        
        folder = directory.openNextFile();
      }
      
      Serial.printf("\n\r");
    }
    else
    {
      Serial.printf("This is system volume information folder... Skipping config\r\n\r\n");
    }

  }
  
  //listDir(fatfs, "/", 3);
}


void loop()
{
  //Serial.printf("loop\r\n");

  //if (usb_hid.ready())
  //{
  //  // Random touch
  //  Serial.println("Random touch");
  //  gp.x       = random(-127, 128);
  //  gp.y       = random(-127, 128);
  //  gp.z       = random(-127, 128);
  //  gp.rz      = random(-127, 128);
  //  gp.rx      = random(-127, 128);
  //  gp.ry      = random(-127, 128);
  //  gp.hat     = random(0,      9);
  //  gp.buttons = random(0, 0xffff);
  //  usb_hid.sendReport(4, &gp, sizeof(gp));
  //}
  

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