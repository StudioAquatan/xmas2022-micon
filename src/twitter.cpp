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
    EVERY_N_MILLISECONDS(2) {
        static uint8_t fadeToBlackScale = 3;

        uint8_t sinBeat1 = beatsin8(30, 0, STAR_LEDS - 1, 0, 0);
        leds[STAR_LED_BEGIN + sinBeat1] = CRGB::DeepPink;

        for (uint16_t i = STAR_LED_BEGIN; i < STAR_LED_END; ++i) {
            leds[i].nscale8(255 - fadeToBlackScale);
        }
        FastLED.show();
    }

    EVERY_N_MILLISECONDS(10) {
        addColorGlitter(30, CRGB::Blue, STAR_LED_BEGIN, STAR_LED_END);
        addColorGlitter(30, CRGB::Green, STAR_LED_BEGIN, STAR_LED_END);
        addColorGlitter(30, CRGB::Orange, STAR_LED_BEGIN, STAR_LED_END);
        addColorGlitter(30, CRGB::Purple, STAR_LED_BEGIN, STAR_LED_END);
        FastLED.show();
    }

    EVERY_N_MILLIS(1000) {
        static int changedLeds = TREE_LEDS * 0.2;

        for (int i = TREE_LED_BEGIN; i < TREE_LED_END; i++) {
            CHSV hsv = rgb2hsv_approximate(CRGB::DeepPink);
            hsv.v = random8(100, 255);
            leds[i] = hsv;
        }

        for (int i = 0; i < changedLeds; i++) {
            uint8_t pos = random8(TREE_LED_BEGIN, TREE_LED_END);
            uint8_t color = random8(0, 4);
            switch (color) {
                case 0:
                    leds[pos] = CRGB::Blue;
                    break;
                case 1:
                    leds[pos] = CRGB::Green;
                    break;
                case 2:
                    leds[pos] = CRGB::Orange;
                    break;
                case 3:
                    leds[pos] = CRGB::Purple;
                    break;
            }
        }
        FastLED.show();
    }

    EVERY_N_MILLIS(2) {
        static uint8_t fadeToBlackScale = 20;
        for (uint16_t i = TREE_LED_BEGIN; i < TREE_LED_END; ++i) {
            CRGB blue = CRGB::Blue;
            CRGB green = CRGB::Green;
            CRGB orange = CRGB::Orange;
            CRGB purple = CRGB::Purple;
            if (leds[i] != blue && leds[i] != green && leds[i] != orange && leds[i] != purple) {
                leds[i].nscale8(255 - fadeToBlackScale);
            }
        }
        FastLED.show();
    }

    FastLED.show();
}

DEFINE_GRADIENT_PALETTE(bhw2_grrrrr_gp){
    0, 184, 15, 155,
    35, 78, 46, 168,
    84, 65, 169, 230,
    130, 9, 127, 186,
    163, 77, 182, 109,
    191, 242, 246, 55,
    216, 142, 128, 103,
    255, 72, 50, 168};

CRGBPalette16 bhw2_grrrrr = bhw2_grrrrr_gp;

void hashtag() {
    EVERY_N_MILLIS(10) {
        static int paletteIndex = 0;

        uint8_t colorIndex = paletteIndex;
        for (uint16_t i = STAR_LED_BEGIN; i < STAR_LED_END; ++i) {
            leds[i] = ColorFromPalette(bhw2_grrrrr, colorIndex, BRIGHTNESS, LINEARBLEND);
            colorIndex += 255 / STAR_LEDS;
            colorIndex++;
        }
        paletteIndex++;
        FastLED.show();
    }
    EVERY_N_MILLIS(50) {
        static uint8_t phase = 0;
        static int loopCount = TREE_LED_END;

        if (phase == 0) {
            for (int i = TREE_LED_BEGIN; i < TREE_LED_END; i++) {
                leds[i] = CRGB::Black;
            }
            phase = 1;
        } else if (phase == 1) {
            leds[loopCount] = ColorFromPalette(bhw2_grrrrr, random8(0, 255), BRIGHTNESS);

            if (loopCount == TREE_LED_BEGIN) {
                phase = 2;
                loopCount = 0;
            } else {
                loopCount--;
            }
        } else if (phase == 2) {
            if (loopCount != 10) {
                loopCount++;
            } else {
                loopCount = 0;
                for (int i = TREE_LED_BEGIN; i < TREE_LED_END; i++) {
                    leds[i] = ColorFromPalette(bhw2_grrrrr, random8(0, 255), BRIGHTNESS);
                }
                if (random8(0, 100) > 80) {
                    phase = 3;
                }
            }
        } else if (phase == 3) {
            if (loopCount != 10) {
                loopCount++;
            } else {
                loopCount = TREE_LED_END;
                for (int i = TREE_LED_BEGIN; i < TREE_LED_END; i++) {
                    leds[i] = CRGB::Black;
                }
                phase = 0;
            }
        }
        FastLED.show();
    }
    FastLED.show();
}
