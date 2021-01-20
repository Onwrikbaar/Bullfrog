/*
 * (C) 2016 Onwrikbaar <onwrikbaar@hotmail.com>
 */

#include "OutputPin.h"


class AirPump : public OutputPin
{
  private:
    uint8_t previousSpeed = 0;
    void setSpeed(uint8_t speed);

  public:
    AirPump(uint8_t pn);
    void start();
    void stop();
    void setFlowRate(float rate);
};

