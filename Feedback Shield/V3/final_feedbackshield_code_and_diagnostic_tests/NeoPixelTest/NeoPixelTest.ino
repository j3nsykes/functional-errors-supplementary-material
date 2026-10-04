/*
 * NeoPixelTest
 * ============
 * Quick test for the feedback shield's NeoPixels, run from the Micro.
 * Use it to check PCB routing: that every pixel lights, in order, and that
 * all three colour channels (R, G, B) work on every LED.
 *
 * Sequence (repeats forever):
 *   1. CHASE - lights one pixel at a time, index 0..NUM_LEDS-1, in white.
 *      Watch that they light one-by-one with no gaps or dead pixels, and that
 *      the physical order matches the index order you expect.
 *   2. ALL RED, ALL GREEN, ALL BLUE - every pixel each colour in turn.
 *      A pixel that skips a colour points to a routing/solder issue on that
 *      channel. (Also confirms GRB ordering: "red" really looks red, etc.)
 *   3. ALL WHITE - everything on together.
 */

#include <FastLED_NeoPixel.h>

#define DATA_PIN    5     // Micro pin driving the LED data line
#define NUM_LEDS    26    // pixels on the shield
#define BRIGHTNESS  40    // keep modest: 26 pixels at full white draws a lot of current

FastLED_NeoPixel<NUM_LEDS, DATA_PIN, NEO_GRB> strip;

void setup() {
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show();           // start with everything off
}

void loop() {
  // 1. Chase: one pixel at a time, in order.
  for (int i = 0; i < NUM_LEDS; i++) {
    clearAll();
    strip.setPixelColor(i, strip.Color(255, 255, 255));  // white
    strip.show();
    delay(120);
  }

  // 2. Each colour across all pixels.
  fillAll(strip.Color(255, 0, 0));  delay(700);  // Red
  fillAll(strip.Color(0, 255, 0));  delay(700);  // Green
  fillAll(strip.Color(0, 0, 255));  delay(700);  // Blue

  // 3. All white together.
  fillAll(strip.Color(255, 255, 255));  delay(1000);

  clearAll();
  strip.show();
  delay(400);
}

// Set every pixel to one colour and show it.
void fillAll(uint32_t color) {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

// Turn every pixel off (does not call show()).
void clearAll() {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
}
