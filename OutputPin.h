/*
 * (C) 2016 Onwrikbaar <onwrikbaar@hotmail.com>
 */

#ifndef __OUTPUTPIN_H__
#define __OUTPUTPIN_H__

#include <stdint.h>


class OutputPin
{
  private:
    uint8_t pinNr;

  protected:
    void pwmWrite(uint8_t val);

  public:
    OutputPin(uint8_t pn);
    void turnOn();
    void turnOff();
};

#endif

