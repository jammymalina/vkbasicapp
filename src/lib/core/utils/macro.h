#ifndef MACRO_UTILS_H
#define MACRO_UTILS_H

#include <stdint.h>

#define TO_STRING(name) #name
#define VALUE_TO_STRING(val) STR(val)
#define CONCAT(arg1, arg2) arg1##arg2

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define TIME_MS_TO_NS(milliseconds) (((uint64_t)(milliseconds)) * (1000 * 1000))

#define SWAP_VALUES(x, y)                                                                                              \
    do {                                                                                                               \
        __typeof__(x) _swap_val = x;                                                                                   \
        x = y;                                                                                                         \
        y = _swap_val;                                                                                                 \
    } while (0)

#endif
