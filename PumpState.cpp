/*
 * (C) 2016 Onwrikbaar <onwrikbaar@hotmail.com>
 */

#include <Arduino.h>
#include "PumpState.h"


bool PumpState::waited(unsigned long milliseconds)
{
    const unsigned long currentTime = millis();
    if (timeMark == 0UL) timeMark = currentTime;
    if (currentTime >= (timeMark + milliseconds)) {
        timeMark = 0UL;
        return true;
    }
    return false;
}

void PumpState::enter()
{
//  Serial.println("# " + String(__PRETTY_FUNCTION__));
}

void PumpState::leave()
{
//  Serial.println("# " + String(__PRETTY_FUNCTION__));
}

// Not used yet.
float PS_Hold::predictPressure(float basePr, float predPr, float prevPr, float lastPr)
{
    return lastPr;
}

