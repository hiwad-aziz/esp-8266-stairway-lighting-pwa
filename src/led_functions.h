#ifndef LED_FUNCTIONS_H
#define LED_FUNCTIONS_H

#include "config.h"
#include <Arduino.h>
#include <NeoPixelBus.h>

using namespace globalconstants;

class LedFunctions {
private:
  // enum definitions
  enum class eLedMode : uint8_t {
    SENSING = 0,
    STEADY,
    NIGHT,
    RAINBOW,
    RUNRAINBOW,
    SPARKLE,
    TWINKLE,
    FIRE
  };

  enum class eSensingState : uint8_t {
    WAITING = 0,
    TOP_TRIGGERED,
    BOTTOM_TRIGGERED
  };

  NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *leds;
  // timing
  unsigned long millis_yield = 0;
  unsigned long previous_millis = 0;
  unsigned long previous_millis1 = 0;
  unsigned long previous_millis2 = 0;
  unsigned long previous_micros_top = 0;
  unsigned long previous_micros_bottom = 0;
  // sensors
  int top_sensor_range = 70;
  int bottom_sensor_range = 70;
  int trigger_state_top = LOW;
  int trigger_state_bottom = LOW;
  // color
  HslColor color;
  bool color_changed = false;
  int vel = 5;
  // states
  eSensingState sensing_state = eSensingState::WAITING;
  eLedMode previous_mode = eLedMode::SENSING;

public:
  explicit LedFunctions(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *leds)
      : leds(leds) {
    color.H = 0.0;
    color.S = 0.0;
    color.L = 0.0;
  }
  void setHue(int hue);
  void setVal(int val);
  void setSat(int sat);
  void setVelocity(int vel);
  void resetState();
  int readSensor(int const pin, int &trigger_state,
                 unsigned long &previous_micros);
  void runTopSensorLogic();
  void runBottomSensorLogic();
  void turnOnLightFromTop();
  void turnOffLightFromTop();
  void turnOnLightFromBottom();
  void turnOffLightFromBottom();
  void turnOffLight();
  void sensingmode();
  void steadymode();
  void nightmode();
  void rainbowmode();
  void rainbowmoderunning();
  void sparkle();
  void twinkle();
  void fire();
  byte *Wheel(byte WheelPos);
};

#endif