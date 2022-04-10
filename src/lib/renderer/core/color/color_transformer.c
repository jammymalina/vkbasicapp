#include "./color_transformer.h"

#include <stddef.h>
#include <stdint.h>

#include "../../../core/string/string.h"

static inline int hex_to_int(char ch) {
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    return -1;
}

Color color_from_hex_string(const char* hexstr) {
    if (string_is_empty(hexstr) || *hexstr != '#') {
        return color_black();
    }
    const char* hexiter = hexstr;
    ++hexiter;

    uint8_t hex[8] = {0};
    size_t i = 0;
    for (i = 0; i < 8 && *hexiter != '\0'; ++i, ++hexiter) {
        int h = hex_to_int(*hexiter);
        if (h == -1) {
            return color_black();
        }
        hex[i] = (uint8_t)h;
    }

    if (*hexiter != '\0' || i < 3 || i == 5 || i == 7) {
        return color_black();
    }

    Color color = {0};

    if (i == 3 || i == 4) {
        color.r = (hex[0] << 4) | hex[0];
        color.g = (hex[1] << 4) | hex[1];
        color.b = (hex[2] << 4) | hex[2];
        color.a = i == 4 ? (hex[3] << 4) | hex[3] : 255;
        return color;
    }

    color.r = (hex[0] << 4) | hex[1];
    color.g = (hex[2] << 4) | hex[3];
    color.b = (hex[4] << 4) | hex[5];
    color.a = i == 8 ? (hex[6] << 4) | hex[7] : 255;

    return color;
}
