#include <Arduino.h>
#include "ZlabUltrasonic.h"

// --- Pin Definitions ---
#define TRIG_PIN 5
#define ECHO_PIN 6

// --- ANSI Color Codes for the Serial Monitor ---
#define CLR_RESET  "\x1B[0m"
#define CLR_RED    "\x1B[31m"
#define CLR_GREEN  "\x1B[32m"
#define CLR_YELLOW "\x1B[33m"
#define CLR_BLUE   "\x1B[34m"
#define CLR_CYAN   "\x1B[36m"
#define CLR_WHITE  "\x1B[37m"
#define BOLD       "\x1B[1m"

// Global sensor object
ZlabUltrasonic mySensor(TRIG_PIN, ECHO_PIN);

// Global variables for menu state management
int currentMode = 0;
char mode1_unit = 0; // Holds the selected unit for Mode 1 ('c' for cm, 'i' for inch)

/**
 * @brief Prints the main menu to the Serial Monitor.
 */
void printMenu() {
    Serial.println("\n\n=======================================================");
    Serial.print(BOLD);
    Serial.println(CLR_CYAN "         ZLAB Ultrasonic Sensor - Control Panel" CLR_RESET);
    Serial.println("=======================================================");
    Serial.println(CLR_BLUE "Please select a test mode:" CLR_RESET);
    Serial.println(CLR_YELLOW "  1. " CLR_WHITE "Get Distance (Interactive Unit Selection)");
    Serial.println(CLR_YELLOW "  2. " CLR_WHITE "Detect Object (Test with a 30cm threshold)");
    Serial.println(CLR_YELLOW "  3. " CLR_WHITE "Get Moving Average (Raw vs. Filtered)");
    Serial.println(CLR_WHITE "Press 'q' anytime to return to this menu." CLR_RESET);
    Serial.print("\nEnter mode number (1-3): " CLR_GREEN);
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
    // Check for user input from the Serial Monitor
    if (Serial.available() > 0) {
        char input = tolower(Serial.read()); // Read input and convert to lowercase

        // NEW: Universal 'q' to quit and return to the main menu
        if (input == 'q') {
            currentMode = 0; // Stop any active mode
            mode1_unit = 0;  // Reset sub-mode state
            Serial.println("q");
            Serial.println(BOLD CLR_YELLOW "\nReturning to Control Panel..." CLR_RESET);
            printMenu();
        }
        // Handle unit selection for Mode 1
        else if (currentMode == 1 && mode1_unit == 0) {
            if (input == 'c') {
                mode1_unit = 'c';
                Serial.println("cm");
                Serial.println(CLR_GREEN "Unit set to Centimeters. Displaying readings..." CLR_RESET);
            } else if (input == 'i') {
                mode1_unit = 'i';
                Serial.println("inch");
                Serial.println(CLR_GREEN "Unit set to Inches. Displaying readings..." CLR_RESET);
            } else {
                 Serial.println(CLR_RED "Invalid choice. Please enter 'c' or 'i'." CLR_RESET);
            }
        }
        // Handle main menu selection
        else if (input >= '1' && input <= '3') {
            currentMode = input - '0';
            mode1_unit = 0; // Reset unit selection when changing modes
            Serial.println(currentMode);
            Serial.print(CLR_GREEN BOLD "\n--- Activating Mode ");
            Serial.print(currentMode);
            Serial.println(" ---" CLR_RESET);

            if (currentMode == 1) {
                Serial.print(CLR_YELLOW "Please choose a unit (c for CM, i for INCH): " CLR_RESET);
            }
            delay(500);
        } else {
            // Clear any other invalid characters from the buffer
            while (Serial.available()) Serial.read();
        }
    }

    // Execute code based on the selected mode
    switch (currentMode) {
        case 1: {
            if (mode1_unit == 'c' || mode1_unit == 'i') {
                Unit selectedUnit = (mode1_unit == 'c') ? Unit::CM : Unit::INCH;
                const char* unitStr = (mode1_unit == 'c') ? "cm" : "in";
                float dist = mySensor.getDistance(selectedUnit);

                Serial.print(CLR_WHITE "Distance: " CLR_RESET);
                if (dist > 0) Serial.printf(CLR_GREEN "%.2f %s" CLR_RESET, dist, unitStr);
                else Serial.print(CLR_RED "Error" CLR_RESET);
                Serial.println();
                delay(500);
            }
            // If no unit is chosen yet, do nothing and wait for input.
            break;
        }

        case 2: {
            float threshold = 30.0;
            bool detected = mySensor.isObjectDetected(threshold);

            Serial.print(CLR_WHITE "Checking for object within " CLR_YELLOW);
            Serial.print(threshold, 0);
            Serial.print(" cm... " CLR_RESET);

            if (detected) {
                Serial.println(BOLD CLR_GREEN "OBJECT DETECTED ✔" CLR_RESET);
            } else {
                Serial.println(CLR_RED "No object found ✖" CLR_RESET);
            }
            delay(300);
            break;
        }

        case 3: {
            float raw_dist = mySensor.getDistance(Unit::CM);
            float filtered_dist = mySensor.getMovingAverageDistance();

            Serial.print(CLR_WHITE "Raw: " CLR_RESET);
            if(raw_dist > 0) Serial.printf(CLR_YELLOW "%.2f cm" CLR_RESET, raw_dist);
            else Serial.print(CLR_RED "Error" CLR_RESET);

            Serial.print(CLR_WHITE " | Filtered: " CLR_RESET);
            if(filtered_dist > 0) Serial.printf(BOLD CLR_GREEN "%.2f cm" CLR_RESET, filtered_dist);
            else Serial.print(CLR_RED "Error" CLR_RESET);
            Serial.println();
            
            delay(500);
            break;
        }
    }
}