#ifndef ZLAB_ULTRASONIC_H
#define ZLAB_ULTRASONIC_H

#include "Arduino.h"

// Doxygen 
/**
 * @class ZlabUltrasonic
 * @brief A professional, non-blocking library for HC-SR04 ultrasonic sensors on ESP32.
 * This library uses low-level ESP32 functions for high precision and includes
 * temperature compensation and advanced filtering.
 */
class ZlabUltrasonic {
public:
    /**
     * @brief Constructor Option 1: Simple setup with only pin numbers.
     * @param trigPin The GPIO pin connected to the sensor's Trig pin.
     * @param echoPin The GPIO pin connected to the sensor's Echo pin.
     */
    ZlabUltrasonic(uint8_t trigPin, uint8_t echoPin);

    /**
     * @brief Sets the ambient temperature to compensate for speed of sound variations.
     * @param tempC Temperature in Celsius. Default is 20°C.
     */
    void setTemperature(float tempC);

    /**
     * @brief Measures distance and returns the value in the specified unit.
     * @param unit "cm" for centimeters or "inch" for inches.
     * @return The distance as a float. Returns -1.0 on error.
     */
    float getDistance(const char* unit = "cm");

    /**
     * @brief Checks if an object is within a given threshold.
     * @param threshold_cm The distance threshold in centimeters.
     * @return True if an object is detected within the threshold, false otherwise.
     */
    bool isObjectDetected(float threshold_cm);

    /**
     * @brief Returns a noise-filtered distance reading using a Kalman filter.
     * Ideal for moving applications like robotics.
     * @return The filtered distance in centimeters.
     */
    float getFilteredDistance();

private:
    uint8_t _trigPin;
    uint8_t _echoPin;
    float _speedOfSound; // in cm/µs

    /**
     * @brief Performs a raw measurement and returns the pulse duration in microseconds.
     * @return The duration of the echo pulse in microseconds.
     */
    long _getRawPulseDuration();

    // --- Kalman Filter Variables ---
    float _kalman_Q = 0.01; // Process noise
    float _kalman_R = 0.1;  // Measurement noise
    float _kalman_P = 1.0;  // Estimation error
    float _kalman_x = 0.0;  // Initial value
    bool _kalman_initialized = false;
};

#endif // ZLAB_ULTRASONIC_H