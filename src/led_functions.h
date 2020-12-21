#ifndef LED_FUNCTIONS_H
#define LED_FUNCTIONS_H

#include "config.h"
#include <Arduino.h>
#include <FastLED.h>

using namespace globalconstants;

class LedFunctions {
private:
  CRGB *leds;
  // timing
  unsigned long millis_yield = 0;
  unsigned long previous_millis = 0;
  unsigned long previous_millis1 = 0;
  unsigned long previous_millis2 = 0;
  unsigned long previous_micros_top = 0;
  unsigned long previous_micros_bottom = 0;
  int state = 0;
  int top_sensor_range = 200;
  int bottom_sensor_range = 200;
  int trigger_state_top = LOW;
  int trigger_state_bottom = LOW;
  // color
  int hue = 0;
  int sat = 0;
  int val = 255;
  int vel = 15;

public:
  explicit LedFunctions(CRGB *leds) : leds(leds) {}
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