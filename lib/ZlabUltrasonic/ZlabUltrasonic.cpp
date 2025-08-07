#include "ZlabUltrasonic.h"
#include "esp32-hal-timer.h" // For the high-precision esp_timer_get_time()

// Constructor
ZlabUltrasonic::ZlabUltrasonic(uint8_t trigPin, uint8_t echoPin) {
    _trigPin = trigPin;
    _echoPin = echoPin;

    pinMode(_trigPin, OUTPUT);
    pinMode(_echoPin, INPUT);

    setTemperature(20.0);
}

void ZlabUltrasonic::setTemperature(float tempC) {
    // Speed of sound formula: 331.3 + 0.606 * Temp(C) m/s
    _speedOfSound = (331.3 + 0.606 * tempC) / 10000.0; // Converted to cm/µs
}

long ZlabUltrasonic::_getRawPulseDuration() {
    digitalWrite(_trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(_trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trigPin, LOW);

    unsigned long startTime_us = esp_timer_get_time();
    unsigned long timeout_us = startTime_us + 30000; // 30ms timeout

    while (digitalRead(_echoPin) == LOW && esp_timer_get_time() < timeout_us);
    if (esp_timer_get_time() >= timeout_us) return 0;
    
    startTime_us = esp_timer_get_time();

    while (digitalRead(_echoPin) == HIGH && esp_timer_get_time() < timeout_us);
    if (esp_timer_get_time() >= timeout_us) return 0;

    unsigned long endTime_us = esp_timer_get_time();
    
    return endTime_us - startTime_us;
}

float ZlabUltrasonic::getDistance(const char* unit) {
    long duration = _getRawPulseDuration();
    if (duration == 0) {
        return -1.0; // Error condition
    }

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
        return false;
    }
    return currentDistance <= threshold_cm;
}

float ZlabUltrasonic::getFilteredDistance() {
    float measurement = getDistance("cm");

    // 1. Range Gate: Reject readings outside the sensor's valid physical range.
    if (measurement < 2.0 || measurement > 400.0) {
        return _kalman_initialized ? _kalman_x : -1.0; 
    }

    // 2. Outlier Rejection & Intelligent Reset Logic
    if (_kalman_initialized && _lastReadings.size() >= READING_WINDOW_SIZE) {
        float sum = 0;
        for (float r : _lastReadings) {
            sum += r;
        }
        float avg = sum / _lastReadings.size();

        if (abs(measurement - avg) > 15.0) { // Threshold for outlier detection
            _consecutive_rejections++;

            // If we get 3 consecutive rejections, assume the scene has changed.
            if (_consecutive_rejections >= 3) {
                _kalman_initialized = false; // Force the filter to reset
                _lastReadings.clear();       // Clear the history
                _consecutive_rejections = 0; // Reset the counter
            } else {
                return _kalman_x; // It might be a temporary outlier, so ignore it.
            }
        } else {
            // If the measurement is good, reset the rejection counter.
            _consecutive_rejections = 0;
        }
        _lastReadings.erase(_lastReadings.begin());
    }
    _lastReadings.push_back(measurement);

    // 3. Filtering Stage
    if (!_kalman_initialized) {
        // (Re)Initialize the filter with the new baseline measurement.
        _kalman_x = measurement;
        _kalman_initialized = true;
        // Also reset the reading window with this new baseline
        _lastReadings.assign(READING_WINDOW_SIZE, measurement); 
        return _kalman_x;
    }
    
    // Standard Kalman Filter Update
    float P_pred = _kalman_P + _kalman_Q;
    float K = P_pred / (P_pred + _kalman_R); 
    _kalman_x = _kalman_x + K * (measurement - _kalman_x); 
    _kalman_P = (1 - K) * P_pred; 

    return _kalman_x;
}