#include "listener.h"
#include "signalDetect.h"
#include "responsivePWM.h"
#include "responsiveAnalog.h"
#include "responsiveFloating.h"
#include <FastLED_NeoPixel.h>
#include <ResponsiveAnalogRead.h>

// Which pin on the Arduino is connected to the LEDs?
#define DATA_PIN 5

// How many LEDs are attached to the Arduino?
#define NUM_LEDS 26

// LED brightness, 0 (min) to 255 (max)
#define BRIGHTNESS 150

// Amount of time for each half-blink, in milliseconds
#define BLINK_TIME 20

FastLED_NeoPixel<NUM_LEDS, DATA_PIN, NEO_GRB> strip;

// Define the instance pointer outside the class
SignalDetector* SignalDetector::instance = nullptr;

// Check signal on all pins
const int numDetectors = 6;
int detectorDigPins[numDetectors] = { 7, 10, 11, 12, 14, 15 };
int detectorPWMPins[numDetectors] = { A4, A5, A6, A7, A8, A9 };

SignalDetector* detectorsDig[numDetectors];
SignalDetector* detectorsPWM[numDetectors];
bool isDigital[numDetectors];

// Digital input pins
Listener diginputPins[6] = {
  Listener(7),
  Listener(10),
  Listener(11),
  Listener(12),
  Listener(14),
  Listener(15)
};

// PWM input pins - now using ResponsivePWMListener
ResponsivePWMListener* pwmPins[6];

// Floating pin detectors - now using ResponsiveFloatingDetector
ResponsiveFloatingDetector* floatingDetectors[6];

// Analog input pins - now using ResponsiveAnalogInput
ResponsiveAnalogInput* analogPins[4];

// Neopixel output pins
int neodigOutPins[6] = { 0, 2, 5, 6, 10, 11 };
int pwmOutPins[6] = { 1, 3, 4, 7, 8, 9 };
int anagOutPins[4] = { 12, 13, 14, 15 };

bool pinHigh = false;
int totalDigPins = 6;
int totalPWMPins = 6;
const int totalAPins = 4;
unsigned long lastBlinkTime = 0;

void setup() {
  Serial.begin(9600);
  
  // Setup signal detect all pins
  for (int i = 0; i < numDetectors; i++) {
    detectorsDig[i] = new SignalDetector(detectorDigPins[i]);
    detectorsPWM[i] = new SignalDetector(detectorPWMPins[i]);
  }

  // Setup listen to dig pin state
  for (int i = 0; i < totalDigPins; i++) {
    diginputPins[i].setupPins();
  }

  // Setup PWM pins with ResponsivePWMListener
  for (int i = 0; i < totalPWMPins; i++) {
    pwmPins[i] = new ResponsivePWMListener(detectorPWMPins[i]);
    pwmPins[i]->setupPWMPins();
    
    // Setup floating detectors for same pins
    floatingDetectors[i] = new ResponsiveFloatingDetector(detectorPWMPins[i]);
  }

  // Setup analog pins with ResponsiveAnalogInput
  // Parameters: (pin, enableSleep, snapMultiplier)
  // enableSleep=true: values stop changing quickly when stable
  // snapMultiplier=0.01: amount of smoothing (lower = more smooth)
  for (int i = 0; i < totalAPins; i++) {
    analogPins[i] = new ResponsiveAnalogInput(A0 + i, true, 0.01);
    // Set activity threshold for analog pins (lower = more sensitive)
    analogPins[i]->setActivityThreshold(4.0);
  }

  strip.begin();
  strip.setBrightness(50);
  strip.show();
  
  Serial.println("Feedback Shield Initialized with ResponsiveAnalogRead");
}

void loop() {
  // Update all responsive analog objects first
  updateResponsiveInputs();
  
  // Then check states
  digCheck();
  floatCheck();
  analogCheck();

  delay(10);
}

// Update all ResponsiveAnalogRead objects every loop
void updateResponsiveInputs() {
  // Update PWM listeners
  for (int i = 0; i < totalPWMPins; i++) {
    pwmPins[i]->update();
    floatingDetectors[i]->update();
  }
  
  // Update analog inputs
  for (int i = 0; i < totalAPins; i++) {
    analogPins[i]->update();
  }
}

void digCheck() {
  // Check if digital or PWM signal
  for (int i = 0; i < totalDigPins; i++) {
    isDigital[i] = detectorsDig[i]->checkSignalType();

    // Read digital state
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

void floatCheck() {
  for (int i = 0; i < totalPWMPins; i++) {
    if (floatingDetectors[i]->isFloating()) {
      // Pin is floating - show PWM value
      int duty = pwmPins[i]->getDutyCycle();
      neoDisplay(pwmOutPins[i], strip.Color(0, 0, duty));  // Blue fade
    } else {
      // Pin is stable - off
      neoDisplay(pwmOutPins[i], strip.Color(0, 0, 0));  // Off
    }
  }
}

void analogCheck() {
  for (int i = 0; i < totalAPins; i++) {
    // Only update LED if value has changed (reduces unnecessary updates)
    if (analogPins[i]->hasChanged()) {
      int mappedValue = analogPins[i]->getMappedValue();
      neoDisplay(anagOutPins[i], strip.Color(0, 0, mappedValue));
    }
  }
}

void neoDisplay(int i, uint32_t color) {
  strip.setPixelColor(i, color);
  strip.show();
  delay(BLINK_TIME);
}
