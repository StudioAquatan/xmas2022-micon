#ifndef INCLUDE_GUARD_UTILS_H
#define INCLUDE_GUARD_UTILS_H

#include <FastLED.h>

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

extern void addGlitter(fract8 chanceOfGlitter);
extern void addGlitter(fract8 chanceOfGlitter, uint16_t ledStartIndex, uint16_t ledEndIndex);
extern void addColorGlitter(fract8 chanceOfGlitter, CRGB color);
extern void addColorGlitter(fract8 chanceOfGlitter, CRGB color, uint16_t ledStartIndex, uint16_t ledEndIndex);

#endif  // INCLUDE_GUARD_UTILS_H
