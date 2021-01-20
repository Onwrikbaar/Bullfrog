/*
 * Bullfrog -- control program for an inflatable sex toy.
 * Video: https://www.youtube.com/watch?v=NQf2FUqjfuk
 * Instructable: https://fetlife.com/users/233604/posts/3745574
 * (C) 2016, 2017 Onwrikbaar <onwrikbaar@hotmail.com>
 */

#include <avr/wdt.h>
#include <TimerOne.h>                           // Get this library here: https://code.google.com/archive/p/arduino-timerone/downloads
#include "RingBuffer.h"
#include "AirPump.h"
#include "SolenoidValve.h"
#include "Vibrator.h"
#include "Inflatable.h"
#include "JsonParser.h"
#include "PumpState.h"

// Arduino pin assignments.
#define PRS_CHANNEL             A2              // Analog input pin for the pressure sensor (e.g. the NXP MPX5050GP).
#define POT_CHANNEL             A1              // Analog input pin for the (optional) potentiometer.
#define VCK_CHANNEL             A0              // Analog input pin to check the regulator output voltage (not used yet).
#define VALVE_PIN               PD6             // Valve to deflate the plug.
#define VIBRA_PIN               PD5             // Plug's built-in vibrator (PWM).
#define ESTIM_PIN               PD4             // User defined function, e.g. e-stim on/off.
#define APUMP_PIN               PD3             // Air pump to inflate the plug (PWM).

// Data acquisition parameters.
#define N_SAMPLES_PER_SECOND    50
#define N_SMOOTH                 4              // Oversampling factor to improve resolution and noise reduction.
#define ADC_SAMPLE_FREQ         (N_SAMPLES_PER_SECOND * N_SMOOTH)

// User parameters.
#define PRESSURE_LO_MIN         180.0
#define PRESSURE_LO_MAX         240.0
#define PRESSURE_HI_MIN         300.0
#define PRESSURE_HI_MAX         360.0
#define PRESSURE_STEP             1.2
#define CLENCH_THRESHOLD          2.0          // A higher number decreases the sensitivity.

static const char versionString[] = "# Bullfrog v0.51\n";

// This buffer gets filled by the timer interrupt routine and written to the serial
// port by the main loop, keeping the interrupt routine fast and predictable.
static RingBuffer outputBuffer;

// The actuators.
static AirPump pump(APUMP_PIN);
static SolenoidValve valve(VALVE_PIN);
static Vibrator vibrator(VIBRA_PIN);
static OutputPin clenchLED(LED_BUILTIN);
static OutputPin eStimSwitch(ESTIM_PIN);

// The inflatable toy.
static PressureLimits pressureLimits = {PRESSURE_LO_MIN, PRESSURE_LO_MAX, PRESSURE_HI_MIN, PRESSURE_HI_MAX};
static Inflatable buttPlug(&pressureLimits);

// The command interpreter.
static JsonParser jsonParser;

// The singleton state objects of the pump/valve/vib state machine.
static PS_Idle pumpStateIdle;
static PS_Pump pumpStatePump;
static PS_Warn pumpStateWarn;
static PS_Hold pumpStateHold;
static PumpState *pumpState = NULL;             // Pointer to the object representing the current state within the cycle.

static float basePressure = 0.0;                // Measured pressure when plug is fully deflated.


// Copies a C string to the output buffer for subsequent asynchronous transmission.
size_t postCString(const char *nts)
{
    return outputBuffer.write((const uint8_t *)nts, strlen(nts));
}

// Copies a C++ String's contents to the output buffer for subsequent asynchronous transmission.
static size_t postString(String& s)
{
    return outputBuffer.write((const uint8_t *)s.c_str(), s.length());
}

// Converts a float to decimal string representation, appends a separator and queues the result for sending.
// In case the buffer does not have enough room, only the separator will be output.
// TODO Improve / simplify this function: leave out unchanged values?
static size_t postSample(float fval, bool isLast)
{
    uint8_t sb[8];
    uint8_t sepi = sizeof sb - 1;
    dtostrf(fval, sepi, 2, (char*)sb);          // Result: dddd.dd\0
    while (sepi > 0 && sb[sepi - 1] == '0') sepi -= 1;
    if (sepi > 0 && sb[sepi - 1] == '.') sepi -= 1;
    sb[sepi] = (isLast ? '\n' : ';');           // Store the separator.
    uint8_t i = 0;
    while (sb[i] == ' ') i += 1;
    size_t nb = outputBuffer.write(sb + i, sepi + 1 - i);
    return (nb > 0 ? nb : outputBuffer.write(sb + sepi, 1));
}


void PS_Idle::enter()
{
    valve.open();                               // Start deflating.
    vibrator.start();                           // Provide a little entertainment.
}

PumpState *PS_Idle::nextState()
{
    if (buttPlug.pressureOverMin() <= 0.0) {    // Minimum pressure reached?
        valve.close();                          // Stop deflating.
    }
    if (waited(9000UL)) {                       // Milliseconds delay.
        vibrator.stop(1);                       // Stop the vibrator and bump its speed for the next round.
        return &pumpStatePump;
    }
    return this;                                // No state change.
}

/* TODO Implement safety functions:
 * - Switch off pump in case sensor does not indicate climbing pressure.
 * - Switch off pump once a certain maximum volume has been pumped.
 */
void PS_Pump::enter()
{
    valve.close();                              // Prepare to inflate.
    pump.start();
}

PumpState *PS_Pump::nextState()
{
    const float overshoot = buttPlug.pressureOverMax();
    if (overshoot >= 0.0) {                    // Maximum pressure reached?
        pump.stop();
        postString(String("# Holding at pressure ") + buttPlug.pressure() + '\n');
        if (buttPlug.cycleDone(PRESSURE_STEP)) {
            vibrator.resetSpeed();              // Start again at the lowest speed.
        }
        if (overshoot >= CLENCH_THRESHOLD) {    // You're in trouble now!
            postString(String("# {\"clench\":") + overshoot + "}\n");
            return &pumpStateWarn;
        }
        return &pumpStateHold;
    }
    pump.setFlowRate(buttPlug.flowRate());
    return this;                                // No state change.
}


void PS_Warn::enter()
{
    clenchLED.turnOn();
    eStimSwitch.turnOn();
}

PumpState *PS_Warn::nextState()
{
    if (waited(4000UL)) {                       // Milliseconds delay.
        eStimSwitch.turnOff();
        clenchLED.turnOff();
        return &pumpStatePump;                  // Resume inflating.
    }
    return this;                                // No state change.
}


PumpState *PS_Hold::nextState()
{
    if (waited(12000UL)) {                      // Milliseconds delay.
        return &pumpStateIdle;
    }
    // TODO Determine heart rate.
    return this;                                // No state change.
}


static void timerISR()
{
    static float sumPrs = 0;
    static unsigned int sampleNr = 0;

    sumPrs += (float)analogRead(PRS_CHANNEL);
    if (++sampleNr % N_SMOOTH == 0) {
        const float pressure = sumPrs / N_SMOOTH;
        buttPlug.updatePressure(pressure);
//      postSample(pressure, true);             // Output sample for drawing pressure graph.
        sumPrs = 0.0;
    }
    if (sampleNr == ADC_SAMPLE_FREQ) {
        sampleNr = 0;
        // This is executed once per second. Currently not used for anything.
    }
}


static void startReadingSensors(TimerOne& timer)
{
    timer.attachInterrupt(timerISR);            // Start sampling.
    timer.initialize(1000000 / ADC_SAMPLE_FREQ);
}


void setup()
{
    analogReference(DEFAULT);
    Serial.begin(9600);
    postCString(versionString);
    basePressure = (float)analogRead(PRS_CHANNEL);
    postString(String("# The pressure baseline value is ") + basePressure + '\n');
    pumpState = &pumpStateHold;                 // Enable the pump/vib/deflate cycle.
    startReadingSensors(Timer1);
    wdt_enable(WDTO_1S);                        // Set watchdog timer to 1 second.
}


void loop()
{
    // Handle serial output one byte at a time, without blocking, for minimal jitter on the sampling.
    if (Serial.availableForWrite() > 0) {
        noInterrupts();
        const int b = outputBuffer.getNextByte();
        interrupts();
        if (b >= 0) {
            Serial.write((byte)b);
        }
    }

    if (Serial.available() > 0) {               // Handle serial input one byte at a time, without blocking.
        const uint8_t b = (uint8_t)Serial.read();
        jsonParser.write(&b, 1);
    }

    if (pumpState != NULL) {
        static PumpState *previousState = &pumpStateHold;
        noInterrupts();                         // Prevent interrupts from messing with the state.
        if ((pumpState = pumpState->nextState()) != previousState) {
            previousState->leave();
            (previousState = pumpState)->enter();
        }
        interrupts();
        wdt_reset();                            // Keep our watchdog happy.
    }
}

