#ifndef COLOR_TRANSFORMER_H
#define COLOR_TRANSFORMER_H

#include "./color.h"

static inline void color_to_float(const Color* color, float dst[4]) {
    dst[0] = ((float)color->r) / 255.0f;
    dst[1] = ((float)color->g) / 255.0f;
    dst[2] = ((float)color->b) / 255.0f;
    dst[3] = ((float)color->a) / 255.0f;
}

Color color_from_hex_string(const char* hexstr);

#endif
