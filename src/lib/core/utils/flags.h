#ifndef FLAGS_H
#define FLAGS_H

#define FLAG_CREATE(n) (1 << (n))
#define FLAGS_SET_FLAG(n, f) ((n) |= (f))
#define FLAGS_CLEAR_FLAG(n, f) ((n) &= ~(f))
#define FLAGS_TOGGLE_FLAG(n, f) ((n) ^= (f))
#define FLAGS_CHECK_FLAG(n, f) ((n) & (f))
#define FLAGS_CHECK_ALL_BITS(n, f) (((n) & (f)) == (f))

#endif
