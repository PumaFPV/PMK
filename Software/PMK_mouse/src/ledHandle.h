#include "Arduino.h"

void ledSetup()
{
    //Demo each LED by turning them on individually for one second. 
    Serial.println("Turn on LEDs individually");
    digitalWrite(PIN_TRACKBALL_LED_WHITE, HIGH);
    delay(1000);
    digitalWrite(PIN_TRACKBALL_LED_WHITE, LOW);
    
    digitalWrite(PIN_TRACKBALL_LED_GREEN, HIGH);
    delay(100);
    digitalWrite(PIN_TRACKBALL_LED_GREEN, LOW);
    
    digitalWrite(PIN_TRACKBALL_LED_RED, HIGH);
    delay(100);
    digitalWrite(PIN_TRACKBALL_LED_RED, LOW);
    
    digitalWrite(PIN_TRACKBALL_LED_BLUE, HIGH);
    delay(100);
    digitalWrite(PIN_TRACKBALL_LED_BLUE, LOW);
    digitalWrite(PIN_TRACKBALL_LED_WHITE, HIGH);
}