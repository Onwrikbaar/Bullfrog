/*
 * (C) 2016 Onwrikbaar <onwrikbaar@hotmail.com>
 */

typedef struct {
    float loMin, loMax;
    float hiMin, hiMax;
} PressureLimits;


class Inflatable
{
  private:
    const PressureLimits *limits;
    volatile float lastPressure;                // This value is maintained by the data acquisition interrupt routine.
    float thresholdLo;
    float thresholdHi;

    void resetLimits();

  public:
    Inflatable(const PressureLimits *pl);
    void updatePressure(float pressure);
    float pressure();
    float pressureOverMin();
    float pressureOverMax();
    bool cycleDone(float step);
    float flowRate();
};

