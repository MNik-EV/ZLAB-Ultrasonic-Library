/**
 * @file ZlabUltrasonic.h
 * @author Mahdi Nikkhah
 * @brief A robust and easy-to-use library for the HC-SR04 ultrasonic sensor.
 * @version 2.0
 * @date 2025-08-10
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef ZLAB_ULTRASONIC_H
#define ZLAB_ULTRASONIC_H

#include "Arduino.h"
#include <vector>

/**
 * @enum Unit
 * @brief Defines the measurement units for distance.
 */
enum class Unit {
    CM,   ///< Centimeters
    INCH  ///< Inches
};

/**
 * @class ZlabUltrasonic
 * @brief Manages interactions with an HC-SR04 ultrasonic distance sensor.
 * @details This class provides functions to get raw, filtered distance readings,
 * and to detect objects within a specific threshold. It's designed to be
 * accurate and easy to integrate.
 */
class ZlabUltrasonic {
public:
    /**
     * @brief Construct a new Zlab Ultrasonic object.
     * @details This constructor initializes the sensor by setting the trigger and
     * echo pins and configuring their modes. It ensures the sensor is ready
     * for use immediately after object creation.
     * @param trigPin The GPIO pin connected to the sensor's TRIG pin.
     * @param echoPin The GPIO pin connected to the sensor's ECHO pin.
     */
    ZlabUltrasonic(uint8_t trigPin, uint8_t echoPin);

    /**
     * @brief Gets the distance to an object, with a selectable unit.
     * @details This function measures the distance and returns it in the specified
     * unit. If no unit is provided, it defaults to centimeters.
     * @param unit The measurement unit (Unit::CM or Unit::INCH). Defaults to Unit::CM.
     * @return The calculated distance as a float. Returns a negative value on error.
     */
    float getDistance(Unit unit = Unit::CM);

    /**
     * @brief Checks if an object is detected within a given distance threshold.
     * @param threshold_cm The distance threshold in centimeters.
     * @return True if an object is detected at or closer than the threshold, false otherwise.
     */
    bool isObjectDetected(float threshold_cm);

    /**
     * @brief Gets a noise-free distance reading by averaging measurements over 100ms.
     * @details This function is ideal for dynamic environments, like a moving robot.
     * It samples the distance for 100 milliseconds, discards invalid readings,
     * and returns the average of the valid ones for a stable output.
     * @param sample_interval_ms The delay in milliseconds between each sample.
     * @return The filtered distance in centimeters. Returns a negative value on error.
     */
    float getMovingAverageDistance(int sample_interval_ms = 10);

    /**
     * @brief Sets the ambient temperature for more accurate speed of sound calculations.
     * @param tempC The ambient temperature in Celsius.
     */
    void setTemperature(float tempC);

private:
    /**
     * @brief Measures the raw duration of the echo pulse.
     * @return The echo pulse duration in microseconds. Returns 0 on timeout.
     */
    long _getRawPulseDuration();

    uint8_t _trigPin;      ///< GPIO pin for the trigger.
    uint8_t _echoPin;      ///< GPIO pin for the echo.
    float _temperatureC;   ///< Stores the current ambient temperature in Celsius.
};

#endif // ZLAB_ULTRASONIC_H