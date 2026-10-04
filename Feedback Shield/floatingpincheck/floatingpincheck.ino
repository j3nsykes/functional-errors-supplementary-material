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

// Define an array of StableAnalogInput objects for the pins you want to monitor
StableAnalogInput sensors[] = {
  StableAnalogInput(A0),
  StableAnalogInput(A1),
  StableAnalogInput(A2),
  StableAnalogInput(A3)
};

void setup() {
  Serial.begin(9600);
}

void loop() {
  for(int i = 0; i < sizeof(sensors)/sizeof(sensors[0]); i++) {
    int reading = sensors[i].get_stable_reading();

    if(reading != -1) {
      Serial.print("A");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(reading);
    } else {
      Serial.print("A");
      Serial.print(i);
      Serial.println(": Unstable reading detected!");
    }
  }
  delay(1000);  // Read every second
}
