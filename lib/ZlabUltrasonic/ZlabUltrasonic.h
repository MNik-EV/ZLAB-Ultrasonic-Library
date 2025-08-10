#ifndef ZLAB_ULTRASONIC_H
#define ZLAB_ULTRASONIC_H

#include "Arduino.h"
#include <vector>

// Enum for type-safe unit selection
enum class Unit { CM, INCH };

// Enum for detailed sensor status reporting
enum class Status {
    OK,
    TIMEOUT_ERROR, // Sensor did not receive an echo pulse in time
    OUT_OF_RANGE   // Measurement is outside the sensor's physical limits
};

class ZlabUltrasonic {
public:
    ZlabUltrasonic(uint8_t trigPin, uint8_t echoPin);

    // --- Configuration Methods ---
    void setTemperature(float tempC);
    void setKalmanParameters(float Q, float R);
    void setOutlierRejection(int windowSize, float threshold);

    // --- Core Measurement Methods ---
    float getDistance(Unit unit = Unit::CM);
    float getFilteredDistance();
    bool isObjectDetected(float threshold_cm);

    // --- Status & Debug Methods ---
    Status getStatus() const;
    long getRawPulseDuration(); // Now public for debugging

private:
    long _getRawPulseDuration_internal();
    
    // Pins & Physics
    uint8_t _trigPin;
    uint8_t _echoPin;
    float _speedOfSound; // in cm/µs
    Status _currentStatus;

    // Kalman Filter Variables
    float _kalman_Q = 0.01;
    float _kalman_R = 0.05;
    float _kalman_P = 1.0;
    float _kalman_x = 0.0;
    bool _kalman_initialized = false;

    // Outlier Rejection Variables
    std::vector<float> _lastReadings;
    int _reading_window_size = 5;
    float _outlier_threshold = 15.0; // in cm
    int _consecutive_rejections = 0;
};

#endif // ZLAB_ULTRASONIC_H