#include <DDBooster.h>

// create booster object
DDBooster booster;

// we have 20 leds
int led_count = 20;
// we start with the first led
int index = 0;
// this color will be used for the turn lights
byte color[] = {0xFF, 0xBB, 0};

void setup() {
  // tell the booster about the number of leds
  booster.init(led_count);
  // turn all leds off to have a clear state
  booster.clearAll();
  booster.show();
  // save the color we want to use in the color buffer
  booster.setRGB(color[0], color[1], color[2]);
}

void loop() {
  // we always set one led per loop run
  booster.setLED(index);
  booster.show();

  // if we arrive the last led, reset all and wait a little bit
  // if not increment index to go set next led on next loop run
  if (index == led_count - 1) {
    index = 0;
    booster.clearAll();
    booster.show();
    delay(500);
    // set the color back
    booster.setRGB(color[0], color[1], color[2]);
  } else {
    index++;
  }

  // control the speed of the turn light
  delay(20);
}
