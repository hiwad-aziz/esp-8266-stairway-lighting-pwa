#include "led_functions.h"
#include <Arduino.h>
#include <FastLED.h>

void LedFunctions::setHue(int hue) { this->hue = hue; }

void LedFunctions::setVal(int val) { this->val = val; }

void LedFunctions::setSat(int sat) { this->sat = sat; }

void LedFunctions::setVelocity(int vel) { this->vel = vel; }

void LedFunctions::resetState() { this->state = 0; }

void LedFunctions::sensingmode() {
  top_sensor_range =
      readSensor(ULTRASONIC_TOP, trigger_state_top, previous_micros_top);
  bottom_sensor_range = readSensor(ULTRASONIC_BOTTOM, trigger_state_bottom,
                                   previous_micros_bottom);
  runTopSensorLogic();
  runBottomSensorLogic();
}

int LedFunctions::readSensor(int const pin, int &trigger_state,
                             unsigned long &previous_micros) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin, LOW);

  pinMode(pin, INPUT);
  float distance_in_cm = pulseIn(pin, HIGH, (unsigned long)12000) / 29 / 2;

  Serial.println("Distance: ");
  Serial.println(distance_in_cm);
  Serial.println("\n");
  return (int)distance_in_cm;
}

void LedFunctions::runTopSensorLogic() {
  if ((top_sensor_range <= RANGE_TRIGGER_MAX) &&
      (top_sensor_range > RANGE_TRIGGER_MIN) && (state == 0)) {
    Serial.println("Turning on light from top...");
    turnOnLightFromTop();
  }
  if ((bottom_sensor_range <= RANGE_TRIGGER_MAX) &&
      (bottom_sensor_range > RANGE_TRIGGER_MIN) && (state == 1)) {
    Serial.println("Turning off light from top...");
    turnOffLightFromTop();
  } else if ((millis() - previous_millis) > 35000) {
    Serial.println("Turning off light...");
    turnOffLight();
  }
}

void LedFunctions::runBottomSensorLogic() {
  if ((bottom_sensor_range <= RANGE_TRIGGER_MAX) && (state == 0)) {
    turnOnLightFromBottom();
  }
  if ((top_sensor_range <= RANGE_TRIGGER_MAX) && (state == 2)) {
    turnOffLightFromBottom();
  } else if ((millis() - previous_millis) > 35000) {
    turnOffLight();
  }
}

void LedFunctions::turnOnLightFromTop() {
  previous_millis = millis();
  state = 1;
  leds[0].setHSV(hue, sat, 255);
  leds[1].setHSV(hue, sat, 255);
  leds[2].setHSV(hue, sat, 255);
  leds[3].setHSV(hue, sat, 255);

  for (int j = 1; j < NUM_STEPS; j++) {
    // s. here why yields are necessary:
    // https://arduino-esp8266.readthedocs.io/en/latest/reference.html?highlight=yield#timing-and-delays
    if ((millis() - millis_yield) > 1000) {
      yield();
      millis_yield = millis();
    }
    for (int i = 1; i < 256; i = i + vel) {
      for (int k = j * NUM_LEDS_PER_STEP; k < NUM_LEDS_PER_STEP * (j + 1);
           k++) {
        leds[k].setHSV(hue, sat, i);
        FastLED.show();
      }
    }
  }
}

void LedFunctions::turnOffLightFromTop() {
  for (int j = 0; j < NUM_STEPS; j++) {
    if ((millis() - millis_yield) > 1000) {
      yield();
      millis_yield = millis();
    }
    for (int i = 255; i > -1; i = i - vel) {
      for (int k = j * NUM_LEDS_PER_STEP; k < NUM_LEDS_PER_STEP * (j + 1);
           k++) {
        leds[k].setHSV(hue, sat, i);
        FastLED.show();
      }
    }
  }

  for (int n = 0; n < NUM_LEDS; n++) {
    leds[n] = CRGB::Black;
    FastLED.show();
  }

  state = 0;
}

void LedFunctions::turnOnLightFromBottom() {
  previous_millis = millis();
  state = 2;
  for (int j = NUM_STEPS; j > 0; j--) {
    if ((millis() - millis_yield) > 1000) {
      yield();
      millis_yield = millis();
    }
    for (int i = 1; i < 256; i = i + vel) {
      for (int k = ((j * NUM_LEDS_PER_STEP) - 1);
           k > (((j - 1) * NUM_LEDS_PER_STEP) - 1); k--) {
        leds[k].setHSV(hue, sat, i);
        FastLED.show();
      }
    }
  }
}

void LedFunctions::turnOffLightFromBottom() {
  for (int j = NUM_STEPS; j > 0; j--) {
    if ((millis() - millis_yield) > 1000) {
      yield();
      millis_yield = millis();
    }
    for (int i = 255; i > -1; i = i - vel) {
      for (int k = ((j * NUM_LEDS_PER_STEP) - 1);
           k > (((j - 1) * NUM_LEDS_PER_STEP) - 1); k--) {
        leds[k].setHSV(hue, sat, i);
        FastLED.show();
      }
    }
  }

  for (int n = NUM_LEDS - 1; n > -1; n--) {
    leds[n] = CRGB::Black;
    FastLED.show();
  }

  state = 0;
}

void LedFunctions::turnOffLight() {
  for (int n = 0; n < NUM_LEDS; n++) {
    leds[n] = CRGB::Black;
    FastLED.show();
  }
  state = 0;
}

void LedFunctions::steadymode() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].setHSV(hue, sat, val);
  }
  FastLED.show();
}

void LedFunctions::nightmode() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].setHSV(0, 0, 64);
  }
  FastLED.show();
}

void LedFunctions::rainbowmode() {
  uint8_t thishue = 0;
  uint8_t deltahue = 3;
  thishue = thishue + 1;
  fill_rainbow(leds, NUM_LEDS, thishue, deltahue);
  FastLED.show();
  FastLED.delay(100);
}

void LedFunctions::rainbowmoderunning() {
  byte *c;
  uint16_t i, j;
  for (j = 0; j < 256;) {
    if ((millis() - millis_yield) > 1000) {
      yield();
      millis_yield = millis();
    }
    if (millis() - previous_millis > vel) {
      for (i = 0; i < NUM_LEDS; i++) {
        c = Wheel(((i * 256 / NUM_LEDS) + j) & 255);
        leds[i].setRGB(*c, *(c + 1), *(c + 2));
      }
      FastLED.show();
      j++;
      previous_millis = millis();
    }
  }
}

void LedFunctions::sparkle() {
  int count = 10;
  int speeddelay = 100;
  for (int n = 0; n < NUM_LEDS; n++) {
    leds[n] = CRGB::Black;
    FastLED.show();
  }
  if (millis() - previous_millis2 > speeddelay) {
    for (int i = 0; i < count; i++) {
      if (millis() - previous_millis1 > speeddelay) {
        leds[random(NUM_LEDS)].setRGB(random(0, 255), random(0, 255),
                                      random(0, 255));
        FastLED.show();
        previous_millis1 = millis();
      }
    }
  }
  previous_millis2 = millis();
}

void LedFunctions::twinkle() {
  int speeddelay = 0;
  if (millis() - previous_millis > speeddelay) {
    leds[random(NUM_LEDS)].setRGB(255, 255, 255);
    FastLED.show();
  }
  for (int n = 0; n < NUM_LEDS; n++) {
    leds[n] = CRGB::Black;
    FastLED.show();
  }
  previous_millis = millis();
}

void LedFunctions::fire() {
  if ((millis() - previous_millis) > random(100, 200)) {
    Serial.println(previous_millis);
    for (int n = 0; n < NUM_LEDS; n++) {
      int flicker = random(0, 150);
      int r1 = R_FIRE - flicker;
      int g1 = G_FIRE - flicker;
      int b1 = B_FIRE - flicker;
      if (g1 < 0)
        g1 = 0;
      if (r1 < 0)
        r1 = 0;
      if (b1 < 0)
        b1 = 0;
      leds[n].setRGB(r1, g1, b1);
    }
    FastLED.show();
    previous_millis = millis();
  }
}

byte *LedFunctions::Wheel(byte WheelPos) {
  static byte c[3];
  if (WheelPos < 85) {
    c[0] = WheelPos * 3;
    c[1] = 255 - WheelPos * 3;
    c[2] = 0;
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    c[0] = 255 - WheelPos * 3;
    c[1] = 0;
    c[2] = WheelPos * 3;
  } else {
    WheelPos -= 170;
    c[0] = 0;
    c[1] = WheelPos * 3;
    c[2] = 255 - WheelPos * 3;
  }
  return c;
}