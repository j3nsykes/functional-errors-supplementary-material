#include "listener.h"
#include "signalDetect.h"
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

FastLED_NeoPixel<NUM_LEDS, DATA_PIN, NEO_GRB> strip;  // <- FastLED NeoPixel version


// Define the instance pointer outside the class
SignalDetector* SignalDetector::instance = nullptr;

const int numDetectors = 12;
int detectorPins[numDetectors] = { 7, 10, 11, 12, 14, 15, A4, A5, A6, A7, A8, A9 };


SignalDetector* detectors[numDetectors];
bool isDigital[numDetectors];

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

PWMListener pwmPins[6]{
  PWMListener(A4),
  PWMListener(A5),
  PWMListener(A6),
  PWMListener(A7),
  PWMListener(A8),
  PWMListener(A9)

};
int neodigOutPins[6] = { 0, 2, 5, 6, 10, 11 };  //neopixel output nums
int pwmOutPins[6] = { 1, 3, 4, 7, 8, 9 };
bool pinHigh = false;
int totalPins = 6;  //total dig pins
int totalPWmPins = 6;

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < numDetectors; i++) {
    detectors[i] = new SignalDetector(detectorPins[i]);
  }

  for (int i = 0; i < totalPins; i++) {
    diginputPins[i].setupPins();
  }

  //setup listen to PWM pin state
  for (int i = 0; i < totalPWmPins; i++) {
    pwmPins[i].setupPWMPins();
  }
  strip.begin();
  strip.setBrightness(50);
  strip.show();  // Initialize all pixels to 'off'
}

void loop() {
  // Your code here
  //for (int i = 0; i < numDetectors; i++) {


    Serial.print("Pin ");
    Serial.print(detectorPins[5]);
    Serial.println(" has a valid signal.");
    p = pwmPins[5].readPWMpins();
    // Serial.println(p);
    neoDisplay(pwmOutPins[5], strip.Color(0, 0, p));  // blue fade

/*
    //is it digital or PWM signal
    isDigital[i] = detectors[i]->checkSignalType();

    //check if there is a signal
    if (detectors[i]->hasValidSignal()) {
      //if digital
      if (isDigital[i]) {

        diginputPins[5].readPins();

        if (diginputPins[5].stateChange()) {
          if (diginputPins[5].pinHigh()) {
            // Serial.print("Pin ");
            // Serial.print(i);
            // Serial.println(" went HIGH");
            neoDisplay(neodigOutPins[5], strip.Color(0, 255, 0));  // Green
          } else if (diginputPins[5].pinLow()) {
            // Serial.print("Pin ");
            // Serial.print(i);
            // Serial.println(" went LOW");
            neoDisplay(neodigOutPins[5], strip.Color(255, 0, 0));  // Red
          }
        }
        //  }
      } else if (!isDigital[i]) {
        //Serial.println("pwm display");
        //3 is pin 9
        //5 is pin 11
        p = pwmPins[5].readPWMpins();
        // Serial.println(p);
        neoDisplay(pwmOutPins[5], strip.Color(0, 0, p));  // blue fade
      }

      //else no signal
      else {
        // Serial.print("Pin ");
        // Serial.print(detectorPins[i]);
        // Serial.println(" has NO valid signal.");
      }
      delay(10);
    }
  }
  */
}




void neoDisplay(int i, uint32_t color) {
  strip.setPixelColor(i, color);
  strip.show();
  delay(BLINK_TIME);  // Consider removing this for a non-blocking approach
}