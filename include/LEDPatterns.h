#ifndef INCLUDE_GUARD_LEDPATTERNS_H
#define INCLUDE_GUARD_LEDPATTERNS_H

#include <FastLED.h>

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define LED_BUILTIN 13
#define DATA_PIN 14
#define LED_TYPE WS2811

#ifdef AWS_IOT_THING_ESP32Sample
#define COLOR_ORDER RGB
#define NUM_LEDS 200
#define STAR_LEDS 40
#define TREE_LEDS (NUM_LEDS - STAR_LEDS)
#define TREE_LED_BEGIN 0
#define TREE_LED_END TREE_LEDS
#define STAR_LED_BEGIN TREE_LEDS
#define STAR_LED_END NUM_LEDS
#endif  // AWS_IOT_THING_ESP32Sample

#define BRIGHTNESS 50
#define FRAMES_PER_SECOND 120

extern CRGB leds[NUM_LEDS];
extern volatile uint8_t gCurrentPatternNumber;

// List of patterns to cycle through.  Each is defined as a separate function.
typedef void (*LEDPatternList[])();
extern LEDPatternList gPatterns;
extern uint8_t gColorIndex[NUM_LEDS];

extern void selectPattern(uint8_t patternNumber);
extern void rainbowPatterns();

extern void rgb_pattern();
extern void xmas_tree_init();
extern void white();
extern void white_fade();
extern void pink();
extern void pink_fade();
extern void white_pink();
extern void white_pink_blue();

extern void white_pink_flush();
extern void white_pink_blue_flush();

extern void rainbowWithGlitter();
extern void confetti();
extern void rainbowNoise();

#endif  // INCLUDE_GUARD_LEDPATTERNS_H
