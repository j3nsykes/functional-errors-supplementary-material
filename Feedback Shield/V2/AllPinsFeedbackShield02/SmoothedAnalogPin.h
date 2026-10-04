class SmoothedAnalogPin {
  private:
    int pin;
    int lastStableValue;
    const int thresholdHigh;
    const int thresholdLow;

  public:
    SmoothedAnalogPin(int _pin, int _thresholdLow, int _thresholdHigh)
      : pin(_pin), thresholdLow(_thresholdLow), thresholdHigh(_thresholdHigh) {
      lastStableValue = analogRead(pin);
    }

    int getStableValue() {
      int currentValue = analogRead(pin);

      // If current value is significantly higher than the last stable value
      if (currentValue > lastStableValue + thresholdHigh) {
        lastStableValue = currentValue;
        return currentValue;
      }
      // If current value is significantly lower than the last stable value
      else if (currentValue < lastStableValue - thresholdLow) {
        lastStableValue = currentValue;
        return currentValue;
      }
      // If current value is within the "dead zone"
      else {
        return lastStableValue;
      }
    }
};

// Create SmoothedAnalogPin objects for pins A0, A1, A2, and A3
// SmoothedAnalogPin analogA0(A0, 2, 2); // Dead zone of 10 units (5 for up and 5 for down)
// SmoothedAnalogPin analogA1(A1, 8, 8);
// SmoothedAnalogPin analogA2(A2, 8, 8);
// SmoothedAnalogPin analogA3(A3, 8, 8);




