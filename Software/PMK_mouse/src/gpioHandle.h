#include "Arduino.h"
#include "variables.h"

void IRAM_ATTR upISR()
{
  yPosition--;
}

void IRAM_ATTR downISR()
{
  yPosition++;
}

void IRAM_ATTR rightISR()
{
  xPosition++;
}

void IRAM_ATTR leftISR()
{
  xPosition--;
}