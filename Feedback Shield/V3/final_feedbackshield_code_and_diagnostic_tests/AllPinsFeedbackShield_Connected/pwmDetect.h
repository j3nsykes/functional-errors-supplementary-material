#include "Arduino.h"

/*
 * PWMListener
 *
 * Shows a real, filtered PWM signal as blue (with brightness equal to the
 * duty cycle), while staying dark for a floating or noisy pin.
 * uses a plain analogRead, no external library, no pull-up
 * probe and is non-blocking.
 *
 * How it tells a driven PWM pin from a floating one:
 *   These PWM lines are low-pass filtered, so a PWM wave arrives as a
 *   steady DC level (equal to the duty cycle) with a small amount of
 *   leftover ripple. Measured over a short window:
 *       a driven PWM signal gives a spread of roughly 60-120 counts
 *       (ripple the filter didn't fully remove)
 *       a floating pin gives a spread of roughly 3-5 counts (quiet)
 *   A modest spread threshold (around 30) cleanly separates the two cases.
 *   This is the same peak-to-peak idea used elsewhere in the project, just
 *   calibrated to the filtered ripple rather than an unfiltered, full-swing
 *   signal: a large enough spread means driven (show blue), and a tiny
 *   spread means floating (stay off).
 *
 * Brightness is a continuously smoothed level (an exponential moving
 * average), updated every loop so the fade looks smooth instead of
 * stepping once per window. For a filtered PWM signal this smoothed level
 * equals the duty-cycle DC level, so the blue brightness tracks the
 * analogWrite() value.
 *
 * A pin counts as "driven" in either of two cases:
 *   (a) the ripple over the window is at or above the threshold, meaning a
 *       switching PWM wave, or
 *   (b) the level is at or above HIGH_LEVEL (around 900), meaning a steady,
 *       near-full signal.
 * Case (b) matters because analogWrite(255) produces a steady HIGH with no
 * ripple at all. Without this check, full brightness would look like "not
 * driven" and the LED would go dark. A floating pin (around 50) doesn't
 * trigger either case. A duty cycle near 0 has no ripple and a low level,
 * so it correctly stays off, since near-0 duty is effectively off anyway.
 */
class PWMListener {
  private:
    int pin;

    // Rolling min/max over one time window, used only for the ripple check.
    int curMin = 1023;
    int curMax = 0;
    int lastPP = 0;
    unsigned long windowStart = 0;
    unsigned long windowMs = 100;

    // Continuously smoothed level for brightness (an exponential moving
    // average), updated every loop.
    float ema = 0;
    bool  emaStarted = false;
    static constexpr float EMA_ALPHA = 0.10;  // lower value = smoother but more lag

    int ppThreshold;                // ripple (in counts) needed to call the pin "driven"
    int highLevel = 900;            // steady level that also counts as "driven"

  public:
    // Default threshold of 30 sits comfortably above a floating reading
    // (around 4) and below a driven reading (around 60).
    PWMListener(int _pin, int _ppThreshold = 30) : pin(_pin), ppThreshold(_ppThreshold) {}

    void setup() {
      pinMode(pin, INPUT);
    }

    // Call this every loop for every PWM pin. Each call does one analogRead,
    // so it's cheap and non-blocking.
    void update() {
      int raw = analogRead(pin);

      // ripple window (for the gate)
      if (raw < curMin) curMin = raw;
      if (raw > curMax) curMax = raw;
      if (millis() - windowStart >= windowMs) {
        lastPP = curMax - curMin;
        curMin = 1023;
        curMax = 0;
        windowStart = millis();
      }

      // smoothed level (for brightness)
      if (!emaStarted) { ema = raw; emaStarted = true; }
      else             { ema += EMA_ALPHA * (raw - ema); }
    }

    // Counts as driven if it's switching (shows ripple) or sitting at a
    // near-full steady level.
    bool isDriven() const { return (lastPP >= ppThreshold) || ((int)ema >= highLevel); }

    // Spread of the last window. Watch this over Serial to help pick a threshold.
    int getPeakToPeak() const { return lastPP; }

    // Duty cycle from 0 to 255, derived from the smoothed level (the
    // filtered duty-cycle level).
    int getDuty() const { return map((int)ema, 0, 1023, 0, 255); }

    int getPin() const { return pin; }
    void setThreshold(int t) { ppThreshold = t; }
    void setHighLevel(int v) { highLevel = v; }
};
