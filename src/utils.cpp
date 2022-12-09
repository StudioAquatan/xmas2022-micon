#include "patterns/utils.h"

#include "LEDPatterns.h"

void addGlitter(fract8 chanceOfGlitter) {
    if (random8() < chanceOfGlitter) {
        leds[random16(NUM_LEDS)] += CRGB::White;
    }
}

void addColorGlitter(fract8 chanceOfGlitter, CRGB color) {
    if (random8() < chanceOfGlitter) {
        leds[random16(NUM_LEDS)] = color;
    }
}
