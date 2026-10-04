class SignalDetector {
private:
  static SignalDetector* instance;  // Declare a pointer to the class instance
  int signalPin;
  unsigned long lastRisingEdge;
  unsigned long lastFallingEdge;
  unsigned long highTime;
  unsigned long period;
  bool validSignal = false;  // flag to track if the signal is valid

public:
  SignalDetector(int pin)
    : signalPin(pin) {
    pinMode(signalPin, INPUT);
    // Assign the instance pointer to 'this' object
    instance = this;
    // Attach the static interrupt handler
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
    // You can also calculate duty cycle and other stuff here
  }

  bool hasValidSignal() const {
    return validSignal;
  }

  // Check the signal type based on highTime and period
  bool checkSignalType() {
    if (highTime && period) {
      validSignal = true;
      float dutyCycle = (float)highTime / period * 100.0;

      // Serial.print("High time: ");
      // Serial.println(highTime);
      // Serial.print("Period: ");
      // Serial.println(period);

      Serial.print("Duty Cycle: ");
      Serial.print(dutyCycle);
      Serial.println("%");

      if (dutyCycle > 10 && dutyCycle < 99) {
        Serial.println("Signal is likely PWM");
        return false;
      } else {
        Serial.println("Signal is likely Digital");
        return true;
      }

      // Reset
      highTime = 0;
      period = 0;
    }
    validSignal = false; //no signal 
  }

  // Static function to handle the interrupt
  static void staticInterruptHandler() {
    // Call the instance-specific interrupt handler
    instance->handleInterrupt();
  }
};