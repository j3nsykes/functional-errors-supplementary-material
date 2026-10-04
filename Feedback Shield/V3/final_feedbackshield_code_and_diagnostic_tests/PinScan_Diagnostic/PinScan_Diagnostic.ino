/*
 * PinScan_Diagnostic
 * ==================
 * A diagnostic pin scan. It reads analogRead() on every listen pin (A0-A9)
 * and prints the min, max, spread, and mean over a short burst. This bypasses
 * ALL of the shield code so you can see exactly what signal is arriving on each
 * pin, with nothing in the way.
 *
 * HOW TO USE:
 *   1. Flash this to the Micro (the shield).
 *   2. Put the base UNO/Leonardo below running your PWM test (e.g. the Fade
 *      sketch on pin 9, or analogWrite on pin 11).
 *   3. Open Serial Monitor at 9600 and read the row for each pin.
 *
 * HOW TO READ IT (per pin):
 *   spread ~800-1023 (mean anywhere) -> a real, UNFILTERED switching signal
 *                                        (PWM square wave). Amplitude detection
 *                                        WILL work here.
 *   spread small, mean sitting mid    -> steady DC. Either a FILTERED PWM (the
 *      and moving with your fade          low-pass filter smoothed it) or an
 *                                        analog voltage. Amplitude detection
 *                                        CANNOT work here - looks like analog.
 *   spread small, mean near 0 or       -> nothing is driving this pin
 *      drifting around                    (floating / not wired to your signal).
 *
 * What we're trying to learn:
 *   - WHICH Ax channel your UNO pin 9 / 11 actually lands on.
 *   - Whether that channel sees a SWINGING wave (unfiltered) or FLAT DC
 *     (filtered). That single fact decides how PWM must be detected.
 */

const int NUM = 10;
int pins[NUM]         = {  A0,   A1,   A2,   A3,   A4,   A5,   A6,   A7,   A8,   A9 };
const char* names[NUM] = { "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9" };

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < NUM; i++) {
    pinMode(pins[i], INPUT);
  }
  Serial.println();
  Serial.println("PinScan running. spread ~1000 = switching wave, small spread = DC/quiet.");
}

void loop() {
  for (int i = 0; i < NUM; i++) {
    int mn = 1023;
    int mx = 0;
    long sum = 0;
    const int N = 200;              // ~20ms of sampling - spans many PWM cycles

    for (int s = 0; s < N; s++) {
      int v = analogRead(pins[i]);
      if (v < mn) mn = v;
      if (v > mx) mx = v;
      sum += v;
    }

    Serial.print(names[i]);
    Serial.print("  min=");    Serial.print(mn);
    Serial.print("\tmax=");    Serial.print(mx);
    Serial.print("\tspread="); Serial.print(mx - mn);
    Serial.print("\tmean=");   Serial.println(sum / N);
  }
  Serial.println("--------");
  delay(500);
}
