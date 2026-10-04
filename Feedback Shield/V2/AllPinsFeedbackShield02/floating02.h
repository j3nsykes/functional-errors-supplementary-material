class PinChecker {
private:
    const int NUM_SAMPLES = 5;        // Number of samples
    const int THRESHOLD = 10;         // Threshold to consider a pin as floating
    const int SAMPLE_DELAY = 10;      // Delay between samples (ms)
    int pin;

public:
    PinChecker(int _pin) : pin(_pin) {}

    bool isFloating() {
        int readings[NUM_SAMPLES];  // Array to store the analog readings
        
        // Take multiple samples
        for (int i = 0; i < NUM_SAMPLES; i++) {
            readings[i] = analogRead(pin);
            delay(SAMPLE_DELAY);
        }

        int maxReading = readings[0];
        int minReading = readings[0];

        // Find the range (max - min)
        for (int i = 1; i < NUM_SAMPLES; i++) {
            if (readings[i] > maxReading) maxReading = readings[i];
            if (readings[i] < minReading) minReading = readings[i];
        }

        // If the difference between max and min readings is above a threshold, it might be floating
        return (maxReading - minReading) > THRESHOLD;
    }
};



