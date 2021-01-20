/*
 * (C) 2016 Onwrikbaar <onwrikbaar@hotmail.com>
 */

#include "Vibrator.h"

#define VIB_SPEED_MIN            70             // Must be large enough for the vib to start.
#define VIB_SPEED_MAX           110             // Keep below 128 when powering a 3V vib from a 6V supply.

// Constructor.
Vibrator::Vibrator(uint8_t pn) : OutputPin(pn)
{
    resetSpeed();
}


void Vibrator::setSpeed(uint8_t speed)
{
    // Note: For a 3V vibrator the on-time must be scaled down proportionally if the supply voltage exceeds 3V.
    pwmWrite(speed <= 128 ? speed : 128);
}


void Vibrator::start()
{
    setSpeed(speed);
}


void Vibrator::stop(int8_t bump)
{
    setSpeed(0);                                // Switch off.
    if (bump >= 0) {                            // TODO Check for overflow.
        if (speed < VIB_SPEED_MAX) speed += bump;
    } else {                                    // TODO Check for underflow.
        if (speed > VIB_SPEED_MIN) speed += bump;
    }
}


void Vibrator::resetSpeed()
{
    speed = VIB_SPEED_MIN;
}

