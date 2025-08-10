#include "ZlabUltrasonic.h"
#include "esp32-hal-timer.h"

ZlabUltrasonic::ZlabUltrasonic(uint8_t trigPin, uint8_t echoPin) {
    _trigPin = trigPin;
    _echoPin = echoPin;

    pinMode(_trigPin, OUTPUT);
    pinMode(_echoPin, INPUT);

    setTemperature(20.0); // Default temperature
    _currentStatus = Status::OK;
}

void ZlabUltrasonic::setTemperature(float tempC) {
    _speedOfSound = (331.3 + 0.606 * tempC) / 10000.0; // cm/µs
}

void ZlabUltrasonic::setKalmanParameters(float Q, float R) {
    _kalman_Q = Q;
    _kalman_R = R;
}

void ZlabUltrasonic::setOutlierRejection(int windowSize, float threshold) {
    _reading_window_size = windowSize;
    _outlier_threshold = threshold;
}

long ZlabUltrasonic::_getRawPulseDuration_internal() {
    digitalWrite(_trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(_trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trigPin, LOW);

    unsigned long startTime_us = esp_timer_get_time();
    // 30ms timeout, corresponds to ~510cm, beyond sensor's max range
    unsigned long timeout_us = startTime_us + 30000; 

    while (digitalRead(_echoPin) == LOW) {
        if (esp_timer_get_time() >= timeout_us) {
            _currentStatus = Status::TIMEOUT_ERROR;
            return 0;
        }
    }
    startTime_us = esp_timer_get_time();

    while (digitalRead(_echoPin) == HIGH) {
        if (esp_timer_get_time() >= timeout_us) {
            _currentStatus = Status::TIMEOUT_ERROR;
            return 0;
        }
    }
    
    return esp_timer_get_time() - startTime_us;
}

long ZlabUltrasonic::getRawPulseDuration() {
    return _getRawPulseDuration_internal();
}

float ZlabUltrasonic::getDistance(Unit unit) {
    long duration = _getRawPulseDuration_internal();
    if (duration == 0) {
        return -1.0f; // Return negative on error
    }

    float distance_cm = (duration * _speedOfSound) / 2.0;

    // Range check
    if (distance_cm < 2.0 || distance_cm > 400.0) {
        _currentStatus = Status::OUT_OF_RANGE;
    } else {
        _currentStatus = Status::OK;
    }

    if (unit == Unit::INCH) {
        return distance_cm / 2.54;
    }
    return distance_cm;
}

bool ZlabUltrasonic::isObjectDetected(float threshold_cm) {
    float currentDistance = getDistance(Unit::CM);
    if (getStatus() != Status::OK) {
        return false;
    }
    return currentDistance <= threshold_cm;
}

float ZlabUltrasonic::getFilteredDistance() {
    float measurement = getDistance(Unit::CM);

    if (getStatus() == Status::TIMEOUT_ERROR) {
        return _kalman_initialized ? _kalman_x : -1.0f;
    }
    
    if (getStatus() == Status::OUT_OF_RANGE) {
        // If out of range, don't update the filter with this invalid value.
        // Return the last known stable value.
        return _kalman_initialized ? _kalman_x : -1.0f;
    }

    // Outlier Rejection & Intelligent Reset Logic
    if (_kalman_initialized && _lastReadings.size() >= _reading_window_size) {
        float sum = 0;
        for (float r : _lastReadings) sum += r;
        float avg = sum / _lastReadings.size();

        if (abs(measurement - avg) > _outlier_threshold) {
            _consecutive_rejections++;
            if (_consecutive_rejections >= 3) {
                _kalman_initialized = false;
                _lastReadings.clear();
                _consecutive_rejections = 0;
            } else {
                return _kalman_x; // Ignore temporary outlier
            }
        } else {
            _consecutive_rejections = 0; // Reset counter on good reading
        }
        _lastReadings.erase(_lastReadings.begin());
    }
    _lastReadings.push_back(measurement);

    // (Re)Initialize or Update Kalman Filter
    if (!_kalman_initialized) {
        _kalman_x = measurement;
        _kalman_P = 1.0;
        _kalman_initialized = true;
        _lastReadings.assign(_reading_window_size, measurement);
        return _kalman_x;
    }
    
    // Standard Kalman Filter Update
    // Q: Process noise covariance. How much we trust the process model.
    //    A smaller Q means the sensor is assumed to be more stable.
    // R: Measurement noise covariance. How much we trust the current measurement.
    //    A smaller R means the raw readings are considered more reliable.
    float P_pred = _kalman_P + _kalman_Q;
    float K = P_pred / (P_pred + _kalman_R); // Kalman Gain
    _kalman_x = _kalman_x + K * (measurement - _kalman_x);
    _kalman_P = (1 - K) * P_pred;

    return _kalman_x;
}

Status ZlabUltrasonic::getStatus() const {
    return _currentStatus;
}