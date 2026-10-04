#include <ResponsiveAnalogRead.h>

/*
 * ResponsiveAnalogInput. a smoothed analog reading for a driven analog
 * source, such as a potentiometer. Unchanged from the original sketch.
 * Noise rejection for unconnected pins is handled separately by
 * PinConnection (connectedDetect.h); this class just smooths a real signal.
 */
class ResponsiveAnalogInput {
  private:
    ResponsiveAnalogRead* analogInput;
    int lastValue;

  public:
    ResponsiveAnalogInput(int pin, bool enableSleep = true, float snapMultiplier = 0.01) {
      analogInput = new ResponsiveAnalogRead(pin, enableSleep, snapMultiplier);
      analogInput->enableEdgeSnap();  // so 0 and 1023 are reachable
      lastValue = 0;
    }

    void update() {
      analogInput->update();
    }

    int getValue() {
      return analogInput->getValue();
    }

    int getRawValue() {
      return analogInput->getRawValue();
    }

    bool hasChanged() {
      return analogInput->hasChanged();
    }

    bool isSleeping() {
      return analogInput->isSleeping();
    }

    void setActivityThreshold(float threshold) {
      analogInput->setActivityThreshold(threshold);
    }

    // Mapped 0-1023 -> 0-255 for LED brightness (capped at 250).
    int getMappedValue() {
      int value = analogInput->getValue();
      return constrain(map(value, 0, 1023, 0, 255), 0, 250);
    }

    ~ResponsiveAnalogInput() {
      delete analogInput;
    }
};
