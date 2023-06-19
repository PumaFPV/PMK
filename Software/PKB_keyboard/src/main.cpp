#include <Arduino.h>

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

uint8_t keyIDtoLedID(uint8_t keyID)
{
  uint8_t ledID[NUM_LEDS+1] = 
  {
    7, 0, 1, 2, 3, 4, 5,
    6, 21, 13, 12, 11, 10, 9,
    8, 20, 38, 14, 15, 16, 17,
    18, 19, 22, 28, 27, 26, 25, 
    24, 23, 36, 37, 100, 32, 
    33, 34, 35, 29, 30, 31
  };

  return ledID[keyID];
}

uint8_t ledIDtoKeyID(uint8_t ledID)
{
  uint8_t keyID[40] = 
  {
    1, 2, 3, 4, 5, 6, 7, 0, 14, 13, 12, 11, 10, 9, 17, 18, 19, 20, 21, 22, 15, 8, 23, 29, 28, 27, 26, 25, 24, 37, 38, 39, 33, 34, 35, 36, 30, 31, 16
  };

  return keyID[ledID];
}

uint8_t keyIDtoChar(uint8_t keyID, uint8_t layer)
{
  uint8_t keyChar[8][40] = 
  {
    {
      '5','4','3','2','1','0',KEY_ESC,
      KEY_TAB,'z','q','w','e','r','t',
      'g','f','d','s','a',0,KEY_CAPS_LOCK,
      KEY_LEFT_SHIFT,'<','y','x','c','v','b',
      0,KEY_LEFT_GUI,KEY_LEFT_ALT,KEY_LEFT_CTRL,KEY_RETURN,' ',
      KEY_DELETE,KEY_BACKSPACE,0,0,0,0
    },
    {
      KEY_F5,KEY_F4,KEY_F3,KEY_F2,KEY_F1,0,KEY_ESC,
      KEY_TAB,0,0,KEY_UP_ARROW,0,0,0,
      0,0,KEY_RIGHT_ARROW,KEY_DOWN_ARROW,KEY_LEFT_ARROW,0,KEY_CAPS_LOCK,
      KEY_LEFT_SHIFT,0,0,0,0,0,0,
      0,KEY_LEFT_GUI,KEY_LEFT_ALT,KEY_LEFT_CTRL,KEY_RETURN,' ',
      KEY_DELETE,KEY_BACKSPACE,0,0,0,0
    },
    {
      0,0,0,0,0,0,KEY_ESC,
      KEY_TAB,0,0,0,0,0,0,
      0,0,0,0,0,0,KEY_CAPS_LOCK,
      KEY_LEFT_SHIFT,0,0,0,0,0,0,
      0,KEY_LEFT_GUI,KEY_LEFT_ALT,KEY_LEFT_CTRL,KEY_RETURN,' ',
      KEY_DELETE,KEY_BACKSPACE,0,0,0,0
    },
    {
      0,0,0,0,0,0,KEY_ESC,
      KEY_TAB,0,0,0,0,0,0,
      0,0,0,0,0,0,KEY_CAPS_LOCK,
      KEY_LEFT_SHIFT,0,0,0,0,0,0,
      0,KEY_LEFT_GUI,KEY_LEFT_ALT,KEY_LEFT_CTRL,KEY_RETURN,' ',
      KEY_DELETE,KEY_BACKSPACE,0,0,0,0
    },
    {
      0,0,0,0,0,0,KEY_ESC,
      KEY_TAB,0,0,0,0,0,0,
      0,0,0,0,0,0,KEY_CAPS_LOCK,
      KEY_LEFT_SHIFT,0,0,0,0,0,0,
      0,KEY_LEFT_GUI,KEY_LEFT_ALT,KEY_LEFT_CTRL,KEY_RETURN,' ',
      KEY_DELETE,KEY_BACKSPACE,0,0,0,0
    },
    {
      0,0,0,0,0,0,KEY_ESC,
      KEY_TAB,0,0,0,0,0,0,
      0,0,0,0,0,0,KEY_CAPS_LOCK,
      KEY_LEFT_SHIFT,0,0,0,0,0,0,
      0,KEY_LEFT_GUI,KEY_LEFT_ALT,KEY_LEFT_CTRL,KEY_RETURN,' ',
      KEY_DELETE,KEY_BACKSPACE,0,0,0,0
    },
    {
      0,0,0,0,0,0,KEY_ESC,
      KEY_TAB,0,0,0,0,0,0,
      0,0,0,0,0,0,KEY_CAPS_LOCK,
      KEY_LEFT_SHIFT,0,0,0,0,0,0,
      0,KEY_LEFT_GUI,KEY_LEFT_ALT,KEY_LEFT_CTRL,KEY_RETURN,' ',
      KEY_DELETE,KEY_BACKSPACE,0,0,0,0
    },
    {
      0,0,0,0,0,0,KEY_ESC,
      KEY_TAB,0,0,0,0,0,0,
      0,0,0,0,0,0,KEY_CAPS_LOCK,
      KEY_LEFT_SHIFT,0,0,0,0,0,0,
      0,KEY_LEFT_GUI,KEY_LEFT_ALT,KEY_LEFT_CTRL,KEY_RETURN,' ',
      KEY_DELETE,KEY_BACKSPACE,0,0,0,0
    },
  };
  
  return keyChar[layer][keyIDtoLedID(keyID)];
}

uint8_t keyCoordinatesToLedID(uint8_t x, uint8_t y)
{
  uint8_t ledID[6][9] =
    {
      {6,5,4,3,2,1,0,100,100},
      {7,8,9,10,11,12,13,100,100},
      {20,19,18,17,16,15,14,100,100},
      {21,22,23,24,25,26,27,28,29},
      {39,38,37,36,35,34,33,32,30},
      {100,100,100,100,100,100,100,100,31}

    };
    return ledID[x][y];
}

uint8_t layerKeyID[8] = { //If no layer, put 0xFE which is key 254, which doesnt exist. Key 255 is no key...
  0x10,
  0x1F,
  0x1E,
  0xFE,
  0xFE,
  0xFE,
  0xFE,
  0xFE
};

uint32_t ledColorProfile[8][NUM_LEDS] = 
{
  {
    0x000000, 0x000000, 0xFF0000, 0xFF0000, 0xFF0000, 0xFF0000, 0xDEB887,
    0x9966CC, 0x32CD32, 0xFFFF00, 0x32CD32, 0xFFFF00, 0xFF0000, 0xDEB887, 
    0xFF0000, 0x0000FF, 0x32CD32, 0x32CD32, 0x32CD32, 0x000000, 0x32CD32, 
    0x32CD32, 0xFF0000, 0x000000, 0xEE82EE , 0x32CD32, 0x111111, 0xFF0000, 
    0x000000, 0x000000, 0x000000, 0x32CD32, 0x000000, 0x32CD32, 0x000000, 
    0x000000, 0x000000, 0x000000, 0x000000
  },
  {
    0x00FF00, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x00FF00,
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111
  },
  {
    0xFF0000, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0xFF0000,
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111
  },
  {
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111,
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111
  },
  {
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111,
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111
  },
  {
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111,
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111
  },
  {
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111,
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111
  },
  {
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111,
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111
  }
};

void setLedColorProfile(uint8_t profile)
{
  for(uint8_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ledColorProfile[profile][i];
  }
}


void pulsar()
{

  for(uint8_t brightness = 40; brightness < 250; brightness++)
  {
    for(uint8_t x = 0; x < 3; x++)
    {
      for(uint8_t y = 0; y < 7; y++)
      {
        uint8_t ledID = keyCoordinatesToLedID(y,x);
        //FastLED.setBrightness(255);
        leds[ledID] = CRGB::pulsarPurple;
      }
    }
    for(uint8_t x = 3; x < 4; x++)
    {
      for(uint8_t y = 0; y < 7; y++)
      {
        uint8_t ledID = keyCoordinatesToLedID(y,x);
        //FastLED.setBrightness(150);
        leds[ledID] = CRGB::pulsarPurpleBlue;
      }
    }      
    for(uint8_t x = 4; x < 9; x++)
    {
      for(uint8_t y = 0; y < 7; y++)
      {
        uint8_t ledID = keyCoordinatesToLedID(y,x);
        //FastLED.setBrightness(brightness);
        
        leds[ledID] = CRGB::pulsarBlue;
        leds[ledID].subtractFromRGB(brightness);
      }
    }
    FastLED.show();
    delay(6);
  }

  for(uint8_t brightness = 250; brightness > 40; brightness--)
  {
    for(uint8_t x = 0; x < 3; x++)
    {
      for(uint8_t y = 0; y < 7; y++)
      {
        uint8_t ledID = keyCoordinatesToLedID(y,x);
        //FastLED.setBrightness(255);
        leds[ledID] = CRGB::pulsarPurple;
      }
    }
    for(uint8_t x = 3; x < 4; x++)
    {
      for(uint8_t y = 0; y < 7; y++)
      {
        uint8_t ledID = keyCoordinatesToLedID(y,x);
        //FastLED.setBrightness(150);
        leds[ledID] = CRGB::pulsarPurpleBlue;
      }
    }      
    for(uint8_t x = 4; x < 9; x++)
    {
      for(uint8_t y = 0; y < 7; y++)
      {
        uint8_t ledID = keyCoordinatesToLedID(y,x);
        //FastLED.setBrightness(brightness);
        leds[ledID] = CRGB::pulsarBlue;
        leds[ledID].subtractFromRGB(brightness);

      }
    }
    FastLED.show();
    delay(10);
  }
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

            //Serial.print("KeyID: 0x");
            //Serial.println(keyboardPacket.key[numberOfPressedKeys], HEX);
            
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

//------------------
      //layerID = 0;
      for(uint8_t i = 0; i < 8; i++)
      {
        for(uint8_t j = 0; j < 8; j++)
        {
          if(keyboardPacket.key[i] == layerKeyID[j])
          {
            layerID = j;
            i = 9;
            j = 9;
          }
        }
      }

      for(uint8_t i = 0; i < 8; i++)
      {
        if(keyboardPacket.key[i] != 255 && layerID != settingLayerID)
        {
          Keyboard.press(keyIDtoChar(keyboardPacket.key[i], layerID));
          //Serial.print("Pressing: 0x");
          //Serial.println(keyIDtoChar(keyboardPacket.key[i], layerID), HEX);
        }
        else if (keyboardPacket.key[i] != 255 && layerID == settingLayerID)
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