#include "listener.h"
#include "signalDetect.h"
#include "PWMListener.h"
#include "floatingAnalog.h"
#include "floating02.h"
#include "SmoothedAnalogPin.h"
#include <FastLED_NeoPixel.h>

// Which pin on the Arduino is connected to the LEDs?
#define DATA_PIN 5

// How many LEDs are attached to the Arduino?
#define NUM_LEDS 26

// LED brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 150

// Amount of time for each half-blink, in milliseconds
#define BLINK_TIME 20

FastLED_NeoPixel<NUM_LEDS, DATA_PIN, NEO_GRB> strip;  // <- FastLED NeoPixel version


// Define the instance pointer outside the class
SignalDetector* SignalDetector::instance = nullptr;

//check signal on all pins.
const int numDetectors = 6;
int detectorDigPins[numDetectors] = { 7, 10, 11, 12, 14, 15 };
int detectorPWMPins[numDetectors] = { A4, A5, A6, A7, A8, A9 };


//int detectorPins[numDetectors] = { 7, 10, 11, 12, 14, 15, 22, 23, 4, 6, 8, 9 };
SignalDetector* detectorsDig[numDetectors];
SignalDetector* detectorsPWM[numDetectors];
bool isDigital[numDetectors];

//digital input pins
Listener diginputPins[6] = {
  Listener(7),
  Listener(10),
  Listener(11),
  Listener(12),
  Listener(14),
  Listener(15)
};
//PWM input pins
int p;
int a;

PWMListener pwmPins[6]{
  PWMListener(A4),
  PWMListener(A5),
  PWMListener(A6),
  PWMListener(A7),
  PWMListener(A8),
  PWMListener(A9)

};

//neopixel output pins
int neodigOutPins[6] = { 0, 2, 5, 6, 10, 11 };  //neopixel output nums
int pwmOutPins[6] = { 1, 3, 4, 7, 8, 9 };
int anagOutPins[6] = { 12, 13, 14, 15 };
bool pinHigh = false;
int totalDigPins = 6;  //total dig pins
int totalPWMPins = 6;
const int totalAPins = 4;
unsigned long lastBlinkTime = 0;

PinChecker* checkers[numDetectors];
//PinCheckerA* checkersA[totalAPins];

// Define an array of StableAnalogInput objects for the pins you want to monitor
StableAnalogInput sensors[] = {
  StableAnalogInput(A0),
  StableAnalogInput(A1),
  StableAnalogInput(A2),
  StableAnalogInput(A3)
};

const int NUM_PINS = 4;
SmoothedAnalogPin analogPins[NUM_PINS] = {
  SmoothedAnalogPin(A0, 5, 5),
  SmoothedAnalogPin(A1, 5, 5),
  SmoothedAnalogPin(A2, 5, 5),
  SmoothedAnalogPin(A3, 5, 5)
};

void setup() {
  Serial.begin(9600);
  //setup signal detect all pins
  for (int i = 0; i < numDetectors; i++) {
    detectorsDig[i] = new SignalDetector(detectorDigPins[i]);
    detectorsPWM[i] = new SignalDetector(detectorPWMPins[i]);
  }

  //setup listen to dig pin state
  for (int i = 0; i < totalDigPins; i++) {
    diginputPins[i].setupPins();
  }

  //setup listen to PWM pin state
  for (int i = 0; i < totalPWMPins; i++) {
    pwmPins[i].setupPWMPins();
    checkers[i] = new PinChecker(detectorPWMPins[i]);
  }
  //analogpins
  // for (int i = 0; i < totalAPins; i++) {
  //   anlgPins[i].setupPWMPins();
  //   //checkersA[i] = new PinCheckerA(detectorAnalogPins[i]);
  // }

  strip.begin();
  strip.setBrightness(50);
  strip.show();  // Initialize all pixels to 'off'
}

void loop() {
  digCheck();
  floatCheck();
  noiseCheck();

  delay(10);
}



void digCheck() {
  //is it digital or PWM signal
  for (int i = 0; i < totalDigPins; i++) {
    isDigital[i] = detectorsDig[i]->checkSignalType();

    //if digital
    diginputPins[i].readPins();

    if (diginputPins[i].stateChange()) {
      if (diginputPins[i].pinHigh()) {
        // Serial.print("Pin ");
        // Serial.print(i);
        // Serial.println(" went HIGH");
        neoDisplay(neodigOutPins[i], strip.Color(0, 255, 0));  // Green
      } else if (diginputPins[i].pinLow()) {
        // Serial.print("Pin ");
        // Serial.print(i);
        // Serial.println(" went LOW");
        neoDisplay(neodigOutPins[i], strip.Color(255, 0, 0));  // Red
      }
    }
  }
}

void floatCheck() {
  for (int i = 0; i < totalPWMPins; i++) {
    if (checkers[i]->isFloating()) {
      // Serial.print(pwmOutPins[i]);
      // Serial.println("Pin is floating!");
      p = pwmPins[i].readPWMpins();
      neoDisplay(pwmOutPins[i], strip.Color(0, 0, p));  // blue fade

    } else {
      // Serial.print(pwmOutPins[i]);
      // Serial.println("Pin is stable.");
      neoDisplay(pwmOutPins[i], strip.Color(0, 0, 0));  //off
    }
  }
}

void noiseCheck() {
  for (int i = 0; i < NUM_PINS; i++) {
    int value = analogPins[i].getStableValue();
    int mappedValue = constrain(map(value, 0, 1023, 0, 255), 0, 250);
    neoDisplay(anagOutPins[i], strip.Color(0, 0, mappedValue));
  }
  delay(30);
}



void neoDisplay(int i, uint32_t color) {
  strip.setPixelColor(i, color);
  strip.show();
  // if (millis() - lastBlinkTime > BLINK_TIME) {
  //   // switch off the LED or do some other task
  //   lastBlinkTime = millis();
  
  // }
  delay(BLINK_TIME);
  }