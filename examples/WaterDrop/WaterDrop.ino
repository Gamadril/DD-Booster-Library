#include <DDBooster.h>

// create booster object
DDBooster booster;

// we have 20 leds
int led_count = 20;
// we start outside from -8 to 0. first led has always index 0
// both values define the length of the drop
int start = -8, end = 0;
// show a gradient from white to blue
byte startColor[] = {60, 60, 60};
byte endColor[] = {0, 0, 60};

void setup() {
  // tell the booster about the number of leds
  booster.init(led_count);
}

void loop() {
  // turn all leds off
  booster.clearAll();
  // set gradient with current values
  booster.setGradient(start, end, startColor, endColor);
  // commit it
  booster.show();

  // when the start index runs out of visible area (last led has index led_count - 1)
  // start from the beginning, otherwise increment the start and end postitions
  if (start == led_count) {
    start = -8;
    end = 0;
    // delay between the drops
    delay(1000);
  } else {
    start++;
    end++;
  }

  // changing the value makes the drop fall faster or slower
  delay(20);
}
