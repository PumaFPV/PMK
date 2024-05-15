#define LEFT_KB 0
#define RIGHT_KB 1

#include <Arduino.h>

#include "config.h"

#include "SPI.h"
#include "WiFi.h"
#include "esp_now.h"
#include "LittleFS.h"
#include "FastLED.h"
#include "pmk.h"

#include "variables.h"

uint8_t deviceID;
#define FIRMWARE_REV "dev"

#include "uartHandle.h"
#include "USBHandle.h"
#include "espNowHandle.h"
#include "ledHandle.h"



void loopCount();


int rotary = 0;

void IRAM_ATTR rotaryEncoderISR()
{

  static unsigned long time = 0;
  const unsigned long interval = 10;

  if(xTaskGetTickCount() - time > interval)
  {
    detachInterrupt(RE_A);

    if(digitalRead(RE_B))
    {
      rotary--;
    }
    else
    {
      rotary++;
    }
    time = xTaskGetTickCount();

    attachInterrupt(RE_A, rotaryEncoderISR, RISING);
  }
}



void setup() 
{

  keyboardPacket.deviceID = RIGHT_KB; 
  mousePacket.deviceID = keyboardPacket.deviceID;

  //-----Serial
  Serial.begin(115200);
  //while(!Serial){}  //Optional debug help
  //delay(100);

  //-----CPU
  Serial.printf("CPU Freq: %i", getCpuFrequencyMhz());
  Serial.printf("XTAL Freq: %i", getXtalFrequencyMhz());

  setCpuFrequencyMhz(80);
  Serial.printf("CPU Freq: %i", getCpuFrequencyMhz());



  //-----USB

  //-----Shift register
  pinMode(SR_PL, OUTPUT);


  //Initialize SPI for SR
  srSpi = new SPIClass(SR_SPI_BUS);
  srSpi->begin(SR_CLK, SR_MISO, -1, SR_CE);
  pinMode(srSpi->pinSS(), OUTPUT);

  
  //-----Rotary Encoder
  pinMode(RE_A, INPUT_PULLUP);
  pinMode(RE_B, INPUT_PULLUP);

  attachInterrupt(RE_A, rotaryEncoderISR, RISING);
  //attachInterrupt(digitalPinToInterrupt(RE_A), handleEncoder, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(RE_B), handleEncoder, CHANGE);


  //-----Leds
  FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(leds, NUM_LEDS);

  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;


  //-----ESP NOW

  WiFi.mode(WIFI_STA);
  delay(10);
  WiFi.setTxPower(WIFI_POWER_19_5dBm);

  Serial.printf("WiFi power: ");
  Serial.println(WiFi.getTxPower());

  Serial.printf("Keyboard MAC address: ");
  Serial.println(WiFi.macAddress());

  if(esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register dongles
  for(uint8_t i = 0; i < 3; i++)
  {
    memcpy(peerInfo.peer_addr, dongleAddress, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;

    // Add dongle        
    if(esp_now_add_peer(&peerInfo) != ESP_OK)
    {
      Serial.printf("Failed to add dongle: %i\r\n", i);
      return;
    }
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


  //------------------------------------------------------
  //------------------------------------------------------ledTask
  //------------------------------------------------------
  if(micros() - ledTask.beginTime >= ledTask.interval)
  {
    ledTask.beginTime = micros();
    ledTask.inBetweenTime = ledTask.beginTime - ledTask.endTime;

      setLedColorProfile(0);

      //FastLED.setBrightness(ledBrightness);
      //Serial.printf("Brightness: %i\r\n", ledBrightness);
      static bool rising = false;
      const uint8_t minBrightness = 20;
      const uint8_t maxBrightness = 120;

      if(rising) 
      {
        if(ledBrightness < maxBrightness)
        {
          ledBrightness++;
        }
        else
        {
          rising = false;
        }
      } 
      else
      {
        if(ledBrightness > minBrightness) 
        {
          ledBrightness--;
        }
        else
        {
          rising = true;
        }
      }

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
      /*
      if(i == 29)
      {
        i = 0;
        for(uint8_t j = 0; j < 29; j++)
        {
          leds[j] = CRGB::Black;
        }
      }*/
      //FastLED.delay(1000 / UPDATES_PER_SECOND);

    ledTask.endTime = micros();
    ledTask.counter++;
    ledTask.duration = ledTask.endTime - ledTask.beginTime;
  }



  //------------------------------------------------------
  //------------------------------------------------------srTask
  //------------------------------------------------------
  if(micros() - srTask.beginTime >= srTask.interval)
  {
    srTask.beginTime = micros();
    srTask.inBetweenTime = srTask.beginTime - srTask.endTime;
    //Serial.println(srTask.duration);
      //Read SPI from shift register

      srSpi->beginTransaction(settingsA);
      digitalWrite(SR_CE, LOW);   //CLK_INH
      digitalWrite(SR_PL, HIGH);  //SH/_LD

      for(uint8_t packet = 0; packet < NUMBER_OF_SR; packet++)
      {
        spiPacket[packet] = srSpi->transfer(0);
        //Serial.print(spiPacket[packet], BIN);
        //Serial.print(" ");
      }
      //Serial.println();

      digitalWrite(SR_CE, HIGH);
      digitalWrite(SR_PL, LOW);
      srSpi->endTransaction();

      //Read pressed keys
      numberOfPressedKeys = 0;

      for(uint8_t i = 0; i < 8; i++)
      {
        keyboardPacket.key[i] = 0;
      }

      for(uint8_t packet = 0; packet < NUMBER_OF_SR; packet++)
      {
        for(uint8_t bit = 0; bit < 8; bit++)
        {
          bool isKeyPressed = spiPacket[packet] & (0b1 << (7 - bit));

          if(isKeyPressed == 0 && numberOfPressedKeys < MAX_NUMBER_OF_KEYS)
          {
            keyboardPacket.key[numberOfPressedKeys] = (packet * 8) + bit + 1; //+1 so we dont have a keyID = 0

            Serial.print("KeyID: 0x");
            Serial.println(keyboardPacket.key[numberOfPressedKeys], HEX);
            
            numberOfPressedKeys++;

            //Serial.print("  Number of pressed keys: ");
            //Serial.println(numberOfPressedKeys);
          }
          if(numberOfPressedKeys == 8)
          {
            telemetryPacket.error = tooManyKeysPressed;
            return;
            //Serial.println("Too many keys pressed");
          }
        }
      }

    srTask.endTime = micros();
    srTask.counter++;
    srTask.duration = srTask.endTime - srTask.beginTime;
  }
  

  //------------------------------------------------------
  //------------------------------------------------------espnowTask
  //------------------------------------------------------
  if(micros() - espnowTask.beginTime >= espnowTask.interval)
  {
    espnowTask.beginTime = micros();
    espnowTask.inBetweenTime = espnowTask.beginTime - espnowTask.endTime;

//------------------
      
      //Send all pressed keys to packet
      // Send message via ESP-NOW
      static bool kb = 0;

      esp_err_t result;

      if(kb)
      {
        result = esp_now_send(dongleAddress, (uint8_t *) &keyboardPacket, sizeof(keyboardPacket));
        kb = 0;
      }
      else
      {
        result = esp_now_send(dongleAddress, (uint8_t *) &mousePacket, sizeof(mousePacket));
        kb = 1;
      }
        
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

  //------------------------------------------------------Rotary Encoder Task
  if(micros() - reTask.beginTime >= reTask.interval)
  {
    reTask.beginTime = micros();
    reTask.inBetweenTime = reTask.beginTime - reTask.endTime;

    //**functions

    if(mousePacket.deviceID == LEFT_KB)
    {
      mousePacket.x = rotary;
      rotary = 0;
    }
    if(mousePacket.deviceID == RIGHT_KB)
    {
      mousePacket.y = rotary;
      rotary = 0;
    }

    //Serial.printf("%i\r\n", rotary);

    reTask.endTime = micros();
    reTask.counter++;
    reTask.duration = reTask.endTime - reTask.beginTime;

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
  //Deal with other stuff
  //Serial.println(getXtalFrequencyMhz());
  //Serial.println(getApbFrequency());
  //Serial.println(getCpuFrequencyMhz());
  
}



void loopCount()
{
  //ledTask frequency counter
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

  //reTask frequency counter
  if(reTask.counter == 0)
  {
    reTask.startCounterTime = micros();
  }
  if(micros() - reTask.startCounterTime > 1000000)
  {
    reTask.frequency = reTask.counter;
    //Serial.println(reTask.counter);
    reTask.counter = 0;
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