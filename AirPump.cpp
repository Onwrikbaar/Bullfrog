/*
 * (C) 2016 Onwrikbaar <onwrikbaar@hotmail.com>
 */

#include <math.h>
#include "AirPump.h"

#define PUMP_SPEED_MIN          120
#define PUMP_SPEED_MAX          240

// Constructor.
AirPump::AirPump(uint8_t pn) : OutputPin(pn)
{
    // Nothing more to do.
}


void AirPump::setSpeed(uint8_t speed)
{
    // Note: For a 3V pump the on-time must be scaled down proportionally if the supply voltage exceeds 3V.
    if (speed != previousSpeed) {               // Only call the library if the value has changed.
        pwmWrite(previousSpeed = speed);
    }
}


void AirPump::start()
{
    setSpeed(PUMP_SPEED_MIN);
}


void AirPump::stop()
{
    setSpeed(0);
}


void AirPump::setFlowRate(float rate)
{
    if (rate >= 0.0 && rate <= 1.0) {
        setSpeed(PUMP_SPEED_MIN + round(rate * (float)(PUMP_SPEED_MAX - PUMP_SPEED_MIN)));
    }
}

