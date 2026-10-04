#include <ResponsiveAnalogRead.h>

class ResponsiveAnalogInput {
  private:
    ResponsiveAnalogRead* analogInput;
    int lastValue;
    
  public:
    // Constructor with configurable sleep and snap settings
    ResponsiveAnalogInput(int pin, bool enableSleep = true, float snapMultiplier = 0.01) {
      analogInput = new ResponsiveAnalogRead(pin, enableSleep, snapMultiplier);
      // Enable edge snap to ensure 0 and 1023 are reachable
      analogInput->enableEdgeSnap();
      lastValue = 0;
    }

    // Update must be called every loop
    void update() {
      analogInput->update();
    }

    // Get the smoothed value
    int getValue() {
      return analogInput->getValue();
    }

    // Get the raw unprocessed value
    int getRawValue() {
      return analogInput->getRawValue();
    }

    // Check if value has changed since last update
    bool hasChanged() {
      return analogInput->hasChanged();
    }

    // Check if the algorithm is in sleep mode (not detecting changes)
    bool isSleeping() {
      return analogInput->isSleeping();
    }

    // Configure activity threshold (how much movement to register as activity)
    void setActivityThreshold(float threshold) {
      analogInput->setActivityThreshold(threshold);
    }

    // Get mapped value for LED brightness (0-255)
    int getMappedValue() {
      int value = analogInput->getValue();
      return constrain(map(value, 0, 1023, 0, 255), 0, 250);
    }

    ~ResponsiveAnalogInput() {
      delete analogInput;
    }
};
