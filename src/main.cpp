#include <Arduino.h>
#include "ZlabUltrasonic.h"

// --- Pin Definitions ---
#define TRIG_PIN 5
#define ECHO_PIN 6

// --- ANSI Color Codes for a beautiful terminal ---
#define COLOR_RESET  "\x1B[0m"
#define COLOR_RED    "\x1B[31m"
#define COLOR_GREEN  "\x1B[32m"
#define COLOR_YELLOW "\x1B[33m"
#define COLOR_BLUE   "\x1B[34m"
#define COLOR_CYAN   "\x1B[36m"
#define COLOR_WHITE  "\x1B[37m"

// --- Global Sensor Object ---
ZlabUltrasonic mySensor(TRIG_PIN, ECHO_PIN);

// --- Global state for the interactive menu ---
int currentMode = 0;

void printMenu() {
    Serial.println("\n\n-----------------------------------------------------");
    Serial.println(COLOR_CYAN "         ZLAB Interactive Sensor Interface" COLOR_RESET);
    Serial.println("-----------------------------------------------------");
    Serial.println(COLOR_BLUE "Select an operation mode:" COLOR_RESET);
    Serial.println(COLOR_YELLOW "  1." COLOR_WHITE " Raw Pulse Duration Test (Hardware & Timing Check)");
    Serial.println(COLOR_YELLOW "  2." COLOR_WHITE " Simple Distance Test (cm & inch)");
    Serial.println(COLOR_YELLOW "  3." COLOR_WHITE " Advanced Filtered Reading Test");
    Serial.print("\nEnter mode number (1-3): " COLOR_GREEN);
}

void setup() {
    Serial.begin(115200);
    while (!Serial) {
        delay(10);
    }
    mySensor.setTemperature(25.0);
    printMenu();
}

void loop() {
    // Check for user input to change mode
    if (Serial.available() > 0) {
        char input = Serial.read();
        if (input >= '1' && input <= '3') {
            currentMode = input - '0'; // Convert char '1' to int 1, etc.
            Serial.println(currentMode);
            Serial.print(COLOR_GREEN "\n--- Starting Mode ");
            Serial.print(currentMode);
            Serial.println(" ---" COLOR_RESET);
            delay(500); // Give user time to see the mode change
        } else {
            // Ignore other characters but clear the buffer
            while(Serial.available()) Serial.read();
        }
    }

    // Execute the selected mode
    switch (currentMode) {
        case 1: {
            long duration = mySensor.getRawPulseDuration();
            if (mySensor.getStatus() == Status::TIMEOUT_ERROR) {
                Serial.println(COLOR_RED "Error: Timeout! Sensor is not connected or not detecting echo." COLOR_RESET);
            } else {
                Serial.print(COLOR_WHITE "Raw Pulse Duration: " COLOR_GREEN);
                Serial.print(duration);
                Serial.println(" us" COLOR_RESET);
            }
            delay(250);
            break;
        }

        case 2: {
            float distance_cm = mySensor.getDistance(Unit::CM);
            if (mySensor.getStatus() == Status::TIMEOUT_ERROR) {
                Serial.println(COLOR_RED "Error: Could not read distance (Timeout)." COLOR_RESET);
            } else {
                float distance_inch = distance_cm / 2.54;
                Serial.print(COLOR_WHITE "Simple Distance: " COLOR_GREEN);
                Serial.print(distance_cm, 2);
                Serial.print(" cm" COLOR_RESET "  |  ");
                Serial.print(COLOR_GREEN);
                Serial.print(distance_inch, 2);
                Serial.println(" inch" COLOR_RESET);
            }
            delay(250);
            break;
        }

        case 3: {
            float raw_dist = mySensor.getDistance(Unit::CM);
            // We call getFilteredDistance *after* getDistance to ensure status is fresh
            float filtered_dist = mySensor.getFilteredDistance(); 

            if (mySensor.getStatus() == Status::TIMEOUT_ERROR) {
                Serial.println(COLOR_RED "Error: Could not read distance (Timeout)." COLOR_RESET);
            } else {
                Serial.print(COLOR_WHITE "Raw: " COLOR_YELLOW);
                Serial.print(raw_dist, 2);
                Serial.print(" cm" COLOR_RESET "  |  ");
                Serial.print(COLOR_WHITE "Filtered: " COLOR_GREEN);
                Serial.print(filtered_dist, 2);
                Serial.println(" cm" COLOR_RESET);
            }
            delay(100); // Faster updates for filter testing
            break;
        }

        default:
            // Do nothing, wait for user to select a mode
            delay(100);
            break;
    }
}