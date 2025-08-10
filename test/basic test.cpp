/**
 * @file simple_test.cpp
 * @brief Simple Test for ZlabUltrasonic Library - Interview Demo
 * @version 2.0.0
 * @author ZLAB Team
 * @date 2024
 * * Simplified version without complex UI for reliable compilation
 */

#include <Arduino.h>
#include "ZlabUltrasonic.h"

// =============================================================================
// HARDWARE CONFIGURATION
// =============================================================================

// Pin definitions - ESP32 configuration
#define TRIG_PIN 5       // GPIO5 on ESP32
#define ECHO_PIN 18      // GPIO18 on ESP32
#define LED_PIN 2        // Built-in LED

// =============================================================================
// GLOBAL VARIABLES
// =============================================================================

ZlabUltrasonic sensor(TRIG_PIN, ECHO_PIN, true);  // Enable debug output
unsigned long lastMeasurement = 0;
uint8_t testMode = 1;  // 1-6 for different test modes
unsigned long testStartTime = 0;

// =============================================================================
// FUNCTION PROTOTYPES  
// =============================================================================
void runBasicTest();
void runObjectDetectionTest();
void runFilteredTest();
void runEnvironmentalTest();
void runGeographicTest();
void runDiagnosticTest();


// =============================================================================
// SETUP FUNCTION
// =============================================================================

void setup() {
    Serial.begin(115200);
    
    // Wait for Serial connection
    while (!Serial && millis() < 3000) {
        delay(10);
    }
    
    pinMode(LED_PIN, OUTPUT);
    
    // Print welcome message
    Serial.println("╔══════════════════════════════════════════════════════════════╗");
    Serial.println("║                    🚀 ZLAB ULTRASONIC                        ║");
    Serial.println("║              Professional Distance Sensor Library            ║");
    Serial.println("║                        Version 2.0.0                        ║");
    Serial.println("╚══════════════════════════════════════════════════════════════╝");
    Serial.println();
    
    // Initialize sensor
    Serial.println("🔧 Initializing ZlabUltrasonic sensor...");
    SensorStatus initStatus = sensor.initialize();
    
    if (initStatus == SensorStatus::OK) {
        Serial.println("✅ Sensor initialization successful!");
        
        // Set environmental conditions for Tehran
        sensor.setEnvironmentalConditions(25.0f, 880.0f, 40.0f);
        Serial.println("🌡️  Environmental conditions set (Tehran: 25°C, 880hPa, 40%)");
        
        // Configure filter for optimal performance
        sensor.configureFilter(0.01f, 0.8f, 12.0f);
        Serial.println("🔬 Kalman filter configured");
        
    } else {
        Serial.println("❌ Sensor initialization failed!");
        Serial.print("   Status: ");
        Serial.println(ZlabUltrasonic::getStatusString(initStatus));
        while(1); // Stop execution
    }
    
    testStartTime = millis();
    Serial.println("\n📡 Starting measurements...");
    Serial.println("Send '1'-'6' via Serial Monitor to change test mode");
    Serial.println("Current mode: 1 - Basic Measurement\n");
}

// =============================================================================
// MAIN LOOP
// =============================================================================

void loop() {
    // Handle serial commands
    if (Serial.available()) {
        char command = Serial.read();
        if (command >= '1' && command <= '6') {
            testMode = command - '0';
            Serial.println("\n" + String("═").substring(0, 50));
            Serial.println("Mode changed to: " + String(testMode));
            switch (testMode) {
                case 1: Serial.println("📏 Basic Measurement Demo"); break;
                case 2: Serial.println("🎯 Object Detection Demo"); break;
                case 3: Serial.println("🔬 Filtered Measurement Demo"); break;
                case 4: Serial.println("🌡️ Environmental Demo"); break;
                case 5: Serial.println("🗺️ Geographic Demo"); break;
                case 6: Serial.println("🔍 Diagnostic Demo"); break;
            }
            Serial.println(String("═").substring(0, 50) + "\n");
        }
    }
    
    // Run measurements every 200ms
    if (millis() - lastMeasurement >= 200) {
        lastMeasurement = millis();
        
        switch (testMode) {
            case 1: runBasicTest(); break;
            case 2: runObjectDetectionTest(); break;
            case 3: runFilteredTest(); break;
            case 4: runEnvironmentalTest(); break;
            case 5: runGeographicTest(); break;
            case 6: runDiagnosticTest(); break;
        }
    }
    
    delay(10);
}

// =============================================================================
// TEST IMPLEMENTATIONS
// =============================================================================

void runBasicTest() {
    Serial.println("📏 BASIC MEASUREMENT TEST");
    Serial.println("─────────────────────────");
    
    // Test different units
    float distanceCm = sensor.getDistance(DistanceUnit::CENTIMETER);
    float distanceInch = sensor.getDistance(DistanceUnit::INCH);
    float distanceMm = sensor.getDistance(DistanceUnit::MILLIMETER);
    
    if (distanceCm > 0) {
        Serial.print("📐 Distance: ");
        Serial.print(distanceCm, 2);
        Serial.print(" cm | ");
        Serial.print(distanceInch, 2);
        Serial.print(" inch | ");
        Serial.print(distanceMm, 1);
        Serial.println(" mm");
        
        // Test averaged measurement
        float avgDistance = sensor.getDistance(MeasurementMode::AVERAGED, DistanceUnit::CENTIMETER);
        if (avgDistance > 0) {
            Serial.print("📊 Averaged: ");
            Serial.print(avgDistance, 2);
            Serial.print(" cm (diff: ");
            Serial.print(abs(distanceCm - avgDistance), 2);
            Serial.println(" cm)");
        }
        
        // LED indication
        digitalWrite(LED_PIN, distanceCm < 50.0f ? HIGH : LOW);
        
    } else {
        Serial.print("❌ Error: ");
        Serial.println(ZlabUltrasonic::getStatusString(sensor.getLastStatus()));
        digitalWrite(LED_PIN, LOW);
    }
    Serial.println();
}

void runObjectDetectionTest() {
    Serial.println("🎯 OBJECT DETECTION TEST");
    Serial.println("─────────────────────────");
    
    float threshold = 30.0f;  // 30cm threshold
    bool detected = sensor.isObjectDetected(threshold);
    float distance = sensor.getDistance(DistanceUnit::CENTIMETER);
    
    if (distance > 0) {
        Serial.print("📡 Distance: ");
        Serial.print(distance, 1);
        Serial.print(" cm | Threshold: ");
        Serial.print(threshold, 0);
        Serial.print(" cm | Status: ");
        
        if (detected) {
            Serial.println("🔴 OBJECT DETECTED");
            digitalWrite(LED_PIN, HIGH);
        } else {
            Serial.println("🟢 CLEAR");
            digitalWrite(LED_PIN, LOW);
        }
        
        // Range analysis
        Serial.print("📊 Range: ");
        if (distance <= 10.0f) Serial.println("VERY_CLOSE");
        else if (distance <= 30.0f) Serial.println("CLOSE");
        else if (distance <= 100.0f) Serial.println("MEDIUM");
        else Serial.println("FAR");
        
    } else {
        Serial.println("❌ Detection failed");
        digitalWrite(LED_PIN, LOW);
    }
    Serial.println();
}

void runFilteredTest() {
    Serial.println("🔬 FILTERED MEASUREMENT TEST");
    Serial.println("─────────────────────────────");
    
    float rawDistance = sensor.getDistance(DistanceUnit::CENTIMETER);
    float filteredDistance = sensor.getFilteredDistance();
    
    if (rawDistance > 0 && filteredDistance > 0) {
        float difference = abs(rawDistance - filteredDistance);
        float stability = (difference < 1.0f) ? 100.0f - (difference * 50.0f) : 50.0f;
        
        Serial.print("📊 Raw:      ");
        Serial.print(rawDistance, 2);
        Serial.println(" cm");
        
        Serial.print("🔬 Filtered: ");
        Serial.print(filteredDistance, 2);
        Serial.println(" cm");
        
        Serial.print("📈 Diff:     ");
        Serial.print(difference, 2);
        Serial.print(" cm | Stability: ");
        Serial.print(stability, 0);
        Serial.println("%");
        
        if (stability > 90.0f) {
            Serial.println("✅ Signal: EXCELLENT");
        } else if (stability > 70.0f) {
            Serial.println("⚠️ Signal: GOOD");
        } else {
            Serial.println("🔴 Signal: NOISY");
        }
        
        digitalWrite(LED_PIN, stability > 70.0f ? HIGH : LOW);
        
    } else {
        Serial.println("❌ Filtering in progress...");
        digitalWrite(LED_PIN, LOW);
    }
    Serial.println();
}

void runEnvironmentalTest() {
    static uint8_t tempIndex = 0;
    
    Serial.println("🌡️ ENVIRONMENTAL COMPENSATION TEST");
    Serial.println("───────────────────────────────────");
    
    // Cycle through different temperatures
    float testTemps[] = {-10.0f, 5.0f, 20.0f, 35.0f, 50.0f};
    float currentTemp = testTemps[tempIndex % 5];
    
    sensor.setTemperature(currentTemp);
    
    float distance = sensor.getDistance(DistanceUnit::CENTIMETER);
    float soundSpeed = sensor.getCurrentSoundSpeed();
    
    Serial.print("🌡️ Temperature: ");
    Serial.print(currentTemp, 0);
    Serial.println("°C");
    
    Serial.print("🔊 Sound Speed: ");
    Serial.print(soundSpeed * 10000, 1);
    Serial.println(" m/s");
    
    if (distance > 0) {
        Serial.print("📏 Compensated Distance: ");
        Serial.print(distance, 2);
        Serial.println(" cm");
        
        if (currentTemp < 0) {
            Serial.println("❄️ Cold: Sound travels slower");
        } else if (currentTemp > 40) {
            Serial.println("🔥 Hot: Sound travels faster");
        } else {
            Serial.println("🌡️ Normal temperature range");
        }
    }
    
    tempIndex++;
    digitalWrite(LED_PIN, (millis() / 500) % 2);  // Slow blink
    Serial.println();
}

void runGeographicTest() {
    static bool isQeshm = false;
    
    Serial.println("🗺️ GEOGRAPHIC CALIBRATION TEST");
    Serial.println("───────────────────────────────");
    
    if (!isQeshm) {
        // Tehran conditions
        sensor.setEnvironmentalConditions(25.0f, 880.0f, 40.0f);
        Serial.println("📍 Location: TEHRAN");
        Serial.println("   Altitude: 1,190m | Pressure: 880 hPa | Humidity: 40%");
    } else {
        // Qeshm Island conditions
        sensor.setEnvironmentalConditions(32.0f, 1013.25f, 75.0f);
        Serial.println("📍 Location: QESHM ISLAND");
        Serial.println("   Altitude: 0m | Pressure: 1,013 hPa | Humidity: 75%");
    }
    
    float distance = sensor.getDistance(DistanceUnit::CENTIMETER);
    float soundSpeed = sensor.getCurrentSoundSpeed();
    
    Serial.print("🔊 Calibrated Sound Speed: ");
    Serial.print(soundSpeed * 10000, 2);
    Serial.println(" m/s");
    
    if (distance > 0) {
        Serial.print("📏 Geographic Distance: ");
        Serial.print(distance, 2);
        Serial.println(" cm");
        
        Serial.println("💡 Factors compensated:");
        Serial.println("   • Altitude effect on air pressure");
        Serial.println("   • Temperature variation");
        Serial.println("   • Humidity absorption");
    }
    
    isQeshm = !isQeshm;
    digitalWrite(LED_PIN, isQeshm ? HIGH : LOW);
    Serial.println();
}

void runDiagnosticTest() {
    static unsigned long lastDiagnostic = 0;
    
    Serial.println("🔍 DIAGNOSTIC & STATISTICS TEST");
    Serial.println("────────────────────────────────");
    
    // Run hardware diagnostic every 5 seconds
    if (millis() - lastDiagnostic >= 5000) {
        Serial.println("🔧 Running hardware diagnostic...");
        bool result = sensor.performDiagnostic();
        Serial.print("🏥 Hardware Health: ");
        Serial.println(result ? "✅ EXCELLENT" : "❌ ISSUES DETECTED");
        lastDiagnostic = millis();
    }
    
    // Display statistics
    float successRate, avgDistance, minDistance, maxDistance;
    sensor.getStatistics(successRate, avgDistance, minDistance, maxDistance);
    
    Serial.println("📊 Performance Statistics:");
    Serial.print("   Success Rate:    ");
    Serial.print(successRate, 1);
    Serial.println("%");
    
    if (successRate > 0) {
        Serial.print("   Average Distance: ");
        Serial.print(avgDistance, 1);
        Serial.println(" cm");
        
        Serial.print("   Measurement Range: ");
        Serial.print(minDistance, 1);
        Serial.print(" - ");
        Serial.print(maxDistance, 1);
        Serial.println(" cm");
    }
    
    // Current reading
    float currentDistance = sensor.getDistance(DistanceUnit::CENTIMETER);
    if (currentDistance > 0) {
        Serial.print("📏 Live Reading: ");
        Serial.print(currentDistance, 2);
        Serial.print(" cm (");
        Serial.print(ZlabUltrasonic::getStatusString(sensor.getLastStatus()));
        Serial.println(")");
    }
    
    // Health indicator
    digitalWrite(LED_PIN, successRate > 90.0f ? HIGH : LOW);
    
    Serial.print("⏱️ Runtime: ");
    Serial.print((millis() - testStartTime) / 1000);
    Serial.println(" seconds");
    Serial.println();
}