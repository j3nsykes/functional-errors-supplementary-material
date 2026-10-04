/*
 * DigitalPinScan_Diagnostic
 * =========================
 * A diagnostic for the digital listen pins (7, 10, 11, 12, 14, 15).
 * It reads each pin with no pull-up, many times over ~20 ms, and reports how
 * many reads came back HIGH. This tells us whether we can distinguish a driven
 * pin from a floating one well enough to make idle pins go dark (option 2).
 *
 * HOW TO USE:
 *   1. Flash code to the Micro (the shield).
 *   2. On the UNO/Leonardo below, run a sketch that drives a couple of pins to
 *      known states and leaves the rest unused, e.g.:
 *          void setup(){ pinMode(13,OUTPUT); digitalWrite(13,HIGH);
 *                        pinMode(8,OUTPUT);  digitalWrite(8,LOW); }
 *          void loop(){}
 *   3. Open Serial Monitor at 9600 and read the count for each pin.
 *
 * HOW TO READ IT (per pin, out of 200 samples):
 *   highs ~200  -> solidly DRIVEN HIGH   (should show green)
 *   highs ~0    -> solidly DRIVEN LOW    (should show red)
 *   highs mixed -> FLOATING, picking up noise (this is what lets us show OFF)
 *
 * If floating pins land in the "mixed" band and driven pins sit at the
 * extremes, the noise method will work. If floating pins sit stuck at ~0 or
 * ~200, it won't, and we'll need a different plan for those pins.
 *
 * Note: pins 10 and 12 are also ADC-capable, so even if the noise method is
 * shaky, those two have the reliable pull-up test as a fallback.
 */

const int NUM = 6;
int pins[NUM]          = {  7,   10,   11,   12,   14,   15 };
const char* names[NUM] = { "7", "10", "11", "12", "14", "15" };

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < NUM; i++) {
    pinMode(pins[i], INPUT);   // NO pull-up - we want to see floating noise
  }
  Serial.println();
  Serial.println("DigitalPinScan: highs out of 200 (no pull-up). ~200=HIGH, ~0=LOW, mixed=floating.");
}

void loop() {
  for (int i = 0; i < NUM; i++) {
    int highs = 0;
    const int N = 200;
    for (int s = 0; s < N; s++) {
      if (digitalRead(pins[i]) == HIGH) highs++;
      delayMicroseconds(100);          // spread ~20ms total to catch mains-hum noise
    }
    Serial.print("pin ");
    Serial.print(names[i]);
    Serial.print("\thighs=");
    Serial.print(highs);
    Serial.print("/200\t");
    if (highs > 190)      Serial.println("-> solid HIGH (driven?)");
    else if (highs < 10)  Serial.println("-> solid LOW (driven?)");
    else                  Serial.println("-> MIXED (floating?)");
  }
  Serial.println("--------");
  delay(500);
}
