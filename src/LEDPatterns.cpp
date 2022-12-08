#include "LEDPatterns.h"

#include <FastLED.h>

FASTLED_USING_NAMESPACE

CRGB leds[NUM_LEDS];

volatile uint8_t gCurrentPatternNumber = 0;  // Index number of which pattern is current
uint8_t gHue = 0;                            // rotating "base color" used by many of the patterns
uint8_t gLoopCount = 0;

LEDPatternList gPatterns = {
    retweet,                // 0 - リツイート
    heart,                  // 1 - いいね
    hashtag,                // 2 - ハッシュタグ
    white,                  // 3 - 白(0~9)
    white_fade,             // 4 - 白点滅
    pink,                   // 5 - ピンク
    pink_fade,              // 6 - ピンク点滅
    white_pink,             // 7 - 白とピンク
    white_pink_flush,       // 8 - 白とピンク点滅
    white_pink_blue,        // 9 - 白とピンクと青
    white_pink_blue_flush,  // 10 - 白とピンクと青点滅
    rainbowWithGlitter,     // 11 - 虹1
    confetti,               // 12 - 虹2
    rainbowPatterns         // 13 - 虹1と虹2を交互に繰り返す(一定数以上用)
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

uint8_t gCurrentRainbowPatternNumber = 0;
LEDPatternList gRainbowPatterns = {rainbowWithGlitter, confetti};
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

DEFINE_GRADIENT_PALETTE(white_pink_gp){
    0, CRGB(CRGB::White).r, CRGB(CRGB::White).g, CRGB(CRGB::White).b,
    255, CRGB(CRGB::DeepPink).r, CRGB(CRGB::DeepPink).g, CRGB(CRGB::DeepPink).b};

DEFINE_GRADIENT_PALETTE(white_pink_blue_gp){
    0, CRGB(CRGB::White).r, CRGB(CRGB::White).g, CRGB(CRGB::White).b,
    128, CRGB(CRGB::DeepPink).r, CRGB(CRGB::DeepPink).g, CRGB(CRGB::DeepPink).b,
    255, CRGB(CRGB::Blue).r, CRGB(CRGB::Blue).g, CRGB(CRGB::Blue).b};

CRGBPalette16 gWhitePinkPalette = white_pink_gp;
CRGBPalette16 gWhitePinkBluePalette = white_pink_blue_gp;

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
    fill_palette(leds, NUM_LEDS, gLoopCount, 255 / NUM_LEDS, purplePalette, 255, LINEARBLEND);

    EVERY_N_MILLISECONDS(10) {
        gLoopCount++;
    }
}

void white() {
    fill_solid(leds, NUM_LEDS, CRGB::White);
    FastLED.show();
    FastLED.delay(1000);
}

int fadeCount = 0;
uint8_t fadeMode = 0;

void white_fade() {
    CHSV white_hsv = rgb2hsv_approximate(CRGB::White);

    if (fadeMode == 0) {
        // フェードイン
        fill_solid(leds, NUM_LEDS, CHSV(white_hsv.h, white_hsv.s, fadeCount % 256));
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
        fill_solid(leds, NUM_LEDS, CHSV(white_hsv.h, white_hsv.s, 255 - (fadeCount % 256)));
        if (255 - (fadeCount % 256) == 0) {
            fadeMode = 0;
            fadeCount = 0;
        }
    }
    fadeCount++;
    FastLED.delay(5);
    FastLED.show();
}

void pink() {
    fill_solid(leds, NUM_LEDS, CRGB::DeepPink);
    FastLED.show();
    addGlitter(30);
    FastLED.delay(10);
}

void pink_fade() {
    // CHSV pink_hsv = rgb2hsv_approximate(CRGB::DeepPink);
    CHSV pink_hsv = CHSV(90 * 2.55, 92 * 2.55, 255);

    if (fadeMode == 0) {
        // フェードイン
        fill_solid(leds, NUM_LEDS, CHSV(pink_hsv.h, pink_hsv.s, fadeCount % 256));
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
        fill_solid(leds, NUM_LEDS, CHSV(pink_hsv.h, pink_hsv.s, 255 - (fadeCount % 256)));
        if (255 - (fadeCount % 256) == 0) {
            fadeMode = 0;
            fadeCount = 0;
        }
    }
    fadeCount++;
    FastLED.delay(5);
    FastLED.show();
}

void white_pink() {
    fill_solid(leds, NUM_LEDS, CRGB::White);
    for (int i = 0; i < NUM_LEDS / 2; i++) {
        if (i % 2 == 0) {
            leds[2 * i] = CRGB::White;
            leds[2 * i + 1] = CRGB::White;
        } else {
            leds[2 * i] = CRGB::DeepPink;
            leds[2 * i + 1] = CRGB::DeepPink;
        }
    }
    addColorGlitter(30, CRGB::White);
    addColorGlitter(30, CRGB::DeepPink);
    FastLED.show();
    FastLED.delay(10);
}

void white_pink_flush() {
    gLoopCount++;
    fadeToBlackBy(leds, NUM_LEDS, 1);
    int pos = random16(NUM_LEDS);
    leds[pos] = ColorFromPalette(gWhitePinkPalette, random8(), 255, LINEARBLEND);
    FastLED.delay(1000 / 10);
    FastLED.show();
}

void white_pink_blue() {
    fill_solid(leds, NUM_LEDS, CRGB::White);
    for (int i = 0; i < NUM_LEDS / 2; i++) {
        if (i % 3 == 0) {
            leds[2 * i] = CRGB::White;
            leds[2 * i + 1] = CRGB::White;
        } else if (i % 3 == 1) {
            leds[2 * i] = CRGB::DeepPink;
            leds[2 * i + 1] = CRGB::DeepPink;
        } else {
            leds[2 * i] = CRGB::Blue;
            leds[2 * i + 1] = CRGB::Blue;
        }
    }
    addColorGlitter(30, CRGB::White);
    addColorGlitter(30, CRGB::DeepPink);
    addColorGlitter(30, CRGB::Blue);
    FastLED.show();
    FastLED.delay(10);
}

void white_pink_blue_flush() {
    gLoopCount++;
    fadeToBlackBy(leds, NUM_LEDS, 1);
    int pos = random16(NUM_LEDS);
    leds[pos] = ColorFromPalette(gWhitePinkBluePalette, random8(), 255, LINEARBLEND);
    FastLED.delay(1000 / 10);
    FastLED.show();
}

void rainbowWithGlitter() {
    // built-in FastLED rainbow, plus some random sparkly glitter
    fill_rainbow(leds, NUM_LEDS, gHue, 1);
    addGlitter(30);
    EVERY_N_MILLISECONDS(100) { gHue++; }
    FastLED.delay(10);
}

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

void confetti() {
    // random colored speckles that blink in and fade smoothly
    gLoopCount++;
    fadeToBlackBy(leds, NUM_LEDS, 1);
    int pos = random16(NUM_LEDS);
    leds[pos] += CHSV(gHue + random8(64), 200, 255);

    FastLED.delay(1000 / 10);
    EVERY_N_MILLISECONDS(100) { gHue++; }
}
