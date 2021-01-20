/*
 * (C) 2016 Onwrikbaar <onwrikbaar@hotmail.com>
 */

class PumpState
{
  private:
    unsigned long timeMark = 0UL;               // The start time of an interval.
//  unsigned long millisecondsDelay;

  protected:
    float alphaRMA = 1.0;                       // The running moving average of alpha.

    bool waited(unsigned long milliseconds);
    void closeValve();
    void openValve();

  public:
    virtual void enter();
    virtual void leave();
    virtual PumpState *nextState();
    virtual float predictPressure(float basePr, float predPr, float prevPr, float lastPr) { return lastPr; };
};


class PS_Idle : public PumpState
{
  public:
    void enter();
    PumpState *nextState();
};


class PS_Pump : public PumpState
{
  public:
    void enter();
    PumpState *nextState();
};


class PS_Warn : public PumpState
{
  public:
    void enter();
    PumpState *nextState();
};


class PS_Hold : public PumpState
{
  public:
    PumpState *nextState();
    float predictPressure(float basePr, float predPr, float prevPr, float lastPr);
};

