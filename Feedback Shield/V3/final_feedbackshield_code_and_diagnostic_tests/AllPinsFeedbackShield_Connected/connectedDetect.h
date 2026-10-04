#include "Arduino.h"

/*
 * PinConnection
 *
 * Answers if is anything actually
 * driving an analogue pin (a potentiometer, a PWM output, a digital output), or is
 * it left floating, with nothing connected so it just picks up electrical
 * noise?
 *
 * This matters because a floating pin gives wandering noise across the
 * whole 0-1023 range, and without this check that noise would get mapped
 * straight to a blue LED. This class lets the sketch keep the LED off
 * unless the pin is genuinely driven.
 *
 * It works using a pull-up differential test:
 *   1. Read the pin with the internal pull-up off.
 *   2. Switch the internal pull-up (about 30k ohms) on, let it settle, and
 *      read again.
 *   3. A floating, high-impedance pin gets pulled up toward 1023 by the
 *      pull-up, so the two readings differ a lot. A driven, low-impedance
 *      source easily overpowers the weak pull-up, so the two readings
 *      barely move.
 *   The difference (delta = reading with pull-up - reading without) tells
 *   us which case we're in: a big delta means floating (report not
 *   connected), a small delta means driven (report connected).
 *
 * on lines shared with the UNO/Leonardo underneath, this only
 * ever enables a weak internal pull-up (about 150 microamps). It never sets
 * the pin to OUTPUT, so it can never fight against whatever the board below
 * is driving. The brightness shown on the LED always comes from the normal
 * read path, not from the moment the pull-up is engaged, so this probe
 * doesn't affect what's displayed.
 */
class PinConnection {
  private:
    int pin;
    bool connected = false;
    int lastDelta = 0;

    // How many counts of upward shift under the pull-up count as "floating".
    // A higher value means only very high-impedance pins are treated as
    // floating, so more pins get reported as connected. A lower value is
    // more aggressive about rejecting pins as floating. A normal
    // potentiometer typically shifts by about 30-60 counts; a truly
    // floating pin shifts by 300 or more.
    int floatDelta;

    static const int SAMPLES = 4;  // average a few reads to reduce single-read noise

    int averageRead() {
      long sum = 0;
      for (int i = 0; i < SAMPLES; i++) {
        sum += analogRead(pin);
      }
      return (int)(sum / SAMPLES);
    }

  public:
    PinConnection(int _pin, int _floatDelta = 150)
      : pin(_pin), floatDelta(_floatDelta) {}

    // Runs one probe. This blocks for roughly 1-2ms (two reads plus a settle
    // delay), so it should be called for one pin per loop
    // rather than for every pin on every loop.
    void probe() {
      // 1. Take a baseline reading with the pull-up off.
      pinMode(pin, INPUT);
      delayMicroseconds(200);          // let the ADC sample-and-hold settle
      int noPull = averageRead();

      // 2. Turn the pull-up on and read again. A floating node gets pulled
      //    toward 5V quickly (it has little capacitance), while a driven
      //    node stays where it was.
      pinMode(pin, INPUT_PULLUP);
      delay(1);                        // let the pull-up settle (~1ms)
      int withPull = averageRead();

      // 3. Return to a neutral, high-impedance input so the normal read
      //    path doesn't see a bias left over from this probe.
      pinMode(pin, INPUT);

      lastDelta = withPull - noPull;
      connected = (lastDelta < floatDelta);
    }

    bool isConnected() const { return connected; }

    // Exposed for tuning: watch this over Serial to help pick a good floatDelta.
    int getDelta() const { return lastDelta; }
    int getPin() const { return pin; }

    void setFloatDelta(int d) { floatDelta = d; }
};
