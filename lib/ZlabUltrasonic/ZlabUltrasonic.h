#ifndef ZLAB_ULTRASONIC_H
#define ZLAB_ULTRASONIC_H

#include "Arduino.h"
#include <vector> // Required for the outlier rejection logic

/**
 * @class ZlabUltrasonic
 * @brief A professional, robust library for HC-SR04 ultrasonic sensors on ESP32.
 * @details This version includes temperature compensation, a tuned Kalman filter, 
 * and an intelligent reset mechanism to handle dynamic environments.
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
     * @brief Measures and returns the raw, unfiltered distance.
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
     * @return The filtered distance in centimeters.
     */
    float getFilteredDistance();

private:
    /**
     * @brief Performs a raw measurement and returns the pulse duration.
     * @return The duration of the echo pulse in microseconds, or 0 on timeout.
     */
    long _getRawPulseDuration();
    
    uint8_t _trigPin;
    uint8_t _echoPin;
    float _speedOfSound; // in cm/µs

    // --- Kalman Filter Variables (Tuned) ---
    float _kalman_Q = 0.01;
    float _kalman_R = 0.05;
    float _kalman_P = 1.0;
    float _kalman_x = 0.0;
    bool _kalman_initialized = false;

    // --- Outlier Rejection Variables ---
    std::vector<float> _lastReadings;
    const int READING_WINDOW_SIZE = 5;
    int _consecutive_rejections = 0;
};

#endif // ZLAB_ULTRASONIC_H