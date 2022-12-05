#include <Arduino.h>
#include "WiFi.h"
#include "esp_now.h"

#ifdef ESP32C3
#include "FastLED.h"

#define NUM_LEDS 1
#define DATA_PIN 7

CRGB leds[NUM_LEDS];
#endif


#ifdef ESP32S2
#include "USB.h"
#include "FirmwareMSC.h"

FirmwareMSC MSC_Update;

USBCDC USBSerial;

#include "USBHandle.h"
#endif

#include "pmk.h"

#include "espNowHandle.h"

//84:F7:03:F0:EF:72
uint8_t dongleAddress[] = {0x84, 0xF7, 0x03, 0xF0, 0xEF, 0x72};

esp_now_peer_info_t peerInfo;

keyboardStruct keyboardPacket;

void setup() 
{
  #ifdef ESP32C3
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(10);
  #endif

  WiFi.mode(WIFI_STA);

  #ifdef ESP32S2
  USB.onEvent(usbEventCallback);
  MSC_Update.onEvent(usbEventCallback);
  MSC_Update.begin();
  USBSerial.onEvent(usbEventCallback);
  USBSerial.begin();

  USBSerial.println(WiFi.macAddress());
  #endif
  Serial.begin(115200);

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
  /*
  #ifdef ESP32C3
  leds[0] = CRGB::Red;
  FastLED.show();
  delay(500);
  // Now turn the LED off, then pause
  leds[0] = CRGB::Black;
  FastLED.show();
  delay(500);
  
  #endif
  */

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
    if(command.equalsIgnoreCase("setDongleMacAddress"))
    {
      Serial.println("Send MAC address");
      while(Serial.available() > 0)
      {
        Serial.readBytesUntil('\n', dongleAddress, 6);
        for(uint8_t i = 0; i < 7; ++i)
        {
          Serial.println(dongleAddress[i]);
        }
      }
    }
    else
    {
      Serial.println("Invalid command");
    }
  }


  /*
  Serial.println(WiFi.macAddress());

  //Read SPI from shift register
  uint32_t spiPacket = 0b00000000000000000000000000000000;
  spiPacket = spiPacket | ((!digitalRead(0)) << 5);
  Serial.println(spiPacket);

  //Process spiPacket
  uint8_t pos = 0;

  for(uint8_t i = 0; i < 32; i++)
  {
    bool isKeyPressed = spiPacket & (0b1 << i);
    
    if(isKeyPressed && pos < 8)
    {
      keyboardPacket.key[pos] = i;
      pos++;
    }
    if(pos == 8)
    {
      Serial.println("maximum 8 keys pressed");
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
  //USBSerial.println(getXtalFrequencyMhz());
  //USBSerial.println(getApbFrequency());
  //USBSerial.println(getCpuFrequencyMhz());

  while(Serial.available() > 0) 
  {
    // read incoming serial data:
    char inChar = Serial.read();
    // Type the next ASCII value from what you received:
    Serial.print(inChar + 1);
  }
  */
}