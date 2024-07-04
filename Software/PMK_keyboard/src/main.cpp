#include <Arduino.h>

#include "SPI.h"
#include "Wire.h"
#include "WiFi.h"
#include "esp_now.h"
#include "FastLED.h"
#include "EEPROM.h"
#include "CirquePinnacle.h"
#include "spaceMouseHandle.h"
#include "pmk.h"

#include "variables.h"

#define FIRMWARE_REV "keyboard-dev"

#include "uartHandle.h"
#include "i2cHandle.h"
#include "espnowHandle.h"
#include "ledHandle.h"
#include "ioHandle.h"


void loopCount();



void setup() 
{
  //-----Serial
  Serial.begin(115200);
  //while(!Serial){}


  //-----CPU
  setCpuFrequencyMhz(80);



  //-----Shift register
  pinMode(SR_PL, OUTPUT);
  // Initialize SPI for SR
  srSpi = new SPIClass(SR_SPI_BUS);
  #ifdef HW01
    srSpi->begin(SR_CLK, SR_MISO, -1, SR_CE);
  #endif
  #ifdef HW02
    srSpi->begin(SR_CLK, SR_MISO, SPI_MOSI);
  #endif
  pinMode(srSpi->pinSS(), OUTPUT);



  //-----I2C
  #ifdef HW02

    Wire.begin(SDA, SCL, i2cClk);

    checkSideModule();
    
  #endif



  //-----Rotary Encoder
  pinMode(RE_A, INPUT_PULLUP);
  pinMode(RE_B, INPUT_PULLUP);
  attachInterrupt(RE_A, rotaryEncoderISR, RISING);



  //-----Leds
  FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(leds, NUM_LEDS);

  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;



  //-----EEPROM
  EEPROM.begin(EEPROM_SIZE);
  deviceID = EEPROM.read(DEVICEID_ADDRESS);
  keyboardPacket.deviceID = deviceID;
  mousePacket.deviceID = deviceID;
  knobPacket.deviceID = deviceID;
  
  for(uint8_t i = 0; i < MAC_ADDRESS_SIZE; i++)
  {
    dongleAddress[i] = EEPROM.read(DONGLE_MACADDRESS_ADDRESS + i);
    delay(50);
    Serial.print(dongleAddress[i], HEX);
  }
  Serial.printf("\r\n");



  //-----ESP NOW
  WiFi.mode(WIFI_STA);
  delay(10);
  WiFi.setTxPower(WIFI_POWER_19_5dBm);

  if(esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register dongles
  registerDongle();

  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);


  #ifdef HW02
    if(trackpadIsPresent)
    {
      if(!trackpad.begin())
      {
        Serial.println(F("Cirque Pinnacle not responding!"));
      }
      Serial.println(F("CirquePinnacle/examples/absolute_mode"));
      trackpad.setDataMode(PINNACLE_ABSOLUTE);
      trackpad.absoluteModeConfig(1);  // set count of z-idle packets to 1
    }
    
    if(spacemouseIsPresent)
    {
      spacemouseSetup();
    }
  
  #endif

}




void loop() 
{
  loopCount();



  //------------------------------------------------------
  //------------------------------------------------------ledTask
  //------------------------------------------------------
  if(micros() - ledTask.beginTime >= ledTask.interval)
  {
    ledTask.beginTime = micros();
    ledTask.inBetweenTime = ledTask.beginTime - ledTask.endTime;

      ledLoop();

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

      srLoop();

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

      espnowLoop();
      
    espnowTask.endTime = micros();
    espnowTask.counter++;
    espnowTask.duration = espnowTask.endTime - espnowTask.beginTime;

  }



  //------------------------------------------------------
  //------------------------------------------------------Rotary Encoder Task
  //------------------------------------------------------
  if(micros() - reTask.beginTime >= reTask.interval)
  {
    reTask.beginTime = micros();
    reTask.inBetweenTime = reTask.beginTime - reTask.endTime;

      reLoop();
      esp_now_send(dongleAddress, (uint8_t *) &knobPacket, sizeof(knobPacket));

    reTask.endTime = micros();
    reTask.counter++;
    reTask.duration = reTask.endTime - reTask.beginTime;

  }



  //------------------------------------------------------
  //------------------------------------------------------UART Task
  //------------------------------------------------------
  if(micros() - uartTask.beginTime >= uartTask.interval)
  {
    uartTask.beginTime = micros();
    uartTask.inBetweenTime = uartTask.beginTime - uartTask.endTime;

      handleUart();

    uartTask.endTime = micros();
    uartTask.counter++;
    uartTask.duration = uartTask.endTime - uartTask.beginTime;
  }



  //------------------------------------------------------
  //------------------------------------------------------cirque Task
  //------------------------------------------------------
  #ifdef HW02
  if(micros() - sideModuleTask.beginTime >= sideModuleTask.interval)
  {
    sideModuleTask.beginTime = micros();
    sideModuleTask.inBetweenTime = sideModuleTask.beginTime - sideModuleTask.endTime;

    if(trackpadIsPresent)
    {
      if(trackpad.available()) 
      {
        trackpad.read(&data);

        // datasheet recommends clamping the axes value to reliable range
        if(data.z)
        {  // only clamp values if Z axis is not idle.
          data.x = data.x > 1920 ? 1920 : (data.x < 128 ? 128 : data.x);  // 128 <= x <= 1920
          data.y = data.y > 1472 ? 1472 : (data.y < 64 ? 64 : data.y);    //  64 <= y <= 1472
        }

        mousePacket.x = prevData.x - data.x;
        mousePacket.y = prevData.y - data.y;
        esp_now_send(dongleAddress, (uint8_t *) &mousePacket, sizeof(mousePacket));

        prevData = data;
        //Serial.print(F("B1:"));
        //Serial.print(data.buttons & 1);
        //Serial.print(F(" B2:"));
        //Serial.print(data.buttons & 2);
        //Serial.print(F(" B3:"));
        //Serial.print(data.buttons & 4);
        //Serial.print(F("\tX:"));
        //Serial.print(data.x);
        //Serial.print(F("\tY:"));
        //Serial.print(data.y);
        //Serial.print(F("\tZ:"));
        //Serial.println(data.z);
      }
    }
    
    
    if(spacemouseIsPresent)
    {
      spacemouseLoop();
    }

    sideModuleTask.endTime = micros();
    sideModuleTask.counter++;
    sideModuleTask.duration = sideModuleTask.endTime - sideModuleTask.beginTime;
  }
  #endif

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

  //uartTask frequency counter
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

  //cirqueTask frequency counter
  if(sideModuleTask.counter == 0)
  {
    sideModuleTask.startCounterTime = micros();
  }
  if(micros() - sideModuleTask.startCounterTime > 1000000)
  {
    sideModuleTask.frequency = sideModuleTask.counter;
    //Serial.println(sideModuleTask.counter);
    sideModuleTask.counter = 0;
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
