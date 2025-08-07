#include "ZlabUltrasonic.h"
#include "esp32-hal-timer.h" 

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
    // Converting to cm/µs: (m/s) * 100 / 1,000,000 = (m/s) / 10,000
    _speedOfSound = (331.3 + 0.606 * tempC) / 10000.0;
}

long ZlabUltrasonic::_getRawPulseDuration() {
    // 1. Send a 10µs trigger pulse
    digitalWrite(_trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(_trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trigPin, LOW);

    // 2. Measure the echo pulse duration using high-precision timer
    // We avoid the blocking pulseIn() function
    unsigned long startTime = esp_timer_get_time();
    unsigned long timeout = startTime + 30000; // 30ms timeout for safety

    // Wait for the echo pin to go HIGH
    while (digitalRead(_echoPin) == LOW && esp_timer_get_time() < timeout);
    startTime = esp_timer_get_time();

    // Wait for the echo pin to go LOW
    while (digitalRead(_echoPin) == HIGH && esp_timer_get_time() < timeout);
    unsigned long endTime = esp_timer_get_time();
    
    // Check for timeout
    if (endTime >= timeout) return 0;

    return endTime - startTime;
}

float ZlabUltrasonic::getDistance(const char* unit) {
    long duration = _getRawPulseDuration();
    if (duration == 0) return -1.0; // Error condition

    // Distance = (Speed * Time) / 2
    float distance_cm = (duration * _speedOfSound) / 2.0;

    if (strcmp(unit, "inch") == 0) {
        return distance_cm / 2.54;
    } else {
        return distance_cm;
    }
}

bool ZlabUltrasonic::isObjectDetected(float threshold_cm) {
    float currentDistance = getDistance("cm");
    if (currentDistance < 0) return false; // Error, assume no object
    return currentDistance <= threshold_cm;
}

float ZlabUltrasonic::getFilteredDistance() {
    float measurement = getDistance("cm");
    if (measurement < 0) return _kalman_x; // On error, return last known value

    if (!_kalman_initialized) {
        _kalman_x = measurement; // Initialize with the first measurement
        _kalman_initialized = true;
        return _kalman_x;
    }

    // Kalman Filter Steps:
    // 1. Prediction
    float P_pred = _kalman_P + _kalman_Q;

    // 2. Update (Correction)
    float K = P_pred / (P_pred + _kalman_R); // Kalman Gain
    _kalman_x = _kalman_x + K * (measurement - _kalman_x);
    _kalman_P = (1 - K) * P_pred;

    return _kalman_x;
}