#include "LEDPatterns.h"

#include <FastLED.h>

#include "patterns/twitter.h"
#include "patterns/utils.h"
#include "patterns/xmas_tree.h"

FASTLED_USING_NAMESPACE

CRGB leds[NUM_LEDS];
uint8_t gColorIndex[NUM_LEDS];

volatile uint8_t gCurrentPatternNumber = 0;  // Index number of which pattern is current

LEDPatternList gPatterns = {
    retweet,               // 0 - リツイート
    heart,                 // 1 - いいね
    hashtag,               // 2 - ハッシュタグ
    xmas_tree_init,        // 3 - ツリー (初期状態)
    xmas_tree_red_flush,   // 4 - ツリー (赤色でフラッシュ)
    xmas_christmas_candy,  // 5 - クリスマスのキャンディ
    xmas_colors,           // 6 - クリスマスの色
    rgb_pattern,           // 7 - RGB
    rainbowWithGlitter,    // 11 - 虹1
    rainbowNoise,          // 12 - 虹2
    rainbowPatterns,       // 13 - 虹1と虹2を交互に繰り返す(一定数以上用)
};

void selectPattern(uint8_t patternNumber) {
    uint8_t ranged = patternNumber % ARRAY_SIZE(gPatterns);

    if (ranged != patternNumber) {
        Serial.print("Pattern number is out of range. ");
        Serial.println(patternNumber);
        gCurrentPatternNumber = ranged;
        Serial.print("Now, current pattern is ");
        Serial.println(gCurrentPatternNumber);
    }

    gPatterns[ranged]();
}

void xmas_tree_init() {
    set_xmas_tree_star();
    set_xmas_tree_body();
    FastLED.show();
}

void xmas_tree_red_flush() {
    set_xmas_tree_star();
    EVERY_N_MILLIS(5) {
        static uint8_t loopCount = 0;
        static bool fadeMode = false;
        static const uint16_t changed_leds = TREE_LEDS * 0.2;
        static uint16_t targetLEDIndex[changed_leds];
        static CRGB colorCurrent = CRGB::Green;

        if (loopCount == 0) {
            fadeMode = !fadeMode;

            if (fadeMode) {
                for (int i = 0; i < changed_leds; i++) {
                    targetLEDIndex[i] = random16(TREE_LED_BEGIN, TREE_LED_END);
                }
            }
        }

        if (fadeMode) {
            colorCurrent = blend(CRGB::Green, CRGB::Red, loopCount);
        } else {
            colorCurrent = blend(CRGB::Red, CRGB::Green, loopCount);
        }

        set_xmas_tree_body();
        for (int i = 0; i < changed_leds; i++) {
            leds[targetLEDIndex[i]] = colorCurrent;
        }
        loopCount++;
        FastLED.show();
    }
    FastLED.show();
}

DEFINE_GRADIENT_PALETTE(christmas_candy_gp){
    0, 255, 255, 255,
    25, 255, 0, 0,
    51, 255, 255, 255,
    76, 0, 55, 0,
    102, 255, 255, 255,
    127, 255, 0, 0,
    153, 255, 255, 255,
    178, 0, 55, 0,
    204, 255, 255, 255,
    229, 255, 0, 0,
    255, 255, 255, 255};

CRGBPalette16 christmas_candy = christmas_candy_gp;

void xmas_christmas_candy() {
    set_xmas_tree_star();
    for (int i = TREE_LED_BEGIN; i < TREE_LED_END; i++) {
        leds[i] = ColorFromPalette(christmas_candy, gColorIndex[i], BRIGHTNESS);
    }
    EVERY_N_MILLISECONDS(100) {
        for (int i = TREE_LED_BEGIN; i < TREE_LED_END; i++) {
            gColorIndex[i] += 1;
        }
        FastLED.show();
    }
    FastLED.show();
}

void rgb_pattern() {
    set_xmas_tree_star();
    EVERY_N_MILLISECONDS(1) {
        static uint16_t loopCount = 0;
        static uint8_t colorIndex = 0;
        loopCount++;
        if (loopCount % 200 == 0) {
            colorIndex = (colorIndex + 1) % 3;
        }
        for (int i = TREE_LED_BEGIN; i < TREE_LED_END; i++) {
            switch (colorIndex) {
                case 0:
                    leds[i] = CRGB::Red;
                    break;
                case 1:
                    leds[i] = CRGB::Green;
                    break;
                case 2:
                    leds[i] = CRGB::Blue;
                    break;
            }
        }
        addGlitter(30, TREE_LED_BEGIN, TREE_LED_END);
        addGlitter(30, TREE_LED_BEGIN, TREE_LED_END);
        addGlitter(30, TREE_LED_BEGIN, TREE_LED_END);
        FastLED.show();
    }
    FastLED.show();
}

uint8_t gCurrentRainbowPatternNumber = 0;
LEDPatternList gRainbowPatterns = {rainbowWithGlitter, rainbowNoise};
void rainbowPatterns() {
    gRainbowPatterns[gCurrentRainbowPatternNumber]();
    EVERY_N_SECONDS(60) {
        for (int i = 0; i < 100; i++) {
            fadeToBlackBy(leds, NUM_LEDS, 10);
            FastLED.delay(10);
            FastLED.show();
        }
        gCurrentRainbowPatternNumber = (gCurrentRainbowPatternNumber + 1) % ARRAY_SIZE(gRainbowPatterns);
    }
}

void rainbowWithGlitter() {
    // built-in FastLED rainbow, plus some random sparkly glitter
    EVERY_N_MILLISECONDS(1) {
        static uint8_t gHue = 0;
        fill_rainbow(leds, NUM_LEDS, gHue, 1);
        addGlitter(30);
        gHue++;
        FastLED.show();
    }
    FastLED.show();
}

void confetti() {
    EVERY_N_MILLISECONDS(1) {
        static uint8_t gHue = 0;
        int pos = random16(NUM_LEDS);
        for (int i = 0; i < 3; i++) {
            leds[pos] += CHSV(gHue + random8(64), 200, 190);
        }
        gHue++;
        FastLED.show();
    }

    EVERY_N_MILLISECONDS(50) {
        fadeToBlackBy(leds, NUM_LEDS, 1);
        FastLED.show();
    }
    FastLED.show();
}

void rainbowNoise() {
    uint8_t octaves = 1;
    uint16_t x = 0;
    int scale = 100;
    uint8_t hue_octaves = 1;
    uint16_t hue_x = 1;
    int hue_scale = 50;
    uint16_t ntime = millis() / 3;
    uint8_t hue_shift = 5;
    fill_noise16(leds, NUM_LEDS, octaves, x, scale, hue_octaves, hue_x, hue_scale, ntime, hue_shift);
    FastLED.show();
}
