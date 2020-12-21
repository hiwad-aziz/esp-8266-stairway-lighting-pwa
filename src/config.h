#ifndef GLOBALCONSTANTS_H
#define GLOBALCONSTANTS_H

namespace globalconstants {
int const NUM_LEDS = 52;
int const NUM_STEPS = 13;
int const NUM_LEDS_PER_STEP = 4;
int const DATA_PIN = 3;        // data pin of led strip (D4)
int const ULTRASONIC_TOP = 14; // pin of ultrasonic sensor at top of stairs (D5)
int const ULTRASONIC_BOTTOM =
    12; // pin of ultrasonic sensor at bottom of stairs (D6)
int const US_RETURN_FACTOR = 2;
int const SPEED_OF_SOUND = 0.03432; // [cm/µs]
int const RANGE_TRIGGER_MAX = 70;
int const RANGE_TRIGGER_MIN = 2;
int const R_FIRE = 255;
int const G_FIRE = R_FIRE - 75;
int const B_FIRE = 40;
} // namespace globalconstants

#endif
