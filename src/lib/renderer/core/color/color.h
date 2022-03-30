#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

typedef struct Color {
    uint8_t r, g, b, a;
} Color;

static inline Color color_black() { return (Color){.r = 0, .g = 0, .b = 0, .a = 255}; }
static inline Color color_white() { return (Color){.r = 255, .g = 255, .b = 255, .a = 255}; }
static inline Color color_red() { return (Color){.r = 255, .g = 0, .b = 0, .a = 255}; }
static inline Color color_lime() { return (Color){.r = 0, .g = 255, .b = 0, .a = 255}; }
static inline Color color_blue() { return (Color){.r = 0, .g = 0, .b = 255, .a = 255}; }
static inline Color color_yellow() { return (Color){.r = 255, .g = 255, .b = 0, .a = 255}; }
static inline Color color_cyan() { return (Color){.r = 0, .g = 255, .b = 255, .a = 255}; }
static inline Color color_magenta() { return (Color){.r = 255, .g = 0, .b = 255, .a = 255}; }
static inline Color color_silver() { return (Color){.r = 192, .g = 192, .b = 192, .a = 255}; }
static inline Color color_gray() { return (Color){.r = 128, .g = 128, .b = 128, .a = 255}; }
static inline Color color_maroon() { return (Color){.r = 128, .g = 0, .b = 0, .a = 255}; }
static inline Color color_olive() { return (Color){.r = 128, .g = 128, .b = 0, .a = 255}; }
static inline Color color_green() { return (Color){.r = 0, .g = 128, .b = 0, .a = 255}; }
static inline Color color_purple() { return (Color){.r = 128, .g = 0, .b = 128, .a = 255}; }
static inline Color color_teal() { return (Color){.r = 0, .g = 128, .b = 128, .a = 255}; }
static inline Color color_navy() { return (Color){.r = 0, .g = 0, .b = 128, .a = 255}; }

#endif
