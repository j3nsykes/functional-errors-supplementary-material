#include "Arduino.h"

/*
 * Listener - reads a digital pin and reports HIGH/LOW state changes.
 * Unchanged from the original sketch.
 *
 * The pin uses INPUT_PULLUP, so a floating digital pin reads HIGH. Because
 * the LED only updates on a state change, and the class starts out assuming
 * HIGH, a disconnected digital pin normally stays dark rather than glowing.
 */
class Listener {
    int pin;
    int pinState = 0;
    int lastPinState = 1;

  public:
    Listener(int _pin) {
      pin = _pin;
    }

    void setupPins() {
      pinMode(pin, INPUT_PULLUP);
    }

    int readPins() {
      pinState = digitalRead(pin);
      return pinState;
    }

    boolean stateChange() {
      if (pinState != lastPinState) {  // true only if the state changed since last time
        return true;
      }
      return false;
    }

    boolean pinHigh() {
      if (stateChange() && pinState == 1) {
        lastPinState = pinState;
        return true;
      } else {
        return false;
      }
    }

    boolean pinLow() {
      if (stateChange() && pinState == 0) {
        lastPinState = pinState;
        return true;
      } else {
        return false;
      }
    }
};  // end of class
