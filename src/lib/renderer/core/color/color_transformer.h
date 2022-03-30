#ifndef COLOR_TRANSFORMER_H
#define COLOR_TRANSFORMER_H

#include "./color.h"

static void color_to_float(const Color* color, float dest[4]) {
    dest[0] = ((float)color->r) / 255.0f;
    dest[1] = ((float)color->g) / 255.0f;
    dest[2] = ((float)color->b) / 255.0f;
    dest[3] = ((float)color->a) / 255.0f;
}

#endif
