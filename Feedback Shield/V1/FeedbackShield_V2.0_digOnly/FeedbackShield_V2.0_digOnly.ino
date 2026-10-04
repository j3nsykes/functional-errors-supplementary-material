#include "listener.h"
#include "signalDetect.h"
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

const int numDetectors = 6;
int detectorPins[numDetectors] = { 7, 10, 11, 12, 14, 15 };

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

int neodigOutPins[6] = { 0, 2, 5, 6, 10, 11 };  //neopixel output nums
bool pinHigh = false;
int totalPins = 6;  //total dig pins

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < numDetectors; i++) {
    detectors[i] = new SignalDetector(detectorPins[i]);
  }

  for (int i = 0; i < totalPins; i++) {
    diginputPins[i].setupPins();
  }

  strip.begin();
  strip.setBrightness(50);
  strip.show();  // Initialize all pixels to 'off'
}

void loop() {
  // Your code here
  //for (int i = 0; i < totalPins; i++) {

      
      Serial.print("Pin ");
      Serial.print(detectorPins[5]);
      Serial.println(" has a valid signal.");

      //is it digital or PWM signal
      isDigital[5] = detectors[5]->checkSignalType();
      
      //check if there is a signal 
      if (detectors[5]->hasValidSignal()) {
      //if digital
      if (isDigital[5]) {

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




  void neoDisplay(int i, uint32_t color) {
    strip.setPixelColor(i, color);
    strip.show();
    delay(BLINK_TIME);  // Consider removing this for a non-blocking approach
  }