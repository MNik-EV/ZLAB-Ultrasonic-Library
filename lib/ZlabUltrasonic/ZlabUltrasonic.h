#ifndef ZLAB_ULTRASONIC_H
#define ZLAB_ULTRASONIC_H

#include "Arduino.h"
#include <vector> 

/**
 * @class ZlabUltrasonic
 * @brief A professional, non-blocking library for HC-SR04 ultrasonic sensors on ESP32.
 * This version includes temperature compensation, a tuned Kalman filter, and outlier rejection.
 */
class ZlabUltrasonic {
public:
    /**
     * @brief Constructor: Sets up the sensor pins.
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
     * @return The distance as a float. Returns a negative value on error.
     */
    float getDistance(const char* unit = "cm");

    /**
     * @brief Checks if an object is within a given threshold.
     * @param threshold_cm The distance threshold in centimeters.
     * @return True if an object is detected within the threshold, false otherwise.
     */
    bool isObjectDetected(float threshold_cm);

    /**
     * @brief Returns a highly stable, noise-filtered distance reading.
     * Uses outlier rejection and a Kalman filter. Ideal for moving applications.
     * @return The filtered distance in centimeters.
     */
    float getFilteredDistance();

private:
    uint8_t _trigPin;
    uint8_t _echoPin;
    float _speedOfSound; // in cm/µs

    /**
     * @brief Performs a raw measurement and returns the pulse duration in microseconds.
     * @return The duration of the echo pulse in microseconds, or 0 on timeout.
     */
    long _getRawPulseDuration();

    // --- Kalman Filter Variables (Tuned) ---
    float _kalman_Q = 0.01;  // Process noise
    float _kalman_R = 0.25;  // Measurement noise (Tuned for better stability)
    float _kalman_P = 1.0;   // Estimation error
    float _kalman_x = 0.0;   // Last estimated value
    bool _kalman_initialized = false;

    // --- Outlier Rejection Variables ---
    std::vector<float> _lastReadings;
    const int READING_WINDOW_SIZE = 5; // Use last 5 readings for average
};

#endif // ZLAB_ULTRASONIC_H