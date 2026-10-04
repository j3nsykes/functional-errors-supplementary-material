#include "Listener.h"
#include "PWMListener.h"
#include <FastLED_NeoPixel.h>

// Which pin on the Arduino is connected to the LEDs?
#define DATA_PIN 5

// How many LEDs are attached to the Arduino?
#define NUM_LEDS 17

// LED brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 50

// Amount of time for each half-blink, in milliseconds
#define BLINK_TIME 30

FastLED_NeoPixel<NUM_LEDS, DATA_PIN, NEO_GRB> strip;      // <- FastLED NeoPixel version

int totalPins = 6;
int totalPWmPins = 5;
int totalAPins = 4;
int r = 0; //test a reading
long p;

Listener inputPins[6] = {
  Listener(7),
  Listener(10),
  Listener(11),
  Listener(12),
  Listener(14),
  Listener(15)
};


PWMListener pwmPins[6] {
  PWMListener(A4),
  PWMListener(A5),
  PWMListener(A6),
  PWMListener(A7),
  PWMListener(A8),
  PWMListener(A9)

};

//analogListener analogPins[4] {
//analogListener(A0),
//analogListener(A1),
//analogListener(A2),
//analogListener(A3)
//};

int digOutPins[6] = {0, 2, 5, 6, 10, 11};
int pwmOutPins[6] = {1, 3, 4, 7, 8, 9};
int analogOutPins[4] = {12, 13, 14, 15};

void setup() {
  // put your setup code here, to run once:
  for (int i = 0; i < totalPins; i++) {
    inputPins[i].setupPins();
  }

  for (int i = 0; i < totalPWmPins; i++) {
    pwmPins[i].setupPWMPins();
  }

  Serial.begin(9600);
  strip.begin();
  strip.setBrightness(50);
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  // put your main code here, to run repeatedly:
  updateCheck();
  for (int i = 0; i < totalPins; i++) {

    if (inputPins[i].pinHigh()) {
      //  Serial.println("PIN i ON");
      neoDisplayOn(digOutPins[i]);
    }
    else if (inputPins[i].pinLow()) {
      //  Serial.println("PIN i OFF");
      neoDisplayOff(digOutPins[i]);
    }
  }
}

//need to put in  draw loop and constantly check  these states.
void updateCheck() {
  for (int i = 0; i < totalPins; i++) {
    inputPins[i].readPins();
    inputPins[i].stateChange();
  }

  //check an individual pin
  //int t = inputPins[0].readPins();
  //Serial.println(t);
}

void updateCheckPWM() {
  for (int i = 0; i < 5; i++) {
    //pwmPins[i].readPWMpins();

    //check an individual pin
    p = pwmPins[1].readPWMpins();

    Serial.println(p);
    neoDisplayPWM(pwmOutPins[i], p);
  }
}

void neoDisplayPWM(int _i, int _p) {
  int i = _i;
  int p = _p;
  //strip.setBrightness(p);
  strip.setPixelColor(i, strip.Color(0, 0, p));  // set blue brightness
  strip.show();
  delay(10);
}

void neoDisplayOn(int _i) {
  int i = _i;
  strip.setPixelColor(i, strip.Color(0, 255, 0));  // set pixel 1 to Green
  strip.show();
  delay(BLINK_TIME);

}

void neoDisplayOff(int _i) {
  int i = _i;
  strip.setPixelColor(i, strip.Color(255, 0, 0));  // set pixel 1 to red
  strip.show();
  delay(BLINK_TIME);

}
