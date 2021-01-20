/*
 * (C) 2016 Onwrikbaar <onwrikbaar@hotmail.com>
 */

#include "SolenoidValve.h"

// Constructor.
SolenoidValve::SolenoidValve(uint8_t pn) : OutputPin(pn)
{
    // Nothing more to do.
}


void SolenoidValve::open()
{
    turnOff();
}


void SolenoidValve::close()
{
    turnOn();
}

