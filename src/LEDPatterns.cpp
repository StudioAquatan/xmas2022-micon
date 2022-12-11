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
    xmas_green_fade,       // 4 - ツリー (緑色でフェードイン)
    xmas_tree_red_flush,   // 5 - ツリー (赤色でフラッシュ)
    xmas_christmas_candy,  // 6 - クリスマスのキャンディ
    purplefly_pattern,     // 7 - 紫色
    xmas_white_christmas,  // 8 - ホワイトクリスマス
    rgb_pattern,           // 9 - RGB
    xmas_colors,           // 10 - クリスマスの色
    rainbowWithGlitter,    // 11 - 虹1
    rainbowNoise,          // 12 - 虹2
    selectionPatterns,     // 13 - 選択されたパターンを繰り返す
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

void xmas_green_fade() {
    EVERY_N_MILLIS(5) {
        static uint8_t fadeCount = 0;
        static uint8_t fadeMode = 0;
        CHSV green_hsv = rgb2hsv_approximate(CRGB::Green);

        if (fadeMode == 0) {
            // フェードイン
            for (int i = TREE_LED_BEGIN; i < TREE_LED_END; i++) {
                leds[i] = CHSV(green_hsv.h, green_hsv.s, fadeCount % 256);
            }
            if (fadeCount % 256 == 255) {
                fadeMode = 1;
            }
        } else if (fadeMode == 1) {
            // 3秒待機する
            if (fadeCount % (255 + 600) == 0) {
                fadeMode = 2;
                fadeCount = 255;
            }
        } else {
            // フェードアウト
            for (int i = TREE_LED_BEGIN; i < TREE_LED_END; i++) {
                leds[i] = CHSV(green_hsv.h, green_hsv.s, 255 - (fadeCount % 256));
            }
            if (255 - (fadeCount % 256) == 0) {
                fadeMode = 0;
                fadeCount = 0;
            }
        }
        fadeCount++;
        FastLED.show();
    }
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

DEFINE_GRADIENT_PALETTE(purplefly_gp){
    0, 0, 0, 0,
    63, 239, 0, 122,
    191, 252, 255, 78,
    255, 0, 0, 0};

CRGBPalette16 purplefly = purplefly_gp;

void purplefly_pattern() {
    set_xmas_tree_star();
    EVERY_N_MILLIS(10) {
        static int paletteIndex = 0;

        uint8_t colorIndex = paletteIndex;
        for (uint16_t i = TREE_LED_BEGIN; i < TREE_LED_END; ++i) {
            leds[i] = ColorFromPalette(purplefly, colorIndex, BRIGHTNESS, LINEARBLEND);
            colorIndex += 255 / TREE_LEDS;
            colorIndex++;
        }
        paletteIndex++;
        FastLED.show();
    }
    FastLED.show();
}

DEFINE_GRADIENT_PALETTE(white_christmas_gp){
    0, 117, 97, 49,
    12, 117, 97, 49,
    12, 26, 173, 106,
    25, 26, 173, 106,
    25, 10, 77, 164,
    63, 10, 77, 164,
    63, 0, 21, 85,
    102, 0, 21, 85,
    102, 53, 86, 135,
    127, 53, 86, 135,
    127, 26, 31, 65,
    153, 26, 31, 65,
    153, 61, 31, 96,
    191, 61, 31, 96,
    191, 82, 97, 85,
    229, 82, 97, 85,
    229, 255, 255, 255,
    255, 255, 255, 255};

CRGBPalette16 white_christmas = white_christmas_gp;

void xmas_white_christmas() {
    set_xmas_tree_star();
    EVERY_N_MILLIS(10) {
        static int paletteIndex = 0;

        uint8_t colorIndex = paletteIndex;
        for (uint16_t i = TREE_LED_BEGIN; i < TREE_LED_END; ++i) {
            leds[i] = ColorFromPalette(white_christmas, colorIndex, BRIGHTNESS, LINEARBLEND);
            colorIndex += 255 / TREE_LEDS;
            colorIndex++;
        }
        paletteIndex++;
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

uint8_t gCurrentSelectionPatternNumber = 0;
LEDPatternList gSelectionPatterns = {
    xmas_tree_red_flush,   // 5 - ツリー (赤色でフラッシュ)
    xmas_christmas_candy,  // 6 - クリスマスのキャンディ
    purplefly_pattern,     // 7 - 紫色
    xmas_white_christmas,  // 8 - ホワイトクリスマス
    rgb_pattern,           // 9 - RGB
    xmas_colors,           // 10 - クリスマスの色
    rainbowWithGlitter,    // 11 - 虹1
    rainbowNoise,          // 12 - 虹2
};
void selectionPatterns() {
    gSelectionPatterns[gCurrentSelectionPatternNumber]();
    EVERY_N_SECONDS(10) {
        for (int i = 0; i < 100; i++) {
            fadeToBlackBy(leds, NUM_LEDS, 10);
            FastLED.delay(10);
            FastLED.show();
        }
        gCurrentSelectionPatternNumber = random8(ARRAY_SIZE(gSelectionPatterns));
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
