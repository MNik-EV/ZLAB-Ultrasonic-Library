#include "ZlabUltrasonic.h"
#include "esp32-hal-timer.h" // For esp_timer_get_time()

// Constructor
ZlabUltrasonic::ZlabUltrasonic(uint8_t trigPin, uint8_t echoPin) {
    _trigPin = trigPin;
    _echoPin = echoPin;

    pinMode(_trigPin, OUTPUT);
    pinMode(_echoPin, INPUT);

    // Set default temperature to 20°C
    setTemperature(20.0);
}

void ZlabUltrasonic::setTemperature(float tempC) {
    // Speed of sound formula: 331.3 + 0.606 * Temp(C) m/s
    // Converting to cm/µs: (m/s) * 100 / 1,000,000 = (m/s) / 10000
    _speedOfSound = (331.3 + 0.606 * tempC) / 10000.0;
}

long ZlabUltrasonic::_getRawPulseDuration() {
    // Send a 10µs trigger pulse to start the measurement
    digitalWrite(_trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(_trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trigPin, LOW);

    // Measure the echo pulse duration using the high-precision ESP32 timer
    // This is a non-blocking approach compared to the standard pulseIn()
    unsigned long startTime_us = esp_timer_get_time();
    unsigned long timeout_us = startTime_us + 30000; // 30ms timeout for safety (covers ~5m range)

    // Wait for the echo pin to go HIGH (or timeout)
    while (digitalRead(_echoPin) == LOW && esp_timer_get_time() < timeout_us);
    
    // If we timed out waiting for the pulse to start, return 0
    if (esp_timer_get_time() >= timeout_us) return 0;
    
    startTime_us = esp_timer_get_time();

    // Wait for the echo pin to go LOW (or timeout)
    while (digitalRead(_echoPin) == HIGH && esp_timer_get_time() < timeout_us);
    
    // If we timed out while the pulse was high, it's an invalid reading
    if (esp_timer_get_time() >= timeout_us) return 0;

    unsigned long endTime_us = esp_timer_get_time();
    
    return endTime_us - startTime_us;
}

float ZlabUltrasonic::getDistance(const char* unit) {
    long duration = _getRawPulseDuration();
    if (duration == 0) {
        return -1.0; // Error condition
    }

    // Distance in cm = (Duration in µs * Speed of Sound in cm/µs) / 2 (for round trip)
    float distance_cm = (duration * _speedOfSound) / 2.0;

    if (strcmp(unit, "inch") == 0) {
        return distance_cm / 2.54;
    } else {
        return distance_cm;
    }
}

bool ZlabUltrasonic::isObjectDetected(float threshold_cm) {
    float currentDistance = getDistance("cm");
    if (currentDistance < 0) {
        return false; // Error, assume no object
    }
    return currentDistance <= threshold_cm;
}

float ZlabUltrasonic::getFilteredDistance() {
    float measurement = getDistance("cm");

    // --- Data Validation Stage ---

    // 1. Range Gate: Reject readings outside the sensor's valid physical range.
    if (measurement < 2.0 || measurement > 400.0) {
        // If filter is initialized, return the last known good value. Otherwise, return error.
        return _kalman_initialized ? _kalman_x : -1.0;
    }

    // 2. Outlier Rejection: Reject readings that deviate too much from recent history.
    if (_lastReadings.size() >= READING_WINDOW_SIZE) {
        float sum = 0;
        for (float r : _lastReadings) {
            sum += r;
        }
        float avg = sum / _lastReadings.size();

        // If the new measurement is too far from the recent average, it's likely an outlier.
        if (abs(measurement - avg) > 15.0) { // Threshold: 15cm deviation (can be tuned)
            return _kalman_x; // Reject the outlier and return the last good filtered value.
        }
        // If the reading is valid, remove the oldest reading to make space for the new one.
        _lastReadings.erase(_lastReadings.begin());
    }
    _lastReadings.push_back(measurement);

    // --- Filtering Stage ---
    // Now, feed the validated ("clean") measurement to the Kalman filter.
    if (!_kalman_initialized) {
        // Initialize the filter with the first valid measurement.
        _kalman_x = measurement;
        _kalman_initialized = true;
        return _kalman_x;
    }

    // Kalman Filter Prediction Step
    float P_pred = _kalman_P + _kalman_Q;

    // Kalman Filter Update (Correction) Step
    float K = P_pred / (P_pred + _kalman_R); // Calculate Kalman Gain
    _kalman_x = _kalman_x + K * (measurement - _kalman_x); // Update the state estimate
    _kalman_P = (1 - K) * P_pred; // Update the estimation error

    return _kalman_x;
}