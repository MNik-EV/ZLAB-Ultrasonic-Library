# Robust Ultrasonic Sensor Library for ESP32

A professional, robust C++ library for the HC-SR04 ultrasonic distance sensor, optimized for the ESP32 platform. This library is designed for high accuracy and stability, especially in dynamic environments where noise and sudden changes are common.

This project was developed as part of the Z-Lab scientific qualification assessment, with a focus on clean code, advanced filtering techniques, and a systematic debugging process.

---

## ✨ Key Features

* **High-Precision Timing:** Uses the ESP32's high-resolution hardware timer (`esp_timer_get_time()`) instead of the standard, blocking `pulseIn()` for more accurate and non-disruptive measurements.
* **Advanced Noise Reduction:** Implements a **Kalman Filter** to provide a smoothed, responsive, and highly stable distance reading, minimizing sensor noise.
* **Intelligent Outlier Rejection:** Features a smart filtering mechanism that detects and rejects random outlier readings. It can distinguish between actual noise and a legitimate, large change in distance, preventing the filter from getting "stuck."
* **Temperature Compensation:** Includes a method to set the ambient temperature, which adjusts the speed of sound calculation for higher absolute accuracy in different conditions.
* **Clean, Object-Oriented Design:** Written in C++ with a clear, modular, and well-documented class structure, making it easy to integrate and extend.

---

## 🛠️ Hardware Requirements

* An ESP32-based development board (e.g., ESP32-DevKitC, ESP32-S3).
* HC-SR04 Ultrasonic Sensor.
* **(Recommended)** A 0.1µF (100nF) ceramic capacitor to place across the sensor's VCC and GND pins for power supply decoupling.
* Jumper wires.

---

## 🚀 Getting Started

This project is built using **PlatformIO**.

1.  **Clone the repository:**
    ```bash
    git clone [https://github.com/YourUsername/YourRepositoryName.git](https://github.com/YourUsername/YourRepositoryName.git)
    ```
2.  **Open in VS Code:** Open the cloned folder in Visual Studio Code with the PlatformIO extension installed.
3.  **Build & Upload:** PlatformIO will automatically handle dependencies. Use the PlatformIO controls to build and upload the project to your ESP32.

---

## ⚙️ How to Use (API)

Here is a basic example of how to use the library in your `main.cpp`.

```cpp
#include <Arduino.h>
#include "ZlabUltrasonic.h" // Include the library

// Define the pins connected to the sensor
#define TRIG_PIN 26
#define ECHO_PIN 25

// Create an instance of the sensor library
ZlabUltrasonic mySensor(TRIG_PIN, ECHO_PIN);

void setup() {
    Serial.begin(115200);
    
    // For best accuracy, set the current ambient temperature
    mySensor.setTemperature(25.0); // Set to 25°C
}

void loop() {
    // Get the simple, raw distance reading
    float rawDistance = mySensor.getDistance();
    
    // Get the highly stable, filtered distance reading
    float filteredDistance = mySensor.getFilteredDistance();

    Serial.print("Raw Distance: ");
    Serial.print(rawDistance, 2);
    Serial.print(" cm   |   Filtered Distance: ");
    Serial.print(filteredDistance, 2);
    Serial.println(" cm");

    delay(500);
}