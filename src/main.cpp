#include <Arduino.h>
#include "ZlabUltrasonic.h" 

// Define sensor pins
#define TRIG_PIN 5
#define ECHO_PIN 6

// Create an instance of our library
ZlabUltrasonic mySensor(TRIG_PIN, ECHO_PIN);

void setup() {
    Serial.begin(115200);
    Serial.println("ZLAB Ultrasonic Sensor Test");

    // Let's set the ambient temperature for higher accuracy (Creative Step!)
    mySensor.setTemperature(25.0); // Assuming it's 25°C
}

void loop() {
    // 1. Get simple distance
    float distance_cm = mySensor.getDistance("cm");
    float distance_inch = mySensor.getDistance("inch");

    Serial.print("Simple Distance: ");
    Serial.print(distance_cm);
    Serial.print(" cm  |  ");
    Serial.print(distance_inch);
    Serial.println(" inch");

    // 2. Check if an object is within 20cm
    if (mySensor.isObjectDetected(20.0)) {
        Serial.println("!!! Object detected within 20cm !!!");
    }

    // 3. Get filtered distance (Kalman Filter)
    float filtered_distance = mySensor.getFilteredDistance();
    Serial.print("Filtered Distance (Kalman): ");
    Serial.print(filtered_distance);
    Serial.println(" cm");

    Serial.println("------------------------------------");

    delay(1000);
}