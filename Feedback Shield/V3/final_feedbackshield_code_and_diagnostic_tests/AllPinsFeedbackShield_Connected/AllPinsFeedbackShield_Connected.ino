/*
 * AllPinsFeedbackShield_Connected
 *
 * A visual debugging shield. An Arduino Micro on a custom shield passively
 * listens to every pin of the UNO/Leonardo underneath it and mirrors each
 * pin's state to a NeoPixel LED:
 *   - Digital HIGH shows green, digital LOW shows red.
 *   - PWM and analog signals show as a fading blue.
 *
 * PWM and analog pins use different detection methods, because the signals
 * look different electrically:
 *   - PWM pins (A4-A9) are low-pass filtered, so a PWM wave arrives as a
 *     steady DC level (equal to the duty cycle) with a small amount of
 *     ripple left over. PWMListener displays that level as blue, and uses
 *     the amount of ripple (the peak-to-peak spread) to tell a driven pin
 *     (ripple around 60-120) from a floating one (ripple around 4). It
 *     doesn't use a pull-up probe, because that would pump charge into the
 *     filter capacitor and make floating pins glow.
 *   - Analog pins (A0-A3) float higher and noisier than the PWM pins, so
 *     these use PinConnection's pull-up test instead, which checks whether
 *     the pin is driven or floating and works well for this kind of signal.
 *   - The digital listener pins are unchanged from the original sketch:
 *     INPUT_PULLUP, green for HIGH, red for LOW.
 *
 * neoDisplay() only sets a pixel's colour in memory; the strip is pushed to
 * the LEDs once per loop, so the whole display update is non-blocking.
 */

#include "listener.h"
#include "responsiveAnalog.h"
#include "connectedDetect.h"
#include "pwmDetect.h"
#include <FastLED_NeoPixel.h>
#include <ResponsiveAnalogRead.h>

// Set to 1 to print detection info over Serial, useful when tuning thresholds.
#define DEBUG_DETECT 0

// Which pin on the Micro drives the LED data line.
#define DATA_PIN 5

// How many LEDs are on the shield.
#define NUM_LEDS 26

// LED brightness, from 0 (off) to 255 (max).
#define BRIGHTNESS 50

// Ripple cutoff used to decide whether a PWM pin is driven, in ADC counts of
// peak-to-peak spread. A driven, filtered PWM signal shows roughly 60-120;
// a floating pin shows roughly 4. Raise this if floating pins still show
// blue, or lower it if real PWM signals are being missed. getPeakToPeak()
// can be watched over Serial (with DEBUG_DETECT on) to help choose a value.
#define PWM_SPREAD_THRESHOLD 30

// Cutoff used by the analog pull-up test to decide whether a pin is
// floating (see connectedDetect.h for how this is used).
#define FLOAT_DELTA 150

FastLED_NeoPixel<NUM_LEDS, DATA_PIN, NEO_GRB> strip;

// Input pin map: which Micro pins are wired to the board underneath.
const int totalDigPins = 6;
const int totalPWMPins = 6;
const int totalAPins   = 4;

int detectorPWMPins[totalPWMPins] = { A4, A5, A6, A7, A8, A9 };

// Digital input pins, shown as green (HIGH) or red (LOW).
Listener diginputPins[totalDigPins] = {
  Listener(7),
  Listener(10),
  Listener(11),
  Listener(12),
  Listener(14),
  Listener(15)
};

// PWM input pins, shown as blue (brightness = duty cycle), gated by the
// ripple check in PWMListener. These don't use the pull-up probe.
PWMListener* pwmPins[totalPWMPins];

// Analog input pins A0-A3, shown as a blue fade, gated by the pull-up
// connection test.
ResponsiveAnalogInput* analogPins[totalAPins];
PinConnection* analogConn[totalAPins];

// NeoPixel output indices on the strip, one per input pin above.
int neodigOutPins[totalDigPins] = { 0, 2, 5, 6, 10, 11 };
int pwmOutPins[totalPWMPins]    = { 1, 3, 4, 7, 8, 9 };
int anagOutPins[totalAPins]     = { 12, 13, 14, 15 };

// The analog pull-up probe takes a little time to settle, so only one
// analog pin is probed per loop, rather than all of them every
// loop. PWM pins are read every loop since that read is not blocking.
int analogProbeIndex = 0;

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < totalDigPins; i++) {
    diginputPins[i].setupPins();
  }

  for (int i = 0; i < totalPWMPins; i++) {
    pwmPins[i] = new PWMListener(detectorPWMPins[i], PWM_SPREAD_THRESHOLD);
    pwmPins[i]->setup();
  }

  for (int i = 0; i < totalAPins; i++) {
    analogPins[i] = new ResponsiveAnalogInput(A0 + i, true, 0.01);
    analogPins[i]->setActivityThreshold(4.0);
    analogConn[i] = new PinConnection(A0 + i, FLOAT_DELTA);
  }

  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show();

  Serial.println("Feedback Shield Initialized (PWM ripple-gate + analog pull-up gate)");
}

void loop() {
  // 1. Read every PWM pin this loop so detection stays real-time.
  for (int i = 0; i < totalPWMPins; i++) pwmPins[i]->update();

  // 2. Smooth the analog reads.
  for (int i = 0; i < totalAPins; i++) analogPins[i]->update();

  // 3. Probe one analog pin's connection state this loop.
  analogConn[analogProbeIndex]->probe();
#if DEBUG_DETECT
  debugPrint();
#endif
  analogProbeIndex = (analogProbeIndex + 1) % totalAPins;

  // 4. Decide each LED's colour. This only sets pixel values in memory;
  // it doesn't call show() or delay().
  digCheck();
  pwmCheck();
  analogCheck();

  // 5. Push the whole strip to the LEDs once.
  strip.show();

  delay(5);
}

// Digital side: the LED only updates when the pin's state changes, not on
// every loop.
//
// A floating or unused digital pin reads a steady HIGH here, because these
// lines have an effective pull-up. If the LED updated every loop regardless
// of change, every idle pin would light up green, which would be confusing
// on a teaching shield. A floating HIGH and a genuinely driven HIGH look
// electrically identical, so there's no way to tell them apart from a
// single reading only from whether the pin ever changes.
//
// Updating only on a change avoids this: a pin sitting steadily at a
// floating HIGH never changes, so it never lights and stays dark. Real
// HIGH/LOW transitions still show green/red as expected. Resetting the
// Micro clears any colour left over from before, e.g. after reflashing the
// board underneath.
void digCheck() {
  for (int i = 0; i < totalDigPins; i++) {
    diginputPins[i].readPins();

    if (diginputPins[i].stateChange()) {
      if (diginputPins[i].pinHigh()) {
        neoDisplay(neodigOutPins[i], strip.Color(0, 255, 0));  // Green
      } else if (diginputPins[i].pinLow()) {
        neoDisplay(neodigOutPins[i], strip.Color(255, 0, 0));  // Red
      }
    }
  }
}

// PWM side: shows blue, brightness equal to duty cycle, only when the pin
// shows real PWM ripple.
void pwmCheck() {
  for (int i = 0; i < totalPWMPins; i++) {
    if (pwmPins[i]->isDriven()) {
      neoDisplay(pwmOutPins[i], strip.Color(0, 0, pwmPins[i]->getDuty()));
    } else {
      neoDisplay(pwmOutPins[i], strip.Color(0, 0, 0));  // Off - floating / no signal
    }
  }
}

// Analog side: shows a blue fade only when the pin is actually driven.
void analogCheck() {
  for (int i = 0; i < totalAPins; i++) {
    if (analogConn[i]->isConnected()) {
      neoDisplay(anagOutPins[i], strip.Color(0, 0, analogPins[i]->getMappedValue()));
    } else {
      neoDisplay(anagOutPins[i], strip.Color(0, 0, 0));  // Off - floating
    }
  }
}

// Sets a single pixel's colour. Doesn't call show() or delay() - the whole
// strip is pushed to the LEDs once per loop, in loop().
void neoDisplay(int i, uint32_t color) {
  strip.setPixelColor(i, color);
}

#if DEBUG_DETECT
void debugPrint() {
  for (int i = 0; i < totalPWMPins; i++) {
    Serial.print("PWM"); Serial.print(i);
    Serial.print(" pp="); Serial.print(pwmPins[i]->getPeakToPeak());
    Serial.print(" duty="); Serial.print(pwmPins[i]->getDuty());
    Serial.print(pwmPins[i]->isDriven() ? "* " : "  ");
  }
  PinConnection* p = analogConn[analogProbeIndex];
  Serial.print("| A"); Serial.print(p->getPin() - A0);
  Serial.print(" delta="); Serial.print(p->getDelta());
  Serial.println(p->isConnected() ? " DRIVEN" : " floating");
}
#endif
