#include <FastLED.h>

#include "LEDPatterns.h"
#include "patterns/utils.h"

static uint8_t loopCount = 0;

void retweet() {
    EVERY_N_MILLISECONDS(2) {
        static uint8_t fadeToBlackScale = 3;

        uint8_t pos1 = map(beat8(40, 0), 0, 255, 0, STAR_LEDS - 1);
        leds[STAR_LED_BEGIN + pos1] = CRGB::Yellow;

        for (uint16_t i = STAR_LED_BEGIN; i < STAR_LED_END; ++i) {
            leds[i].nscale8(255 - fadeToBlackScale);
        }
        FastLED.show();
    }

    EVERY_N_MILLIS(100) {
        static int changedLeds = TREE_LEDS * 0.2;

        for (int i = 0; i < changedLeds / 2; i++) {
            uint8_t pos = random8(TREE_LED_BEGIN, TREE_LED_END);
            leds[pos] = CRGB::Yellow;
        }
        for (int i = 0; i < changedLeds / 2; i++) {
            uint8_t pos = random8(TREE_LED_BEGIN, TREE_LED_END);
            leds[pos] = CRGB::Green;
        }
        FastLED.show();
    }

    EVERY_N_MILLIS(2) {
        static uint8_t fadeToBlackScale = 20;
        for (uint16_t i = TREE_LED_BEGIN; i < TREE_LED_END; ++i) {
            leds[i].nscale8(255 - fadeToBlackScale);
        }
        FastLED.show();
    }
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
