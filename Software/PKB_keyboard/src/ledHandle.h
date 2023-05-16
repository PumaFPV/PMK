void FillLEDsFromPaletteColors(uint8_t colorIndex);
void SetupTotallyRandomPalette();
void SetupBlackAndWhiteStripedPalette();
void SetupPurpleAndGreenPalette();
void ChangePalettePeriodically();
void pulsar(uint8_t minBrightness, uint8_t maxBrightness);

void ledLoop()
{

}


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
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::pulsarPurple,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::pulsarBlue,
    CRGB::Black,
    
    CRGB::pulsarPurple,
    CRGB::Gray,
    CRGB::pulsarBlue,
    CRGB::Black,
    
    CRGB::pulsarPurple,
    CRGB::pulsarPurple,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::pulsarBlue,
    CRGB::pulsarBlue,
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


void pulsar(uint8_t minBrightness, uint8_t maxBrightness)
{
  uint8_t brightness;
  bool rising;
    Serial.println(brightness);


  if(brightness == 0)
  {
    rising = 1;
  }

  pulsarNoDelay.currentMillis = millis();

  if(brightness < maxBrightness && (pulsarNoDelay.currentMillis - pulsarNoDelay.previousMillis) >= pulsarNoDelay.interval && rising)
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

    pulsarNoDelay.previousMillis = pulsarNoDelay.currentMillis;
    brightness++;
    
    if(brightness == maxBrightness)
    {
      rising = 0;
      pulsarNoDelay.interval = 10;
    }
  }
    
  if(brightness > minBrightness && (pulsarNoDelay.currentMillis - pulsarNoDelay.previousMillis) >= pulsarNoDelay.interval && !rising)
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

    pulsarNoDelay.previousMillis = pulsarNoDelay.currentMillis;
    brightness--;

    if(brightness == minBrightness)
    {
      rising = 1;
      pulsarNoDelay.interval = 6;
    }  
  }
}