#include "listener.h"
#include "signalDetect_Enhanced.h"
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

// PWM input pins - using ResponsivePWMListener
ResponsivePWMListener* pwmPins[6];

// Floating pin detectors - using ResponsiveFloatingDetector
ResponsiveFloatingDetector* floatingDetectors[6];

// Analog input pins - using ResponsiveAnalogInput
ResponsiveAnalogInput* analogPins[4];

// Neopixel output pins
int neodigOutPins[6] = { 0, 2, 5, 6, 10, 11 };  // For digital pins
int pwmOutPins[6] = { 1, 3, 4, 7, 8, 9 };       // For PWM/Servo pins
int anagOutPins[4] = { 12, 13, 14, 15 };        // For analog pins

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
  for (int i = 0; i < totalAPins; i++) {
    analogPins[i] = new ResponsiveAnalogInput(A0 + i, true, 0.01);
    analogPins[i]->setActivityThreshold(4.0);
  }

  strip.begin();
  strip.setBrightness(50);
  strip.show();
  
  Serial.println("========================================");
  Serial.println("Feedback Shield with Servo Detection");
  Serial.println("========================================");
  Serial.println("LED Color Guide:");
  Serial.println("  GREEN = Digital HIGH");
  Serial.println("  RED = Digital LOW");
  Serial.println("  BLUE (fade) = PWM signal");
  Serial.println("  MAGENTA (fade) = Servo signal");
  Serial.println("  CYAN = Floating/unconnected");
  Serial.println("  BLUE (analog pins) = Analog input");
  Serial.println("========================================");
}

void loop() {
  // Update all responsive analog objects first
  updateResponsiveInputs();
  
  // Then check states
  digCheck();
  pwmServoCheck();  // Combined PWM and Servo detection
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
  // Check digital pins
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

void pwmServoCheck() {
  // Check PWM/Servo/Floating pins with enhanced detection
  for (int i = 0; i < totalPWMPins; i++) {
    
    // First check if pin is floating (unconnected)
    if (floatingDetectors[i]->isFloating()) {
      // Pin is floating - show cyan (light blue)
      int brightness = pwmPins[i]->getDutyCycle();
      neoDisplay(pwmOutPins[i], strip.Color(0, brightness/2, brightness));  // Cyan
      
    } else if (detectorsPWM[i]->hasValidSignal()) {
      // Pin has a valid signal - determine type
      
      if (detectorsPWM[i]->isServoSignal()) {
        // SERVO DETECTED!
        int angle = detectorsPWM[i]->getServoAngle();
        
        // Map angle (0-180) to brightness (50-255)
        int brightness = map(angle, 0, 180, 50, 255);
        
        // Show magenta (purple) with brightness based on angle
        neoDisplay(pwmOutPins[i], strip.Color(brightness, 0, brightness));  // Magenta
        
        // Optional: print to serial for debugging
        // Serial.print("Servo on pin ");
        // Serial.print(i);
        // Serial.print(": ");
        // Serial.print(angle);
        // Serial.println("°");
        
      } else if (detectorsPWM[i]->isPWMSignal()) {
        // STANDARD PWM DETECTED
        int duty = pwmPins[i]->getDutyCycle();
        
        // Show blue with brightness based on duty cycle
        neoDisplay(pwmOutPins[i], strip.Color(0, 0, duty));  // Blue
        
      } else if (detectorsPWM[i]->isDigitalSignal()) {
        // Digital-like signal (constant HIGH or LOW)
        float dutyCycle = detectorsPWM[i]->getDutyCycle();
        
        if (dutyCycle > 50) {
          neoDisplay(pwmOutPins[i], strip.Color(0, 255, 0));  // Green (HIGH)
        } else {
          neoDisplay(pwmOutPins[i], strip.Color(255, 0, 0));  // Red (LOW)
        }
        
      } else {
        // Unknown signal type - show white
        neoDisplay(pwmOutPins[i], strip.Color(100, 100, 100));  // White
      }
      
    } else {
      // No signal detected - LED off
      neoDisplay(pwmOutPins[i], strip.Color(0, 0, 0));  // Off
    }
  }
}

void analogCheck() {
  // Handle analog input pins (A0-A3)
  for (int i = 0; i < totalAPins; i++) {
    // Only update LED if value has changed (reduces unnecessary updates)
    if (analogPins[i]->hasChanged()) {
      int mappedValue = analogPins[i]->getMappedValue();
      neoDisplay(anagOutPins[i], strip.Color(0, 0, mappedValue));  // Blue
    }
  }
}

void neoDisplay(int i, uint32_t color) {
  strip.setPixelColor(i, color);
  strip.show();
  delay(BLINK_TIME);
}
