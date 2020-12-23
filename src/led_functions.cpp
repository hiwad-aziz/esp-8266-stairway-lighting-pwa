#include "led_functions.h"
#include <Arduino.h>
#include <FastLED.h>

void LedFunctions::setHue(int hue) {
  this->color.H = hue;
  color_changed = true;
}

void LedFunctions::setVal(int val) {
  this->color.L = val;
  color_changed = true;
}

void LedFunctions::setSat(int sat) {
  this->color.S = sat;
  color_changed = true;
}

void LedFunctions::setVelocity(int vel) { this->vel = vel; }

void LedFunctions::resetState() {
  this->sensing_state = eSensingState::WAITING;
}

void LedFunctions::sensingmode() {
  top_sensor_range =
      readSensor(ULTRASONIC_TOP, trigger_state_top, previous_micros_top);
  bottom_sensor_range = readSensor(ULTRASONIC_BOTTOM, trigger_state_bottom,
                                   previous_micros_bottom);
  runTopSensorLogic();
  runBottomSensorLogic();

  if (previous_mode != eLedMode::SENSING)
    previous_mode = eLedMode::SENSING;
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
      (top_sensor_range > RANGE_TRIGGER_MIN) &&
      (sensing_state == eSensingState::WAITING)) {
    Serial.println("Turning on light from top...");
    turnOnLightFromTop();
  }
  if ((bottom_sensor_range <= RANGE_TRIGGER_MAX) &&
      (bottom_sensor_range > RANGE_TRIGGER_MIN) &&
      (sensing_state == eSensingState::TOP_TRIGGERED)) {
    Serial.println("Turning off light from top...");
    turnOffLightFromTop();
  } else if ((millis() - previous_millis) > 35000) {
    Serial.println("Turning off light...");
    turnOffLight();
  }
}

void LedFunctions::runBottomSensorLogic() {
  if ((bottom_sensor_range <= RANGE_TRIGGER_MAX) &&
      (sensing_state == eSensingState::WAITING)) {
    turnOnLightFromBottom();
  }
  if ((top_sensor_range <= RANGE_TRIGGER_MAX) &&
      (sensing_state == eSensingState::BOTTOM_TRIGGERED)) {
    turnOffLightFromBottom();
  } else if ((millis() - previous_millis) > 35000) {
    turnOffLight();
  }
}

void LedFunctions::turnOnLightFromTop() {
  previous_millis = millis();
  sensing_state = eSensingState::TOP_TRIGGERED;
  leds->SetPixelColor(0, color);
  leds->SetPixelColor(1, color);
  leds->SetPixelColor(2, color);
  leds->SetPixelColor(3, color);
  leds->Show();

  for (int j = 1; j < NUM_STEPS; j++) {
    // s. here why yields are necessary:
    // https://arduino-esp8266.readthedocs.io/en/latest/reference.html?highlight=yield#timing-and-delays
    if ((millis() - millis_yield) > 1000) {
      yield();
      millis_yield = millis();
    }
    for (int i = 1; i < 256; i = i + vel) {
      color.L = i / 256.0;
      for (int k = j * NUM_LEDS_PER_STEP; k < NUM_LEDS_PER_STEP * (j + 1);
           k++) {
        leds->SetPixelColor(k, color);
        leds->Show();
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
      color.L = i / 256.0;
      for (int k = j * NUM_LEDS_PER_STEP; k < NUM_LEDS_PER_STEP * (j + 1);
           k++) {
        leds->SetPixelColor(k, color);
        leds->Show();
      }
    }
  }
  HslColor off(0.0, 0.0, 0.0);
  for (int n = 0; n < NUM_LEDS; n++) {
    leds->SetPixelColor(n, off);
    leds->Show();
  }

  sensing_state = eSensingState::WAITING;
}

void LedFunctions::turnOnLightFromBottom() {
  previous_millis = millis();
  sensing_state = eSensingState::BOTTOM_TRIGGERED;
  for (int j = NUM_STEPS; j > 0; j--) {
    if ((millis() - millis_yield) > 1000) {
      yield();
      millis_yield = millis();
    }
    for (int i = 1; i < 256; i = i + vel) {
      color.L = i / 256.0;
      for (int k = ((j * NUM_LEDS_PER_STEP) - 1);
           k > (((j - 1) * NUM_LEDS_PER_STEP) - 1); k--) {
        leds->SetPixelColor(k, color);
        leds->Show();
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
      color.L = i / 256.0;
      for (int k = ((j * NUM_LEDS_PER_STEP) - 1);
           k > (((j - 1) * NUM_LEDS_PER_STEP) - 1); k--) {
        leds->SetPixelColor(k, color);
        leds->Show();
      }
    }
    HslColor off(0.0, 0.0, 0.0);
    for (int n = 0; n < NUM_LEDS; n++) {
      leds->SetPixelColor(n, off);
      leds->Show();
    }

    sensing_state = eSensingState::WAITING;
  }

  HslColor off(0.0, 0.0, 0.0);
  for (int n = NUM_LEDS - 1; n > -1; n--) {
    leds->SetPixelColor(n, off);
    leds->Show();
  }

  sensing_state = eSensingState::WAITING;
}

void LedFunctions::turnOffLight() {
  HslColor off(0.0, 0.0, 0.0);
  for (int n = 0; n < NUM_LEDS; n++) {
    leds->SetPixelColor(n, off);
  }
  leds->Show();
  sensing_state = eSensingState::WAITING;
}

void LedFunctions::steadymode() {
  if ((previous_mode != eLedMode::STEADY) && (color_changed == true)) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds->SetPixelColor(i, color);
    }
    leds->Show();
    previous_mode = eLedMode::STEADY;
    color_changed = false;
  }
}

void LedFunctions::nightmode() {
  if (previous_mode != eLedMode::NIGHT) {
    color.H = 0.0;
    color.S = 0.0;
    color.L = 0.25;
    for (int i = 0; i < NUM_LEDS; i++) {
      leds->SetPixelColor(i, color);
    }
    leds->Show();
    previous_mode = eLedMode::NIGHT;
  }
}

void LedFunctions::rainbowmode() {
  if (previous_mode != eLedMode::RAINBOW) {
    HsbColor rainbow(0.0, 1.0, 0.75);
    for (int i = 0; i < NUM_LEDS; i++) {
      // Calc hue such that each color of the rainbow is represented
      // in the available sum of LEDs
      rainbow.H = i / NUM_LEDS;
      leds->SetPixelColor(i, rainbow);
    }
    leds->Show();
    previous_mode = eLedMode::RAINBOW;
  }
}

void LedFunctions::rainbowmoderunning() {
  byte *c;
  uint16_t i, j;
  RgbColor rgb_color;
  for (j = 0; j < 256;) {
    if ((millis() - millis_yield) > 1000) {
      yield();
      millis_yield = millis();
    }
    if (millis() - previous_millis > vel) {
      for (i = 0; i < NUM_LEDS; i++) {
        c = Wheel(((i * 256 / NUM_LEDS) + j) & 255);
        rgb_color.R = *c;
        rgb_color.G = *(c + 1);
        rgb_color.B = *(c + 2);
        leds->SetPixelColor(i, rgb_color);
      }
      leds->Show();
      j++;
      previous_millis = millis();
    }
  }
}

void LedFunctions::sparkle() {
  int count = 10;
  int speeddelay = 100;
  HslColor off(0.0, 0.0, 0.0);
  for (int n = 0; n < NUM_LEDS; n++) {
    leds->SetPixelColor(n, off);
    leds->Show();
  }
  if (millis() - previous_millis2 > speeddelay) {
    for (int i = 0; i < count; i++) {
      if (millis() - previous_millis1 > speeddelay) {
        RgbColor rgb_color(random(0, 255), random(0, 255), random(0, 255));
        leds->SetPixelColor(random(NUM_LEDS), rgb_color);
        leds->Show();
        previous_millis1 = millis();
      }
    }
  }
  previous_millis2 = millis();
  if (previous_mode != eLedMode::SPARKLE)
    previous_mode = eLedMode::SPARKLE;
}

void LedFunctions::twinkle() {
  int speeddelay = 0;
  HslColor off(0.0, 0.0, 0.0);
  RgbColor rgb_white(255, 255, 255);
  if (millis() - previous_millis > speeddelay) {
    leds->SetPixelColor(random(NUM_LEDS), rgb_white);
    leds->Show();
  }
  for (int n = 0; n < NUM_LEDS; n++) {
    leds->SetPixelColor(n, off);
    leds->Show();
  }
  previous_millis = millis();
  if (previous_mode != eLedMode::TWINKLE)
    previous_mode = eLedMode::TWINKLE;
}

void LedFunctions::fire() {
  if ((millis() - previous_millis) > random(100, 200)) {
    Serial.println(previous_millis);
    HslColor off(0.0, 0.0, 0.0);
    RgbColor rgb_fire;
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
      rgb_fire.R = r1;
      rgb_fire.G = g1;
      rgb_fire.B = b1;
      leds->SetPixelColor(n, off);
    }
    leds->Show();
    previous_millis = millis();
  }

  if (previous_mode != eLedMode::FIRE)
    previous_mode = eLedMode::FIRE;
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