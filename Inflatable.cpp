/*
 * (C) 2016 Onwrikbaar <onwrikbaar@hotmail.com>
 */

#include "Inflatable.h"

// Constructor.
Inflatable::Inflatable(const PressureLimits *pl)
{
    limits = pl;
    resetLimits();
    lastPressure = 0.0;
}


void Inflatable::resetLimits()
{
    thresholdLo = limits->loMin;
    thresholdHi = limits->hiMin;
}


void Inflatable::updatePressure(float pressure)
{
    lastPressure = pressure;
}


float Inflatable::pressure()
{
    return lastPressure;
}


float Inflatable::pressureOverMin()
{
    return (lastPressure - thresholdLo);
}


float Inflatable::pressureOverMax()
{
    return (lastPressure - thresholdHi);
}


bool Inflatable::cycleDone(float step)
{
    if ((thresholdHi += step) > limits->hiMax) {
        resetLimits();
        return true;
    }
    if ((thresholdLo += step) > limits->loMax) thresholdLo = limits->loMax;
    return false;
}


float Inflatable::flowRate()
{
    return (lastPressure / limits->hiMax);
}

