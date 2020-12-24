# ESP8266 WS2812B Stairs Lighting

A private project that lights up my stairs with WS2812B LEDs. Controllable via an PWA and running on an ESP8266.

## Parts

 - NodeMcu V3
 - 2 HC-SR04 ultrasonic sensors (modded to work with 3 pins only, s. [here](https://www.cascologix.com/2015/09/19/hc-sr04-ping-sensor-hardware-mod/))
 - WS2812B led strip
 - Extra PSU to power the led strip

## Detailed instructions
### Software Setup
Upload the data folder to your ESP8266 with SPIFFS.
Build and upload the source code (e.g. with PlatformIO) to your ESP8266.
Configuration parameters can be set in config.h.
### Hardware Setup
TBD

## Modes
I've implemented several modes to light up the leds:

 - Sensing mode: turns the light on if the ultrasonic sensor returns a distance that is in a pre-configured range and turns it off once the other ultrasonic sensor does the same (e.g. lights on once a person steps on the first step and lights off once he/she steps on the last step of a stairway)
 - Steady mode: turns light on with configurable color
 - Night mode: dimmed white light
 - Rainbow effect: rainbow colors starting from top going to bottom
 - Running rainbow effect: rainbow colors moving up
 - Sparkle effect
 - Twinkle effect
 - Fire effect

All these modes can be turned on/off by looking up your ESP8266's IP in a web browser.
