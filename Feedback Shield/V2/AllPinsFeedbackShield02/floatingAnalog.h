class StableAnalogInput {
  private:
    const int input_pin;
    const int num_samples = 10;       // Number of samples to determine stability
    const int threshold = 5;         // Change threshold to consider as fluctuation
    int previousValue = 0;

  public:
    StableAnalogInput(int pin) : input_pin(pin) {}

    bool is_stable() {
      int fluctuationCount = 0;

      for(int i = 0; i < num_samples; i++) {
        int currentValue = analogRead(input_pin);
        
        // Check if the change from the previous value exceeds the threshold
        if(abs(currentValue - previousValue) > threshold) {
          fluctuationCount++;
        }
        
        previousValue = currentValue;
        delay(10);  // Small delay to get the next sample
      }

      // If more than half the readings fluctuated, it's considered unstable
      return fluctuationCount <= num_samples / 2;
    }

    int get_stable_reading() {
      if(is_stable()) {
        return analogRead(input_pin);
      } else {
        return -1; // Return a value indicating that the reading is unstable
      }
    }
};





