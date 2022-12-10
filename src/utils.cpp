#include "patterns/utils.h"

#include "LEDPatterns.h"

void addGlitter(fract8 chanceOfGlitter) {
    if (random8() < chanceOfGlitter) {
        leds[random16(NUM_LEDS)] += CRGB::White;
    }
}

void addGlitter(fract8 chanceOfGlitter, uint16_t ledStartIndex, uint16_t ledEndIndex) {
    if (random8() < chanceOfGlitter) {
        leds[random16(ledStartIndex, ledEndIndex)] += CRGB::White;
    }
}

void addColorGlitter(fract8 chanceOfGlitter, CRGB color) {
    if (random8() < chanceOfGlitter) {
        leds[random16(NUM_LEDS)] = color;
    }
}

void addColorGlitter(fract8 chanceOfGlitter, CRGB color, uint16_t ledStartIndex, uint16_t ledEndIndex) {
    if (random8() < chanceOfGlitter) {
        leds[random16(ledStartIndex, ledEndIndex)] = color;
    }
}
