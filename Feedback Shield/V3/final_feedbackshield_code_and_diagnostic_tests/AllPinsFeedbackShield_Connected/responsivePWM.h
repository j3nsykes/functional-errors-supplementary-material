#include <ResponsiveAnalogRead.h>

/*
 * ResponsivePWMListener. reads a PWM pin and returns a smoothed duty-cycle
 * estimate from 0 to 255.
 *
 * How it reads the duty cycle: each analogRead samples the PWM waveform at
 * one instant, landing randomly in either the high or low phase. Averaged
 * over many reads, the smoothed value settles at a level proportional to the duty cycle, so
 * getDutyCycle() tracks the analogWrite() value.
 *
 * Whether the pin is actually driven, as opposed to floating, is decided
 * separately by PinConnection (connectedDetect.h), not by this class.
 */
class ResponsivePWMListener {
  private:
    ResponsiveAnalogRead* analogInput;
    int pin;

  public:
    ResponsivePWMListener(int _pin) : pin(_pin) {
      analogInput = new ResponsiveAnalogRead(_pin, true, 0.05);
      analogInput->setActivityThreshold(3.0);
      analogInput->enableEdgeSnap();
    }

    void setupPWMPins() {
      pinMode(pin, INPUT);
    }

    void update() {
      analogInput->update();
    }

    int getDutyCycle() {
      int value = analogInput->getValue();
      return map(value, 0, 1023, 0, 255);
    }

    int getValue() {
      return analogInput->getValue();
    }

    bool hasChanged() {
      return analogInput->hasChanged();
    }

    int getRawValue() {
      return analogInput->getRawValue();
    }

    bool isSleeping() {
      return analogInput->isSleeping();
    }

    ~ResponsivePWMListener() {
      delete analogInput;
    }
};
