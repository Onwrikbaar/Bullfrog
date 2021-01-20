/*
 * (C) 2016 Onwrikbaar <onwrikbaar@hotmail.com>
 */

#include "OutputPin.h"


class Vibrator : public OutputPin
{
  private:
    uint8_t speed;
    void setSpeed(uint8_t speed);

  public:
    Vibrator(uint8_t pn);
    void start();
    void stop(int8_t bump);
    void resetSpeed();
};

