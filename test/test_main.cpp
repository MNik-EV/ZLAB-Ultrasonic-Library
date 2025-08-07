// test/test_main.cpp
#include <AUnit.h>
#include "ZlabUltrasonic.h"

// We can't test hardware directly, so we mock it or test logic.
// Here, we can test the logic of unit conversion and temperature compensation.

test(UnitConversion) {
    // This is a conceptual test. In reality you'd need to mock the hardware reads.
    // Let's assume our calculation logic is what we are testing.
    float speedOfSoundAt20C = (331.3 + 0.606 * 20.0) / 10000.0;
    long dummyDuration = 588; // Corresponds to ~10cm at 20C
    float expected_cm = (dummyDuration * speedOfSoundAt20C) / 2.0;
    float expected_inch = expected_cm / 2.54;

    // We can't call getDistance directly, but we can check the math.
    assertEqual(String(expected_cm, 2), "10.00");
    assertEqual(String(expected_inch, 2), "3.94");
}

test(TemperatureCompensation) {
    float speedOfSoundAt0C = 331.3 / 10000.0;
    float speedOfSoundAt35C = (331.3 + 0.606 * 35.0) / 10000.0;

    assertTrue(speedOfSoundAt35C > speedOfSoundAt0C);
}

void setup() {
    Serial.begin(115200);
    while (!Serial); // wait for serial port to connect
    TestRunner::run();
}

void loop() {}