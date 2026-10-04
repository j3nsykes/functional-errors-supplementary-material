/*
 * Debug Sketch for ResponsiveAnalogRead Integration
 * 
 * This sketch helps verify that your ResponsiveAnalogRead
 * integration is working correctly. Upload this to test
 * before using the full Feedback Shield code.
 * 
 * Monitor with Serial Monitor at 9600 baud.
 */

#include <ResponsiveAnalogRead.h>
#include "responsiveAnalog.h"
#include "responsivePWM.h"
#include "responsiveFloating.h"

// Test one of each type
ResponsiveAnalogInput* testAnalog;
ResponsivePWMListener* testPWM;
ResponsiveFloatingDetector* testFloating;

void setup() {
  Serial.begin(9600);
  
  Serial.println("=================================");
  Serial.println("ResponsiveAnalogRead Test Sketch");
  Serial.println("=================================");
  Serial.println();
  
  // Initialize test objects
  testAnalog = new ResponsiveAnalogInput(A0, true, 0.01);
  testAnalog->setActivityThreshold(4.0);
  
  testPWM = new ResponsivePWMListener(A4);
  
  testFloating = new ResponsiveFloatingDetector(A5);
  
  Serial.println("Connect test signals to:");
  Serial.println("  A0 - Analog input (potentiometer or voltage)");
  Serial.println("  A4 - PWM signal");
  Serial.println("  A5 - Floating pin (leave disconnected)");
  Serial.println();
  Serial.println("Monitoring will start in 2 seconds...");
  delay(2000);
  
  Serial.println();
  Serial.println("Format: [Type] Raw | Smooth | Changed | Sleeping | Notes");
  Serial.println("-----------------------------------------------------------");
}

void loop() {
  // Update all objects
  testAnalog->update();
  testPWM->update();
  testFloating->update();
  
  // Test Analog Input (A0)
  if (testAnalog->hasChanged()) {
    Serial.print("[A0] ");
    Serial.print(testAnalog->getRawValue());
    Serial.print(" | ");
    Serial.print(testAnalog->getValue());
    Serial.print(" | CHANGED | ");
    Serial.print(testAnalog->isSleeping() ? "SLEEP" : "ACTIVE");
    Serial.print(" | Mapped: ");
    Serial.println(testAnalog->getMappedValue());
  }
  
  // Test PWM Input (A4)
  if (testPWM->hasChanged()) {
    Serial.print("[A4] ");
    Serial.print(testPWM->getRawValue());
    Serial.print(" | ");
    Serial.print(testPWM->getValue());
    Serial.print(" | CHANGED | ");
    Serial.print(testPWM->isSleeping() ? "SLEEP" : "ACTIVE");
    Serial.print(" | Duty: ");
    Serial.print(testPWM->getDutyCycle());
    Serial.println("/255");
  }
  
  // Test Floating Detection (A5)
  static unsigned long lastFloatingReport = 0;
  if (millis() - lastFloatingReport > 1000) {  // Report every second
    Serial.print("[A5] ");
    Serial.print(testFloating->getRawValue());
    Serial.print(" | ");
    Serial.print(testFloating->getValue());
    Serial.print(" | ");
    Serial.print(testFloating->hasActivity() ? "CHANGED" : "stable");
    Serial.print(" | ");
    Serial.print(testFloating->isFloating() ? "FLOATING" : "stable");
    Serial.println();
    lastFloatingReport = millis();
  }
  
  delay(10);
}

/*
 * EXPECTED RESULTS:
 * 
 * 1. A0 (Analog Input):
 *    - When you turn a potentiometer, you should see CHANGED appear
 *    - Raw values will be noisy (jumping around)
 *    - Smooth values should change gradually without jumping
 *    - When stable, it should quickly go to SLEEP state
 *    - Mapped values should range 0-250 for LED brightness
 * 
 * 2. A4 (PWM Input):
 *    - Connect a PWM signal from Arduino pin 3, 5, 6, 9, 10, or 11
 *    - Raw values should be noisy
 *    - Smooth values should be clean
 *    - Duty cycle should match your analogWrite() value
 *    - Should sleep when PWM is stable
 * 
 * 3. A5 (Floating Detection):
 *    - With pin disconnected, should show FLOATING
 *    - Raw values will jump around wildly
 *    - Smooth values will try to follow but with delay
 *    - Should NOT sleep (always active)
 *    - When you connect to GND or 5V, should become "stable"
 * 
 * TROUBLESHOOTING:
 * 
 * If A0 values are too jittery:
 *   testAnalog = new ResponsiveAnalogInput(A0, true, 0.005);
 *   
 * If A4 PWM is too slow:
 *   In responsivePWM.h, increase snap to 0.1
 *   
 * If A5 doesn't detect floating:
 *   testFloating->setFloatingThreshold(5.0);
 */
