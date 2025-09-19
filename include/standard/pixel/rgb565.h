#ifndef RGB565_H
#define RGB565_H

#include <stdint.h>

typedef union {
  struct {
    uint16_t r : 5;
    uint16_t g : 6;
    uint16_t b : 5;
  };

  uint16_t mask;
} rgb565_t;

#define RGB565(red, green, blue)                               \
  (rgb565_t) {                                                 \
    .r = ((red) >> 3), .g = ((green) >> 2), .b = ((blue) >> 3) \
  }
#define RGB565_GRAY(x) RGB565((x), (x), (x))

#define RGB565_BLACK RGB565_GRAY(0)
#define RGB565_GRAY_DARK RGB565_GRAY(63)
#define RGB565_GRAY_MEDIUM RGB565_GRAY(127)
#define RGB565_GRAY_LIGHT RGB565_GRAY(191)
#define RGB565_WHITE RGB565_GRAY(255)

#define RGB565_RED RGB565(255, 0, 0)
#define RGB565_GREEN RGB565(0, 255, 0)
#define RGB565_BLUE RGB565(0, 0, 255)
#define RGB565_CYAN RGB565(0, 255, 255)
#define RGB565_MAGENTA RGB565(255, 0, 255)
#define RGB565_YELLOW RGB565(255, 255, 0)

static inline rgb565_t rgb565_invert(rgb565_t a) {
  return (rgb565_t){.mask = ~a.mask};
}

static inline rgb565_t rgb565_blend8(rgb565_t a, rgb565_t b, uint8_t v) {
  return (rgb565_t){
      .r = (((uint16_t)a.r * (256 - v)) + ((uint16_t)b.r * v)) / 256,
      .g = (((uint16_t)a.g * (256 - v)) + ((uint16_t)b.g * v)) / 256,
      .b = (((uint16_t)a.b * (256 - v)) + ((uint16_t)b.b * v)) / 256};
}

static inline rgb565_t rgb565_max(rgb565_t a, rgb565_t b) {
  return (rgb565_t){.r = (a.r > b.r ? a.r : b.r),
                    .g = (a.g > b.g ? a.g : b.g),
                    .b = (a.b > b.b ? a.b : b.b)};
}

static inline rgb565_t rgb565_adds(rgb565_t a, rgb565_t b) {
  return (rgb565_t){.r = ((a.r + b.r) >= 31 ? 31 : (a.r + b.r)),
                    .g = ((a.g + b.g) >= 63 ? 63 : (a.g + b.g)),
                    .b = ((a.b + b.b) >= 31 ? 31 : (a.b + b.b))};
}

#endif
