#include <Arduino.h>
#include "ZlabUltrasonic.h"

// =======================================================
//                  Select The Test Mode
// 1: Raw Pulse Duration Test (Hardware & Timing Check)
// 2: Raw Distance Test (Calculation & Unit Check)
// 3: Full Filtered Test with Debug Prints
#define TEST_MODE 3
// =======================================================


// --- Pin Definitions ---
// Make sure these pins match your wiring
#define TRIG_PIN 5
#define ECHO_PIN 6


// --- Global Sensor Object ---
ZlabUltrasonic mySensor(TRIG_PIN, ECHO_PIN);


void setup() {
    Serial.begin(115200);
    // This loop is important for boards with native USB (like ESP32-S3)
    // It waits until the Serial Monitor is open before printing.
    while (!Serial) {
        delay(10);
    }

    // Set a known temperature for consistent calculations
    mySensor.setTemperature(25.0); 
    
    Serial.println("\n--- ZLAB Sensor Debug Mode ---");
    Serial.print("Current Test Mode: ");
    Serial.println(TEST_MODE);
    Serial.println("------------------------------------");
}

void loop() {
#if TEST_MODE == 1
    // Test 1: Reads the raw pulse duration in microseconds.
    // GOAL: To verify the hardware and the core timing function (_getRawPulseDuration).
    // NOTE: For this test to work, you must temporarily move the `_getRawPulseDuration()`
    // function from the `private` to the `public` section of your ZlabUltrasonic.h file.

    long duration = mySensor._getRawPulseDuration();
    Serial.print("Raw Pulse Duration: ");
    Serial.print(duration);
    Serial.println(" us");

#elif TEST_MODE == 2
    // Test 2: Reads the raw distance and corrects the inch conversion bug.
    // GOAL: To verify the distance calculation logic.
    
    float distance_cm = mySensor.getDistance("cm");

    if (distance_cm > 0) {
        // Calculate inches from the SAME reading to avoid inconsistency.
        float distance_inch = distance_cm / 2.54; 
        
        Serial.print("Simple Distance: ");
        Serial.print(distance_cm, 2); // Print with 2 decimal places
        Serial.print(" cm  |  ");
        Serial.print(distance_inch, 2); // Print with 2 decimal places
        Serial.println(" inch");
    } else {
        Serial.println("Error: Could not read distance.");
    }

#elif TEST_MODE == 3
    // Test 3: Full test with debug output for the filter.
    // GOAL: To see the values inside the filter and the outlier rejection logic.
    // NOTE: This test will require adding temporary Serial.print statements inside
    // the ZlabUltrasonic.cpp file to be useful.
    
    float raw_dist = mySensor.getDistance("cm");
    float filtered_dist = mySensor.getFilteredDistance();

    Serial.print("Raw: ");
    Serial.print(raw_dist, 2);
    Serial.print(" cm  |  Filtered: ");
    Serial.print(filtered_dist, 2);
    Serial.println(" cm");

#endif

    // A small delay between measurements
    delay(500); 
}