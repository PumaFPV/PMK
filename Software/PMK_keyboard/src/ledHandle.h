#ifndef LEDHANDLE_H
#define LEDHANDLE_H



#include "pmk.h"
#include "variables.h"



uint32_t ledColorProfile[8][NUM_LEDS] = 
{
  {
    0x000000, 0x000000, 0xFF0000, 0xFF0000, 0xFF0000, 0xFF0000, 0xDEB887,
    0x9966CC, 0x32CD32, 0xFFFF00, 0x32CD32, 0xFFFF00, 0xFF0000, 0xDEB887, 
    0xFF0000, 0x0000FF, 0x32CD32, 0x32CD32, 0x32CD32, 0x000000, 0x32CD32, 
    0x32CD32, 0xFF0000, 0x000000, 0xEE82EE , 0x32CD32, 0x111111, 0xFF0000, 
    0x000000
  },
  {
    0x00FF00, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x00FF00,
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111
  },
  {
    0x00FF00, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x00FF00,
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111
  },  {
    0x00FF00, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x00FF00,
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111
  },  {
    0x00FF00, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x00FF00,
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111
  },  {
    0x00FF00, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x00FF00,
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111
  },  {
    0x00FF00, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x00FF00,
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111
  },  {
    0x00FF00, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x00FF00,
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 0x111111, 
    0x111111
  }
};



void FillLEDsFromPaletteColors(uint8_t colorIndex);
void SetupTotallyRandomPalette();
void SetupBlackAndWhiteStripedPalette();
void SetupPurpleAndGreenPalette();
void ChangePalettePeriodically();
void pulsar(uint8_t minBrightness, uint8_t maxBrightness);



void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    
    for( int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}



// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.  All are shown here.



// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; ++i) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}



// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
    
}



// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}



// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 pulsarPalette PROGMEM =
{
    pulsarPurple,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    pulsarBlue,
    CRGB::Black,
    
    pulsarPurple,
    CRGB::Gray,
    pulsarBlue,
    CRGB::Black,
    
    pulsarPurple,
    pulsarPurple,
    CRGB::Gray,
    CRGB::Gray,
    pulsarBlue,
    pulsarBlue,
    CRGB::Black,
    CRGB::Black
};



// Additional notes on FastLED compact palettes:
//
// Normally, in computer graphics, the palette (or "color lookup table")
// has 256 entries, each containing a specific 24-bit RGB color.  You can then
// index into the color palette using a simple 8-bit (one byte) value.
// A 256-entry color palette takes up 768 bytes of RAM, which on Arduino
// is quite possibly "too many" bytes.
//
// FastLED does offer traditional 256-element palettes, for setups that
// can afford the 768-byte cost in RAM.
//
// However, FastLED also offers a compact alternative.  FastLED offers
// palettes that store 16 distinct entries, but can be accessed AS IF
// they actually have 256 entries; this is accomplished by interpolating
// between the 16 explicit entries to create fifteen intermediate palette
// entries between each pair.
//
// So for example, if you set the first two explicit entries of a compact 
// palette to Green (0,255,0) and Blue (0,0,255), and then retrieved 
// the first sixteen entries from the virtual palette (of 256), you'd get
// Green, followed by a smooth gradient from green-to-blue, and then Blue.
void ChangePalettePeriodically()
{
    uint8_t secondHand = (millis() / 1000) % 60;
    static uint8_t lastSecond = 99;
    
    if( lastSecond != secondHand) {
        lastSecond = secondHand;
        if( secondHand ==  0)  { currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
        if( secondHand == 10)  { currentPalette = RainbowStripeColors_p;   currentBlending = NOBLEND;  }
        if( secondHand == 15)  { currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; }
        if( secondHand == 20)  { SetupPurpleAndGreenPalette();             currentBlending = LINEARBLEND; }
        if( secondHand == 25)  { SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; }
        if( secondHand == 30)  { SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; }
        if( secondHand == 35)  { SetupBlackAndWhiteStripedPalette();       currentBlending = LINEARBLEND; }
        if( secondHand == 40)  { currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 45)  { currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 50)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  }
        if( secondHand == 55)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; }
    }
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
};



void pulsar(uint8_t minBrightness, uint8_t maxBrightness, bool breathingSide /*0=left - 1=right*/)
{

  uint8_t brightness = 0;
  bool rising = 1;

  pulsarNoDelay.currentMillis = millis();
  if(pulsarNoDelay.currentMillis - pulsarNoDelay.previousMillis > pulsarNoDelay.interval)
  {

    for(uint8_t x = 0; x < 3; x++)
    {
      for(uint8_t y = 0; y < 7; y++)
      {
        uint8_t ledID = keyCoordinatesToLedID(y,x);
        leds[ledID] = pulsarPurple;
      }
    }
    for(uint8_t x = 3; x < 4; x++)
    {
      for(uint8_t y = 0; y < 7; y++)
      {
        uint8_t ledID = keyCoordinatesToLedID(y,x);
        leds[ledID] = pulsarPurpleBlue;
      }
    }      
    for(uint8_t x = 4; x < 9; x++)
    {
      for(uint8_t y = 0; y < 7; y++)
      {
        uint8_t ledID = keyCoordinatesToLedID(y,x);
        leds[ledID] = pulsarBlue;
        leds[ledID].subtractFromRGB(brightness);
      }
    }

    if(brightness == maxBrightness)
    {
      rising = 0;
      pulsarNoDelay.interval = 10;
    }
    
    if(brightness == minBrightness)
    {
      rising = 1;
      pulsarNoDelay.interval = 6;
    }  
    
    pulsarNoDelay.previousMillis = pulsarNoDelay.currentMillis;
  }
}



void setLedColorProfile(uint8_t profile)
{
  switch (profile)
  {
  case 0:
    for(uint8_t i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = CRGB::White; //ledColorProfile[profile][i];
    }
    break;
  
  case 1: 
    for(uint8_t i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = ledColorProfile[profile][i];
    }
    break;
  
  case 2: 
    for(uint8_t i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = ledColorProfile[profile][i];
    }
    break;

  case 3:
    pulsar(40, 250, 1);
    break;

  }
};

void ledBreathingEffect()
{
  static bool rising = false;
  const uint8_t minBrightness = 20;
  const uint8_t maxBrightness = 120;
  //ledBrightness = 255;

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

  FastLED.setBrightness(ledBrightness);
}

void ledLoop()
{
  setLedColorProfile(0);

  #ifndef DEBUG
  ledBreathingEffect();
  #else
  FastLED.setBrightness(10);
  #endif

  //FastLED.setBrightness(ledBrightness);
  //Serial.printf("Brightness: %i\r\n", ledBrightness);
  
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
  if(ledSleep == 1)
  {
    FastLED.setBrightness(0);
  }
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
}


#endif