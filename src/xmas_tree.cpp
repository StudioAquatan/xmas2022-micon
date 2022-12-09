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

void xmas_colors() {
    set_xmas_tree_star();
    EVERY_N_SECONDS(4) {
        for (int i = TREE_LED_BEGIN; i < TREE_LED_END; i++) {
            uint8_t r = random8(0, 4);
            switch (r) {
                case 0:
                    leds[i] = CRGB::Red;
                    break;
                case 1:
                    leds[i] = CRGB::Blue;
                    break;
                case 2:
                    leds[i] = CRGB::Green;
                    break;
                case 3:
                    leds[i] = CRGB::Orange;
                    break;
                default:
                    break;
            }
        }
    }
    FastLED.delay(1);
    FastLED.show();
}

void xmas_snow() {
    set_xmas_tree_star();
    EVERY_N_SECONDS(4) {
        for (int i = TREE_LED_BEGIN; i < TREE_LED_END; i++) {
            uint8_t r = random8(0, 7);
            if (r) {
                leds[i] = CRGB::White;
            } else {
                leds[i] = CRGB::Green;
            }
        }
        FastLED.delay(1);
        FastLED.show();
    }
}

void xmas_autumn() {
    set_xmas_tree_star();
    EVERY_N_SECONDS(4) {
        for (int i = TREE_LED_BEGIN; i < TREE_LED_END; i++) {
            uint8_t r = random8(0, 5);
            switch (r) {
                case 0:
                case 1:
                    leds[i] = CRGB::Red;
                    break;
                case 2:
                case 3:
                    leds[i] = CRGB::Orange;
                    break;
                case 4:
                    leds[i] = CRGB::Yellow;
                    break;
            }
        }
        FastLED.delay(1);
        FastLED.show();
    }
}
