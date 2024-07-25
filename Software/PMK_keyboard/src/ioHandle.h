#ifndef IOHANDLE_H
#define IOHANDLE_H



#include "SPI.h"
#include "variables.h"
#include "uartHandle.h"


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
            
            if(debug1)
            {
              Serial.printf("KeyID: 0x%02X\r\n", keyboardPacket.key[numberOfPressedKeys]);
            }
            
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

    result = esp_now_send(dongleAddress, (uint8_t *) &keyboardPacket, sizeof(keyboardPacket));
  
    if(debug2)
    {
      if (result == ESP_OK)
      {
        Serial.println("Sent with success");
      }
      else
      {
        Serial.println("Error sending the data");
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