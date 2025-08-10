/**
 * @file ZlabUltrasonic.cpp
 * @brief Implementation file for the ZlabUltrasonic library.
 */
#include "ZlabUltrasonic.h"

// The constructor sets up the pins and default values.
ZlabUltrasonic::ZlabUltrasonic(uint8_t trigPin, uint8_t echoPin) {
    _trigPin = trigPin;
    _echoPin = echoPin;

    pinMode(_trigPin, OUTPUT);
    pinMode(_echoPin, INPUT);

    // Set a default temperature for initial calculations.
    setTemperature(20.0);
}

// Sets the temperature for accurate calculations.
void ZlabUltrasonic::setTemperature(float tempC) {
    _temperatureC = tempC;
}

// Private function to get the raw pulse duration from the sensor.
long ZlabUltrasonic::_getRawPulseDuration() {
    // Send a 10 microsecond pulse to trigger the sensor.
    digitalWrite(_trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(_trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trigPin, LOW);

    // Read the echo pulse duration.
    // pulseIn() waits for the pin to go HIGH, starts timing, then waits for the
    // pin to go LOW and stops timing. The duration is returned in microseconds.
    // A timeout of 30,000µs (30ms) is used to prevent blocking indefinitely.
    return pulseIn(_echoPin, HIGH, 30000);
}

// Calculates and returns the distance.
float ZlabUltrasonic::getDistance(Unit unit) {
    long duration = _getRawPulseDuration();

    // If duration is 0, it indicates a timeout (error).
    if (duration == 0) {
        return -1.0f;
    }

    // Calculate the speed of sound in meters/second based on temperature.
    float speedOfSound_mps = 331.3 + 0.606 * _temperatureC;

    // Convert duration (µs) to seconds and calculate distance in cm.
    // Formula: distance = (duration * speed_of_sound) / 2
    float distance_cm = (duration * 0.000001 * speedOfSound_mps * 100) / 2.0;

    if (unit == Unit::INCH) {
        return distance_cm / 2.54;
    }
    return distance_cm;
}

// Checks if an object is within the specified threshold.
bool ZlabUltrasonic::isObjectDetected(float threshold_cm) {
    float currentDistance = getDistance(Unit::CM);

    // Return true only if the reading is valid ( > 0) and within the threshold.
    if (currentDistance > 0 && currentDistance <= threshold_cm) {
        return true;
    }
    return false;
}

// Calculates a stable distance reading by averaging over 100ms.
float ZlabUltrasonic::getMovingAverageDistance(int sample_interval_ms) {
    std::vector<float> readings;
    unsigned long startTime = millis();
    const int sampling_duration_ms = 100; // Total duration to read samples.

    // Step 1: Collect data for 100 milliseconds.
    while (millis() - startTime < sampling_duration_ms) {
        float dist = getDistance(Unit::CM);
        if (dist > 0) { // Only store valid readings.
            readings.push_back(dist);
        }
        delay(sample_interval_ms);
    }

    // Step 2: If no valid data was collected, return an error.
    if (readings.empty()) {
        return -1.0f;
    }

    // Step 3: Calculate and return the average of the valid readings.
    float sum = 0;
    for (float r : readings) {
        sum += r;
    }
    return sum / readings.size();
}