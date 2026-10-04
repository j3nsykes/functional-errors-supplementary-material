/*
 * Servo Detection Test Sketch
 * 
 * This sketch tests the enhanced SignalDetector to verify it can
 * distinguish between different signal types.
 * 
 * SETUP:
 * 1. Connect a test pin that can output signals to A4
 * 2. Upload this sketch
 * 3. Open Serial Monitor at 9600 baud
 * 
 * The sketch will cycle through different signal types and report
 * what the detector identifies.
 */

#include "signalDetect_Enhanced.h"
#include <Servo.h>

// Test output pin (generates signals)
const int OUTPUT_PIN = 9;  // Pin 9 can do PWM and Servo

// Test input pin (detects signals)
const int INPUT_PIN = A4;

SignalDetector* detector;
Servo testServo;

unsigned long lastTest = 0;
int testMode = 0;

void setup() {
  Serial.begin(9600);
  
  Serial.println("========================================");
  Serial.println("Signal Detection Test");
  Serial.println("========================================");
  Serial.println();
  Serial.println("Connect pin 9 to A4 (use jumper wire)");
  Serial.println("This test will cycle through:");
  Serial.println("  1. Digital LOW");
  Serial.println("  2. Digital HIGH");
  Serial.println("  3. PWM 25%");
  Serial.println("  4. PWM 50%");
  Serial.println("  5. PWM 75%");
  Serial.println("  6. Servo 0°");
  Serial.println("  7. Servo 90°");
  Serial.println("  8. Servo 180°");
  Serial.println("========================================");
  Serial.println();
  
  // Setup detector
  detector = new SignalDetector(INPUT_PIN);
  
  delay(2000);
}

void loop() {
  // Change test mode every 3 seconds
  if (millis() - lastTest > 3000) {
    lastTest = millis();
    testMode++;
    if (testMode > 7) testMode = 0;
    
    // Generate different signals
    switch(testMode) {
      case 0:
        // Digital LOW
        testServo.detach();
        pinMode(OUTPUT_PIN, OUTPUT);
        digitalWrite(OUTPUT_PIN, LOW);
        Serial.println("\n--- Testing: Digital LOW ---");
        break;
        
      case 1:
        // Digital HIGH
        digitalWrite(OUTPUT_PIN, HIGH);
        Serial.println("\n--- Testing: Digital HIGH ---");
        break;
        
      case 2:
        // PWM 25%
        analogWrite(OUTPUT_PIN, 64);
        Serial.println("\n--- Testing: PWM 25% duty ---");
        break;
        
      case 3:
        // PWM 50%
        analogWrite(OUTPUT_PIN, 128);
        Serial.println("\n--- Testing: PWM 50% duty ---");
        break;
        
      case 4:
        // PWM 75%
        analogWrite(OUTPUT_PIN, 192);
        Serial.println("\n--- Testing: PWM 75% duty ---");
        break;
        
      case 5:
        // Servo 0°
        testServo.attach(OUTPUT_PIN);
        testServo.write(0);
        Serial.println("\n--- Testing: Servo 0° ---");
        break;
        
      case 6:
        // Servo 90°
        testServo.write(90);
        Serial.println("\n--- Testing: Servo 90° ---");
        break;
        
      case 7:
        // Servo 180°
        testServo.write(180);
        Serial.println("\n--- Testing: Servo 180° ---");
        break;
    }
  }
  
  // Read and report detection
  if (detector->hasValidSignal()) {
    Serial.print("Detected: ");
    Serial.print(detector->getSignalTypeString());
    Serial.print(" | Freq: ");
    Serial.print(detector->getFrequency(), 1);
    Serial.print("Hz | Pulse: ");
    Serial.print(detector->getPulseWidth());
    Serial.print("μs | Duty: ");
    Serial.print(detector->getDutyCycle(), 1);
    Serial.print("%");
    
    if (detector->isServoSignal()) {
      Serial.print(" | Angle: ");
      Serial.print(detector->getServoAngle());
      Serial.print("°");
    }
    
    Serial.println();
  }
  
  delay(200);  // Report 5 times per second
}

/*
 * EXPECTED RESULTS:
 * 
 * Digital LOW:
 *   Detected: DIGITAL | Freq: ~490Hz | Duty: <2%
 * 
 * Digital HIGH:
 *   Detected: DIGITAL | Freq: ~490Hz | Duty: >98%
 * 
 * PWM 25%:
 *   Detected: PWM | Freq: ~490Hz | Duty: ~25%
 * 
 * PWM 50%:
 *   Detected: PWM | Freq: ~490Hz | Duty: ~50%
 * 
 * PWM 75%:
 *   Detected: PWM | Freq: ~490Hz | Duty: ~75%
 * 
 * Servo 0°:
 *   Detected: SERVO | Freq: ~50Hz | Pulse: ~1000μs | Angle: 0°
 * 
 * Servo 90°:
 *   Detected: SERVO | Freq: ~50Hz | Pulse: ~1500μs | Angle: 90°
 * 
 * Servo 180°:
 *   Detected: SERVO | Freq: ~50Hz | Pulse: ~2000μs | Angle: 180°
 * 
 * 
 * TROUBLESHOOTING:
 * 
 * If all signals show as PWM:
 *   - Check your jumper wire connection
 *   - Verify you're using pin 9 and A4
 * 
 * If no signals detected:
 *   - Check that you've connected pin 9 to A4
 *   - Verify the wire isn't loose
 * 
 * If servo detection is unreliable:
 *   - Servo library can be finicky
 *   - Try different servo positions
 *   - May need to adjust tolerance in signalDetect_Enhanced.h
 */
