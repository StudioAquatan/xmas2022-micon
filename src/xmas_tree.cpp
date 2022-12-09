#include <FastLED.h>

#include "LEDPatterns.h"

void set_xmas_tree_star() {
    for (int i = STAR_LED_BEGIN; i < STAR_LED_END; i++) {
        leds[i] = CRGB::Yellow;
    }
}

void set_xmas_tree_body() {
    for (int i = TREE_LED_BEGIN; i < TREE_LED_END; i++) {
        leds[i] = CRGB::Green;
    }
}

void xmas_tree() {
    set_xmas_tree_star();
    set_xmas_tree_body();
    FastLED.delay(1);
}
