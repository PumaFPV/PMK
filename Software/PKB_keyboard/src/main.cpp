#include <Arduino.h>

#include "config.h"

#include "SPI.h"
#include "USB.h"
#include "USBHIDKeyboard.h"
#include "WiFi.h"
#include "esp_now.h"
#include "FastLED.h"
#include "pmk.h"

#include "variables.h"

#include "USBHandle.h"
#include "espNowHandle.h"
#include "ledHandle.h"

void loopCount();


void setup() 
{
  //-----Serial
  Serial.begin(115200);


  //-----USB
  Keyboard.begin();


  //-----Shift register
  pinMode(SR_PL, OUTPUT);
  pinMode(SR_CE, OUTPUT);


  //Initialize SPI for SR
  srSpi = new SPIClass(SR_SPI_BUS);
  srSpi->begin(SR_CLK, SR_MISO, -1, SR_CE);
  pinMode(srSpi->pinSS(), OUTPUT);


  //-----Leds
  FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(250);

  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;


  //-----ESP NOW
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

  //-----PMK

}




void loop() 
{
  //Serial.println(WiFi.macAddress());
  //Serial.println("loop");
  loopCount();

  //------------------------------------------------------ledTask
  if(micros() - ledTask.beginTime >= ledTask.interval)
  {
    ledTask.beginTime = micros();
    ledTask.inBetweenTime = ledTask.beginTime - ledTask.endTime;

      Serial.println(WiFi.macAddress());

      setLedColorProfile(layerID);

      /*leds[ledNumber] = CRGB::pulsarPurple;
      leds[ledNumber-1] = CRGB::pulsarBlue;
      
      ledNumber++;
      if(ledNumber == NUM_LEDS)
      {
        ledNumber = 0;
        leds[38] = CRGB::pulsarBlue;
      }*/
      
      /*
      for(uint8_t i = 0; i < 7; i++)
      {
        leds[keyIDtoLedID(keyboardPacket.key[i])] = CRGB::pulsarBlue;
      }    
      */

      //ChangePalettePeriodically();

      //currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;    static uint8_t startIndex = 0;
      //startIndex = startIndex + 1; /* motion speed */

      //FillLEDsFromPaletteColors( startIndex);
    
      //pulsar();
      FastLED.setBrightness(ledBrightness);
      FastLED.show();
      //FastLED.delay(1000 / UPDATES_PER_SECOND);

    ledTask.endTime = micros();
    ledTask.counter++;
    ledTask.duration = ledTask.endTime - ledTask.beginTime;
  }

  //------------------------------------------------------srTask
  if(micros() - srTask.beginTime >= srTask.interval)
  {
    srTask.beginTime = micros();
    srTask.inBetweenTime = srTask.beginTime - srTask.endTime;
    //Serial.println(srTask.duration);
      //Read SPI from shift register

      srSpi->beginTransaction(settingsA);
      digitalWrite(SR_CE, LOW);
      digitalWrite(SR_PL, HIGH);
          
      for(uint8_t packet = 0; packet < 5; packet++)
      {
        spiPacket[packet] = 0xFF - srSpi->transfer(0);
        //Serial.print(spiPacket[packet], BIN);
        //Serial.print(" ");
      }

      digitalWrite(SR_CE, HIGH);
      digitalWrite(SR_PL, LOW);
      srSpi->endTransaction();

      //Read pressed keys
      numberOfPressedKeys = 0;

      for(uint8_t i = 0; i < 8; i++)
      {
        keyboardPacket.key[i] = 255;
      }

      for(uint8_t packet = 0; packet < 5; packet++)
      {
        for(uint8_t bit = 0; bit < 8; bit++)
        {
          bool isKeyPressed = spiPacket[packet] & (0b1 << bit);

          if(isKeyPressed == 1 && numberOfPressedKeys < 8)
          {
            keyboardPacket.key[numberOfPressedKeys] = (packet * 8) + bit;

            Serial.print("KeyID: 0x");
            Serial.println(keyboardPacket.key[numberOfPressedKeys], HEX);
            
            numberOfPressedKeys++;

            //Serial.print("  Number of pressed keys: ");
            //Serial.println(numberOfPressedKeys);
          }
          if(numberOfPressedKeys == 8)
          {
            telemetryPacket.error = tooManyKeysPressed;
            Serial.println("Too many keys pressed");
          }
        }
      }

    srTask.endTime = micros();
    srTask.counter++;
    srTask.duration = srTask.endTime - srTask.beginTime;
  }
  
  //send packet
  //------------------------------------------------------espnowTask
  if(micros() - espnowTask.beginTime >= espnowTask.interval)
  {
    espnowTask.beginTime = micros();
    espnowTask.inBetweenTime = espnowTask.beginTime - espnowTask.endTime;
    /*
    for(uint8_t i = 0; i < 255; i++)
    {
      Serial.print("Pressing: 0x");
      Serial.println(i, HEX);
      Keyboard.press(i);
      delay(10);
      Keyboard.release(i);
      delay(500);
    }*/
//------------------
      
      //Send all pressed keys to packet
      // Send message via ESP-NOW
      esp_err_t result = esp_now_send(dongleAddress, (uint8_t *) &keyboardPacket, sizeof(keyboardPacket));
        
      if (result == ESP_OK) 
      {
        //Serial.println("Sent with success");
      }
      else 
      {
        Serial.println("Error sending the data");
      }
//------------------
    espnowTask.endTime = micros();
    espnowTask.counter++;
    espnowTask.duration = espnowTask.endTime - espnowTask.beginTime;

  }

  //Deal with LED

  //Deal with other stuff
  //Serial.println(getXtalFrequencyMhz());
  //Serial.println(getApbFrequency());
  //Serial.println(getCpuFrequencyMhz());
  
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

  //srTask frequency counter
  if(srTask.counter == 0)
  {
    srTask.startCounterTime = micros();
  }
  if(micros() - srTask.startCounterTime > 1000000)
  {
    srTask.frequency = srTask.counter;
    //Serial.println(srTask.counter);
    srTask.counter = 0;
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
}

/* Typical task outline

  //------------------------------------------------------Task
  if(micros() - Task.beginTime >= Task.interval)
  {
    Task.beginTime = micros();
    Task.inBetweenTime = Task.beginTime - Task.endTime;

    **functions

    Task.endTime = micros();
    Task.counter++;
    Task.duration = Task.endTime - Task.beginTime;

  }

  //Task frequency counter
  if(Task.counter == 0)
  {
    Task.startCounterTime = micros();
  }
  if(micros() - Task.startCounterTime > 1000000)
  {
    Task.frequency = Task.counter;
    debug.println(Task.counter);
    Task.counter = 0;
  }

*/



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