#ifndef TRACKPAD_H
#define TRACKPAD_H

#include "variables.h"
#include "CirquePinnacle.h"



void trackpadSetup()
{
    if(!trackpad.begin())
    {
        Serial.printf("Cirque Pinnacle not responding!\r\n");
    }
    trackpad.setDataMode(PINNACLE_ABSOLUTE);
    trackpad.absoluteModeConfig(1);  // set count of z-idle packets to 1
}



void trackpadLoop()
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
    }
}

#endif