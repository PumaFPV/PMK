#include "Arduino.h"
#include "variables.h"

void pinModeDef()
{
    //Define pin functionality on the Arduino
    pinMode(PIN_TRACKBALL_BUTTON,    INPUT);
    pinMode(PIN_TRACKBALL_LEFT,      INPUT);
    pinMode(PIN_TRACKBALL_RIGHT,     INPUT);
    pinMode(PIN_TRACKBALL_UP,        INPUT);
    pinMode(PIN_TRACKBALL_DOWN,      INPUT);
    pinMode(PIN_TRACKBALL_LED_WHITE, OUTPUT);
    pinMode(PIN_TRACKBALL_LED_GREEN, OUTPUT);
    pinMode(PIN_TRACKBALL_LED_RED,   OUTPUT);
    pinMode(PIN_TRACKBALL_LED_BLUE,  OUTPUT);

    //Pull LED pins low to prevent flickering
    digitalWrite(PIN_TRACKBALL_LED_WHITE, LOW);
    digitalWrite(PIN_TRACKBALL_LED_GREEN, LOW);
    digitalWrite(PIN_TRACKBALL_LED_RED,   LOW);
    digitalWrite(PIN_TRACKBALL_LED_BLUE,  LOW);
}