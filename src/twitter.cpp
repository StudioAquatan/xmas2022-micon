#include <FastLED.h>

#include "LEDPatterns.h"
#include "patterns/utils.h"

static uint8_t loopCount = 0;

void retweet() {
    uint8_t pos1 = map(beat8(40, 0), 0, 255, 0, NUM_LEDS - 1);
    uint8_t pos2 = map(beat8(40, 333), 0, 255, 0, NUM_LEDS - 1);
    uint8_t pos3 = map(beat8(40, 666), 0, 255, 0, NUM_LEDS - 1);
    leds[pos1] = CRGB::Green;
    leds[pos2] = CRGB::Yellow;
    leds[pos3] = CRGB::Green;

    fadeToBlackBy(leds, NUM_LEDS, 3);

    FastLED.delay(2);
    FastLED.show();
}

void heart() {
    uint8_t sinBeat1 = beatsin8(30, 0, NUM_LEDS - 1, 0, 0);
    uint8_t sinBeat2 = beatsin8(30, 0, NUM_LEDS - 1, 0, 127);

    leds[sinBeat1] = CRGB::DeepPink;
    leds[sinBeat2] = CRGB::DeepPink;

    fadeToBlackBy(leds, NUM_LEDS, 5);

    EVERY_N_MILLISECONDS(10) {
        addColorGlitter(30, CRGB::Blue);
        addColorGlitter(30, CRGB::Green);
        addColorGlitter(30, CRGB::Orange);
        addColorGlitter(30, CRGB::Purple);
    }

    FastLED.delay(1);
    FastLED.show();
}

CRGBPalette16 purplePalette = CRGBPalette16(
    CRGB::DarkViolet,
    CRGB::DarkViolet,
    CRGB::DarkViolet,
    CRGB::DarkViolet,

    CRGB::Magenta,
    CRGB::Magenta,
    CRGB::Linen,
    CRGB::Linen,

    CRGB::Magenta,
    CRGB::Magenta,
    CRGB::DarkViolet,
    CRGB::DarkViolet,

    CRGB::DarkViolet,
    CRGB::DarkViolet,
    CRGB::Linen,
    CRGB::Linen);

void hashtag() {
    fill_palette(leds, NUM_LEDS, loopCount, 255 / NUM_LEDS, purplePalette, 255, LINEARBLEND);

    EVERY_N_MILLISECONDS(10) {
        loopCount++;
    }
}
