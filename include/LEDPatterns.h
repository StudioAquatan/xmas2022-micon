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
#define NUM_LEDS 100
#endif  // AWS_IOT_THING_ESP32Sample

#define BRIGHTNESS 96
#define FRAMES_PER_SECOND 120

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

extern void selectPattern(uint8_t patternNumber);
extern void rainbowPatterns();

extern void retweet();
extern void heart();
extern void hashtag();

extern void white();
extern void white_fade();
extern void pink();
extern void pink_fade();
extern void white_pink();
extern void white_pink_blue();

extern void white_pink_flush();
extern void white_pink_blue_flush();

extern void rainbowWithGlitter();
extern void addGlitter(fract8 chanceOfGlitter);
extern void addColorGlitter(fract8 chanceOfGlitter, CRGB color);
extern void confetti();

extern CRGB leds[NUM_LEDS];
extern volatile uint8_t gCurrentPatternNumber;
extern uint8_t gHue;

// List of patterns to cycle through.  Each is defined as a separate function.
typedef void (*LEDPatternList[])();
extern LEDPatternList gPatterns;

#endif  // INCLUDE_GUARD_LEDPATTERNS_H
