#include "Arduino.h"

class SignalDetector {
private:
  static SignalDetector* instance;  // Pointer to the class instance
  int signalPin;
  unsigned long lastRisingEdge;
  unsigned long lastFallingEdge;
  unsigned long highTime;
  unsigned long period;
  bool validSignal = false;
  
public:
  SignalDetector(int pin) : signalPin(pin) {
    pinMode(signalPin, INPUT_PULLUP);
    instance = this;
    attachInterrupt(digitalPinToInterrupt(signalPin), staticInterruptHandler, CHANGE);
  }

  void handleInterrupt() {
    if (digitalRead(signalPin) == HIGH) {
      lastRisingEdge = micros();
      period = lastRisingEdge - lastFallingEdge;
    } else {
      lastFallingEdge = micros();
      highTime = lastFallingEdge - lastRisingEdge;
    }
  }

  bool hasValidSignal() const {
    return validSignal;
  }

  // Get frequency in Hz
  float getFrequency() {
    if (period > 0) {
      return 1000000.0 / period;  // Convert microseconds to Hz
    }
    return 0;
  }

  // Get pulse width in microseconds
  unsigned long getPulseWidth() {
    return highTime;
  }

  // Get period in microseconds
  unsigned long getPeriod() {
    return period;
  }

  // Get duty cycle as percentage (0-100)
  float getDutyCycle() {
    if (period > 0) {
      return ((float)highTime / period) * 100.0;
    }
    return 0;
  }

  // Check if signal is a servo signal (50Hz, 1-2ms pulse)
  bool isServoSignal() {
    if (highTime && period) {
      validSignal = true;
      float freq = getFrequency();
      float pulse = getPulseWidth();
      
      // Servo characteristics:
      // Frequency: 50Hz (±5Hz tolerance)
      // Pulse width: 1000-2000μs (with some tolerance)
      if (freq > 45 && freq < 55 && pulse > 800 && pulse < 2200) {
        return true;
      }
    }
    return false;
  }

  // Check if signal is standard Arduino PWM (490Hz or 980Hz)
  bool isPWMSignal() {
    if (highTime && period) {
      validSignal = true;
      float freq = getFrequency();
      
      // Standard Arduino PWM frequencies:
      // Pins 5, 6: 980Hz (±50Hz tolerance)
      // Pins 3, 9, 10, 11: 490Hz (±50Hz tolerance)
      if ((freq > 450 && freq < 540) || (freq > 930 && freq < 1030)) {
        return true;
      }
    }
    return false;
  }

  // Check if signal is digital (constant HIGH or LOW)
  bool isDigitalSignal() {
    if (highTime && period) {
      validSignal = true;
      float dutyCycle = getDutyCycle();
      
      // Digital signals have duty cycles very close to 0% or 100%
      if (dutyCycle < 2 || dutyCycle > 98) {
        return true;
      }
    }
    return false;
  }

  // Get servo angle (0-180) from pulse width
  int getServoAngle() {
    if (isServoSignal()) {
      float pulse = getPulseWidth();
      // Standard servo: 1000μs = 0°, 2000μs = 180°
      return constrain(map(pulse, 1000, 2000, 0, 180), 0, 180);
    }
    return -1;  // Invalid
  }

  // Legacy method for backward compatibility
  bool checkSignalType() {
    if (highTime && period) {
      validSignal = true;
      float dutyCycle = getDutyCycle();

      // If duty cycle is in normal PWM range (not purely digital)
      if (dutyCycle > 5 && dutyCycle < 95) {
        return false;  // PWM
      } else {
        return true;   // Digital
      }
    }
    validSignal = false;
    return false;
  }

  // Get signal type as string for debugging
  const char* getSignalTypeString() {
    if (!validSignal) return "NONE";
    if (isServoSignal()) return "SERVO";
    if (isPWMSignal()) return "PWM";
    if (isDigitalSignal()) return "DIGITAL";
    return "UNKNOWN";
  }

  // Static interrupt handler
  static void staticInterruptHandler() {
    instance->handleInterrupt();
  }
};

// Define the static instance pointer
SignalDetector* SignalDetector::instance = nullptr;
