#ifndef IOHANDLE_H
#define IOHANDLE_H



#include "SPI.h"
#include "variables.h"
#include "uartHandle.h"



bool areArraysEqual(uint8_t* arr1, uint8_t* arr2, int size) {
    for (int i = 0; i < size; i++) {
        if (arr1[i] != arr2[i]) {
            return false;
        }
    }
    return true;
}



void srLoop()
{
    // Read SPI from shift register

    srSpi->beginTransaction(settingsA);
    gpio_set_level((gpio_num_t)SR_CE, LOW);   //CLK_INH
    gpio_set_level((gpio_num_t)SR_PL, HIGH);  //SH/_LD

    for(uint8_t packet = 0; packet < NUMBER_OF_SR; packet++)
    {
      spiPacket[packet] = srSpi->transfer(0);
      //Serial.print(spiPacket[packet], BIN);
      //Serial.print(" ");
    }
    //Serial.println();

    gpio_set_level((gpio_num_t)SR_CE, HIGH);
    gpio_set_level((gpio_num_t)SR_PL, LOW);
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
            
            #ifdef DEBUG
            if(debug1)
            {
              Serial.printf("KeyID: 0x%02X\r\n", keyboardPacket.key[numberOfPressedKeys]);
            }
            #endif
            
            numberOfPressedKeys++;

            //Serial.print("  Number of pressed keys: ");
            //Serial.println(numberOfPressedKeys);
          }
          if(numberOfPressedKeys == 8)
          {
            telemetryPacket.error[0] = tooManyKeysPressed;  // TODO deal better with error
            return;
            //Serial.println("Too many keys pressed");
          }
      }
    }



    esp_err_t result;

    bool keyContentIsEqual = areArraysEqual(keyboardPacket.key, previousKeyboardPacket.key, 8);

    //Serial.printf("Arrays are equals: %u   ", keyContentIsEqual);
    //Serial.printf("key0: %u, prevKey0: %u\r\n", keyboardPacket.key[0], previousKeyboardPacket.key[0]);

    if(!keyContentIsEqual || (result != ESP_OK))
    {
      result = esp_now_send(dongleAddress, (uint8_t *) &keyboardPacket, sizeof(keyboardPacket));

      #ifdef DEBUG
      if(debug2)
      {
        if (result == ESP_OK)
        {
          Serial.println("ESP_OK");
        }
        else if(result == ESP_ERR_ESPNOW_NOT_INIT)
        {
          Serial.println("ESP_ERR_ESPNOW_NOT_INIT");
        }
        else if(result == ESP_ERR_ESPNOW_ARG)
        {
          Serial.println("ESP_ERR_ESPNOW_ARG");
        }
        else if(result == ESP_ERR_ESPNOW_INTERNAL)
        {
          Serial.println("ESP_ERR_ESPNOW_INTERNAL");
        }
        else if(result == ESP_ERR_ESPNOW_NO_MEM)
        {
          Serial.println("ESP_ERR_ESPNOW_NO_MEM");
        }
        else if(result == ESP_ERR_ESPNOW_NOT_FOUND)
        {
          Serial.println("ESP_ERR_ESPNOW_NOT_FOUND");
        }
        else if(result == ESP_ERR_ESPNOW_IF)
        {
          Serial.println("ESP_ERR_ESPNOW_IF");
        }
      }
      #endif

      for(uint8_t i = 0; i < 8; i++)
      {
        previousKeyboardPacket.key[i] = keyboardPacket.key[i];
      } 
    }

  
  
}



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



void reLoop()
{
  knobPacket.knob[0] = rotary;
  /* 
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
    */
}


#endif