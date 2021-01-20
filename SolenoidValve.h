/*
 * (C) 2016 Onwrikbaar <onwrikbaar@hotmail.com>
 */

#include "OutputPin.h"


class SolenoidValve : public OutputPin
{
  public:
    SolenoidValve(uint8_t pn);
    void open();
    void close();
};

