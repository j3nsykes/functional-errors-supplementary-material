#include <ResponsiveAnalogRead.h>

class ResponsivePWMListener {
  private:
    ResponsiveAnalogRead* analogInput;
    int pin;

  public:
    // Constructor. more responsive settings for PWM (less sleep, more snap)
    ResponsivePWMListener(int _pin) : pin(_pin) {
      // Use more responsive settings for PWM:
      // Sleep enabled to reduce jitter when stable
      // Higher snap multiplier (0.05) for faster response
      analogInput = new ResponsiveAnalogRead(_pin, true, 0.05);

      // Set lower activity threshold for more sensitivity to PWM changes
      analogInput->setActivityThreshold(3.0);

      // Enable edge snap for clean 0 and 255 duty cycle readings
      analogInput->enableEdgeSnap();
    }

    void setupPWMPins() {
      pinMode(pin, INPUT);
    }

    // Update must be called every loop
    void update() {
      analogInput->update();
    }

    // Get duty cycle value (0-255)
    int getDutyCycle() {
      int value = analogInput->getValue();
      return map(value, 0, 1023, 0, 255);
    }

    // Get the smoothed 10-bit value
    int getValue() {
      return analogInput->getValue();
    }

    // Check if value has changed
    bool hasChanged() {
      return analogInput->hasChanged();
    }

    // Get raw unfiltered reading
    int getRawValue() {
      return analogInput->getRawValue();
    }

    // Check if sleeping (stable signal)
    bool isSleeping() {
      return analogInput->isSleeping();
    }

    ~ResponsivePWMListener() {
      delete analogInput;
    }
};
