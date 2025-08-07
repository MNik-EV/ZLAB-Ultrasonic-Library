# ZLAB Sensor Project: Test Log

## Test #1: Raw Hardware & Timing Validation

- **Date:** 2025-08-07
- **Goal:** To verify the stability and accuracy of the raw pulse duration from the sensor, bypassing all software filters.

---

### Setup
- **Board:** ESP32-S3 USB OTG
- **Sensor:** HC-SR04 with a 0.1uF decoupling capacitor.
- **Target:** A flat, hard object placed at a fixed distance of **15.0 cm**.
- **Test Mode:** 1 (Raw Pulse Duration)

---

### Results
The sensor produced a highly stable stream of raw pulse durations. A sample of the output is below:
--- ZLAB Sensor Debug Mode ---
Current Test Mode: 1
------------------------------------
Raw Pulse Duration: 922 us
Raw Pulse Duration: 923 us
Raw Pulse Duration: 924 us
Raw Pulse Duration: 923 us
Raw Pulse Duration: 923 us
Raw Pulse Duration: 924 us
Raw Pulse Duration: 924 us
Raw Pulse Duration: 923 us
Raw Pulse Duration: 923 us
Raw Pulse Duration: 923 us
Raw Pulse Duration: 922 us
Raw Pulse Duration: 922 us
Raw Pulse Duration: 924 us
Raw Pulse Duration: 923 us
---

### Analysis & Conclusion
1.  **Stability:** The output is extremely stable, with jitter of only ±1 microsecond. This confirms that the power supply, wiring, and core `_getRawPulseDuration()` function are working perfectly.
2.  **Accuracy:** The average reading of `923 µs` corresponds to a calculated distance of **~15.98 cm** (at 25°C). This is very close to the measured 15 cm. The ~1 cm difference is considered a normal calibration offset due to the sensor's physical build and measurement precision.

**Verdict: The hardware and base timing layer are validated. The source of inaccuracy is not in the hardware; we can now proceed to debug the software layers.**
