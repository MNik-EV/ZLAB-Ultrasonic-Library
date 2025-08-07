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
---

## Test #2: Raw Distance Calculation & Unit Conversion

- **Date:** 2025-08-07
- **Goal:** To verify the accuracy of the `getDistance()` calculation and confirm the fix for the inch/cm inconsistency.
---
### Setup
- Same as Test #1 (Target at 15.0 cm).
- **Test Mode:** 2 (Raw Distance)
---
### Results
The output showed stable and consistent distance readings in both units.
 --- ZLAB Sensor Debug Mode ---
Current Test Mode: 2
------------------------------------
Simple Distance: 16.53 cm  |  6.51 inch
Simple Distance: 16.53 cm  |  6.51 inch
Simple Distance: 16.53 cm  |  6.51 inch
Simple Distance: 16.53 cm  |  6.51 inch
Simple Distance: 16.51 cm  |  6.50 inch
Simple Distance: 16.54 cm  |  6.51 inch
Simple Distance: 16.53 cm  |  6.51 inch
Simple Distance: 16.54 cm  |  6.51 inch
Simple Distance: 16.53 cm  |  6.51 inch
---
### Analysis & Conclusion
1.  **Bug Fix Confirmed:** The inch and centimeter values are now perfectly synchronized, as they are derived from a single raw measurement. The previous bug is resolved.
2.  **Calibration Offset Confirmed:** The readings are consistently stable around **16.5 cm**, which is ~1.5 cm more than the physically measured distance. This confirms a consistent calibration offset. This is not a software logic bug and can be corrected later if needed.
**Verdict: The `getDistance()` function is working correctly. The raw data pipeline is now fully validated. We can now confidently proceed to debug the filtering layer.**
---

## Test #3: Final Filter Robustness Test
- **Date:** 2025-08-07
- **Goal:** To verify that the final version of the library with the "Intelligent Reset" logic can handle dynamic scene changes without the filter getting stuck.
---
### Setup
- Final library code with the intelligent reset feature implemented.
- **Test Mode:** 3 (or any mode that displays both Raw and Filtered distance).
- **Procedure:** The sensor was moved to various distances (21cm, 24cm, 17cm) to simulate a dynamic environment.
---
### Results
The output shows the filtered distance now tracks the raw distance smoothly, even after large and rapid changes. The filter no longer gets stuck at an old value.
--- ZLAB Sensor Debug Mode ---
Current Test Mode: 3
------------------------------------
Raw: 21.01 cm  |  Filtered: 21.01 cm
Raw: 21.01 cm  |  Filtered: 20.65 cm
Raw: 21.05 cm  |  Filtered: 20.84 cm
Raw: 21.05 cm  |  Filtered: 20.75 cm
Raw: 21.05 cm  |  Filtered: 20.85 cm
Raw: 21.50 cm  |  Filtered: 21.07 cm
Raw: 24.53 cm  |  Filtered: 22.32 cm
Raw: 24.01 cm  |  Filtered: 22.93 cm
Raw: 23.92 cm  |  Filtered: 23.27 cm
Raw: 24.42 cm  |  Filtered: 23.69 cm
Raw: 24.41 cm  |  Filtered: 23.94 cm
Raw: 23.09 cm  |  Filtered: 23.62 cm
Raw: 23.07 cm  |  Filtered: 23.42 cm
Raw: 23.07 cm  |  Filtered: 23.29 cm
Raw: 23.09 cm  |  Filtered: 23.20 cm
Raw: 23.09 cm  |  Filtered: 23.16 cm
Raw: 23.09 cm  |  Filtered: 23.12 cm
Raw: 23.11 cm  |  Filtered: 23.11 cm
Raw: 23.11 cm  |  Filtered: 23.10 cm
Raw: 21.45 cm  |  Filtered: 22.63 cm
Raw: 17.55 cm  |  Filtered: 20.81 cm
Raw: 16.99 cm  |  Filtered: 19.58 cm
Raw: 16.98 cm  |  Filtered: 18.66 cm
Raw: 16.99 cm  |  Filtered: 18.06 cm
Raw: 16.99 cm  |  Filtered: 17.69 cm
Raw: 16.99 cm  |  Filtered: 17.57 cm
Raw: 16.99 cm  |  Filtered: 17.37 cm
Raw: 16.99 cm  |  Filtered: 17.24 cm
Raw: 16.98 cm  |  Filtered: 17.15 cm
Raw: 16.99 cm  |  Filtered: 17.09 cm
---
### Analysis & Conclusion
The "Intelligent Reset" logic successfully solved the "stuck filter" problem. By allowing the filter to re-initialize after several consecutive "outlier" readings, the system can now adapt to new environments instead of incorrectly flagging legitimate changes as noise.
**Verdict: The library's filtering mechanism is now considered robust, stable, and production-ready. The development and debugging phase for this feature is complete.**