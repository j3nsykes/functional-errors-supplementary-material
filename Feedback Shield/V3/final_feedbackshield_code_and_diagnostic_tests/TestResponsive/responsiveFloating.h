#include <ResponsiveAnalogRead.h>

class ResponsiveFloatingDetector {
  private:
    ResponsiveAnalogRead* analogInput;
    int pin;
    const float FLOATING_ACTIVITY_THRESHOLD = 8.0;  // Higher = more noise needed to detect floating
    
  public:
    ResponsiveFloatingDetector(int _pin) : pin(_pin) {
      // Disable sleep for floating detection - we want to catch all movement
      // Use lower snap multiplier for more sensitivity to noise
      analogInput = new ResponsiveAnalogRead(_pin, false, 0.001);
      
      // Set activity threshold specific to floating detection
      analogInput->setActivityThreshold(FLOATING_ACTIVITY_THRESHOLD);
    }

    // Update must be called every loop
    void update() {
      analogInput->update();
    }

    // Check if pin is floating based on the algorithm's sleep state
    // A pin that's truly floating will never sleep (constant noise)
    // A stable signal will sleep quickly
    bool isFloating() {
      // If the pin is NOT sleeping, it means there's constant activity = floating
      // We invert the logic: !isSleeping() means floating
      return !analogInput->isSleeping();
    }

    // Alternative: check if there's significant variation
    bool hasActivity() {
      return analogInput->hasChanged();
    }

    // Get current reading
    int getValue() {
      return analogInput->getValue();
    }

    // Get raw reading to see actual noise
    int getRawValue() {
      return analogInput->getRawValue();
    }

    // Configure sensitivity to floating detection
    void setFloatingThreshold(float threshold) {
      analogInput->setActivityThreshold(threshold);
    }

    ~ResponsiveFloatingDetector() {
      delete analogInput;
    }
};
