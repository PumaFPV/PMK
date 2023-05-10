#include <Arduino.h>

#include "SPI.h"
#include "USB.h"
#include "FirmwareMSC.h"
#include "WiFi.h"
#include "esp_now.h"

#include "FastLED.h"

#define SR_SPI_BUS 1 //Which SPI bus to use for this SPI object
#define SR_MISO 3
#define SR_CLK 7
#define SR_CE 9
#define SR_PL 5

static const int srSpiClk = 100000; // 100kHz

SPIClass * srSpi = NULL;
SPISettings settingsA(srSpiClk, MSBFIRST, SPI_MODE0);

#define NUM_LEDS 1
#define LED_DATA_PIN 4

//CRGB leds[NUM_LEDS];
CRGBArray<NUM_LEDS> leds;

//FirmwareMSC MSC_Update;

//USBCDC Serial;

#include "USBHandle.h"

#include "pmk.h"

#include "espNowHandle.h"

//84:F7:03:F0:EF:72
uint8_t dongleAddress[] = {0x58, 0xCF, 0x79, 0xA3, 0x98, 0xC2};

esp_now_peer_info_t peerInfo;


telemetryStruct telemetryPacket;
keyboardStruct keyboardPacket;

void setup() 
{
  Serial.begin(115200);

  //Initialize SPI for SR
  srSpi = new SPIClass(SR_SPI_BUS);
  
  srSpi->begin(SR_CLK, SR_MISO, -1, SR_CE);
  pinMode(srSpi->pinSS(), OUTPUT);


  FastLED.addLeds<WS2812B, LED_DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(255);
  leds[0] = CRGB::White;
  FastLED.show();

  //USB.onEvent(usbEventCallback);
  //MSC_Update.onEvent(usbEventCallback);
  //MSC_Update.begin();
  //Serial.onEvent(usbEventCallback);

  WiFi.mode(WIFI_STA);
  Serial.println(WiFi.macAddress());

  if(esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, dongleAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);

  //pinMode(0, INPUT_PULLUP);
  //pinMode(15, OUTPUT);
}



void loop() 
{
  //pride();
  //FastLED.show(); 
  //Serial.println(WiFi.macAddress());
  Serial.print("loop");
  for(uint8_t brightness = 0; brightness < 254; brightness++)
  {
    FastLED.setBrightness(brightness);
    FastLED.show();
    delay(2);

  }
  for(uint8_t brightness = 255; brightness > 0; brightness--)
  {
    FastLED.setBrightness(brightness);
    FastLED.show();
    delay(2);
  }

  //Read SPI from shift register
  uint32_t spiPacket[5] = {0x00, 0x00, 0x00, 0x00, 0x00};

  srSpi->beginTransaction(settingsA);

  for(uint8_t packet = 0; packet < 5; packet++)
  {
    spiPacket[packet] = srSpi->transfer(0);
    Serial.println(spiPacket[packet]);
  }
  srSpi->endTransaction();
/*
  //Read pressed keyshello worldhello world
  uint8_t numberOfPressedKeys = 0;

  for(uint8_t packet = 0; packet < 4; packet++)
  {
    for(uint8_t bit = 0; bit < 7; bit++)
    {
      bool isKeyPressed = spiPacket[packet] & (0b1 << bit);

      if(isKeyPressed == 1 && numberOfPressedKeys < 8)
      {
        keyboardPacket.key[numberOfPressedKeys] = (packet * 8) + bit;
        numberOfPressedKeys++;
      }
      if(numberOfPressedKeys == 8)
      {
        telemetryPacket.error = tooManyKeysPressed;
      }
    }
  }


  //Send all pressed keys to packet
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(dongleAddress, (uint8_t *) &keyboardPacket, sizeof(keyboardPacket));
   
  if (result == ESP_OK) 
  {
    Serial.println("Sent with success");
  }
  else 
  {
    Serial.println("Error sending the data");
  }
  //send packet

  //Deal with LED

  //Deal with other stuff
  //Serial.println(getXtalFrequencyMhz());
  //Serial.println(getApbFrequency());
  //Serial.println(getCpuFrequencyMhz());

  while(Serial.available() > 0) 
  {
    // read incoming serial data:
    char inChar = Serial.read();
    // Type the next ASCII value from what you received:
    Serial.print(inChar + 1);
  }
  */
}


void readUARTCommand()
{
  if(Serial.available() > 0)
  {
    String command = Serial.readStringUntil('\n');

    if(command.equalsIgnoreCase("macaddress"))
    {
      Serial.print("Device MAC address is: ");
      Serial.println(WiFi.macAddress());
    }
    else
    if(command.equalsIgnoreCase("id"))
    {
      uint32_t chipId = 0;
      for(int i=0; i<17; i=i+8) 
      { chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;}
      Serial.printf("ESP32 model = %s Rev %d\n", ESP.getChipModel(), ESP.getChipRevision());
	    Serial.printf("Core number %d \n", ESP.getChipCores());
      Serial.print("Chip ID: "); Serial.println(chipId);
    }
    else
    if(command.indexOf("setDongleMacAddress") >= 0)
    {
      //Expected command: setDongleMacAddress=84:F7:03:F0:EF:72
      uint8_t macAddressStart = command.indexOf("=");
      Serial.println(macAddressStart); //should return 19
      uint8_t columnStart = command.indexOf(":");
      Serial.println(columnStart); //should return 22
      
      for(uint8_t i = 0; i < 5; ++i)
      {
        String macSubAddress = command.substring(macAddressStart + i, columnStart + i);
        dongleAddress[i] = macSubAddress.toInt();
      }

    }
    else
    {
      Serial.println("Invalid command");
    }
  }
}