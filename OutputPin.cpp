/*
 * (C) 2016 Onwrikbaar <onwrikbaar@hotmail.com>
 */

#include <Arduino.h>
#include "OutputPin.h"

// Constructor.
OutputPin::OutputPin(uint8_t pn)
{
    pinNr = pn;
    digitalWrite(pinNr, LOW);
    pinMode(pinNr, OUTPUT);
}


void OutputPin::pwmWrite(uint8_t val)
{
    analogWrite((unsigned int)pinNr, val);
}


void OutputPin::turnOff()
{
    digitalWrite((unsigned int)pinNr, LOW);
}


void OutputPin::turnOn()
{
    digitalWrite((unsigned int)pinNr, HIGH);
}

