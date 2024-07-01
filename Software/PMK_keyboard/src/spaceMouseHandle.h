#ifndef SPACEMOUSEHANDLE_H
#define SPACEMOUSEHANDLE_H

#include "Wire.h"
#include "variables.h"


void smSetup()
{
    Wire.beginTransmission(LDC1612_ADDRESS);
}


void smLoop()
{
    
}

#endif