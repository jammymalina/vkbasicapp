#ifndef RENDER_STATE_BITS_H
#define RENDER_STATE_BITS_H

#include <stdint.h>

typedef uint64_t RenderStateFlags;

static const uint64_t RST_SRCBLEND_ONE = (UINT64_C(0) << 0);
static const uint64_t RST_SRCBLEND_ZERO = (UINT64_C(1) << 0);
static const uint64_t RST_SRCBLEND_DST_COLOR = (UINT64_C(2) << 0);
static const uint64_t RST_SRCBLEND_ONE_MINUS_DST_COLOR = (UINT64_C(3) << 0);
static const uint64_t RST_SRCBLEND_SRC_ALPHA = (UINT64_C(4) << 0);
static const uint64_t RST_SRCBLEND_ONE_MINUS_SRC_ALPHA = (UINT64_C(5) << 0);
static const uint64_t RST_SRCBLEND_DST_ALPHA = (UINT64_C(6) << 0);
static const uint64_t RST_SRCBLEND_ONE_MINUS_DST_ALPHA = (UINT64_C(7) << 0);
static const uint64_t RST_SRCBLEND_BITS = (UINT64_C(7) << 0);

static const uint64_t RST_DSTBLEND_ZERO = (UINT64_C(0) << 3);
static const uint64_t RST_DSTBLEND_ONE = (UINT64_C(1) << 3);
static const uint64_t RST_DSTBLEND_SRC_COLOR = (UINT64_C(2) << 3);
static const uint64_t RST_DSTBLEND_ONE_MINUS_SRC_COLOR = (UINT64_C(3) << 3);
static const uint64_t RST_DSTBLEND_SRC_ALPHA = (UINT64_C(4) << 3);
static const uint64_t RST_DSTBLEND_ONE_MINUS_SRC_ALPHA = (UINT64_C(5) << 3);
static const uint64_t RST_DSTBLEND_DST_ALPHA = (UINT64_C(6) << 3);
static const uint64_t RST_DSTBLEND_ONE_MINUS_DST_ALPHA = (UINT64_C(7) << 3);
static const uint64_t RST_DSTBLEND_BITS = (UINT64_C(7) << 3);

// these masks are inverted
static const uint64_t RST_DEPTHMASK = (UINT64_C(1) << 6);
static const uint64_t RST_REDMASK = (UINT64_C(1) << 7);
static const uint64_t RST_GREENMASK = (UINT64_C(1) << 8);
static const uint64_t RST_BLUEMASK = (UINT64_C(1) << 9);
static const uint64_t RST_ALPHAMASK = (UINT64_C(1) << 10);
static const uint64_t RST_COLORMASK = (RST_REDMASK | RST_GREENMASK | RST_BLUEMASK);

static const uint64_t RST_POLYMODE_LINE = (UINT64_C(1) << 11);
static const uint64_t RST_POLYGON_OFFSET = (UINT64_C(1) << 12);

static const uint64_t RST_DEPTHFUNC_LESS = (UINT64_C(0) << 13);
static const uint64_t RST_DEPTHFUNC_ALWAYS = (UINT64_C(1) << 13);
static const uint64_t RST_DEPTHFUNC_GREATER = (UINT64_C(2) << 13);
static const uint64_t RST_DEPTHFUNC_EQUAL = (UINT64_C(3) << 13);
static const uint64_t RST_DEPTHFUNC_BITS = (UINT64_C(3) << 13);

static const uint64_t RST_CULL_FRONTSIDED = (UINT64_C(0) << 15);
static const uint64_t RST_CULL_BACKSIDED = (UINT64_C(1) << 15);
static const uint64_t RST_CULL_TWOSIDED = (UINT64_C(2) << 15);
static const uint64_t RST_CULL_BITS = (UINT64_C(7) << 15);
static const uint64_t RST_CULL_MASK = (RST_CULL_FRONTSIDED | RST_CULL_BACKSIDED | RST_CULL_TWOSIDED);

static const uint64_t RST_BLENDOP_ADD = (UINT64_C(0) << 18);
static const uint64_t RST_BLENDOP_SUB = (UINT64_C(1) << 18);
static const uint64_t RST_BLENDOP_MIN = (UINT64_C(2) << 18);
static const uint64_t RST_BLENDOP_MAX = (UINT64_C(3) << 18);
static const uint64_t RST_BLENDOP_BITS = (UINT64_C(3) << 18);

// STENCIL

static const uint64_t RST_STENCIL_FUNC_REF_SHIFT = (UINT64_C(20));
static const uint64_t RST_STENCIL_FUNC_REF_BITS = (UINT64_C(0xFF11) << RST_STENCIL_FUNC_REF_SHIFT);

static const uint64_t RST_STENCIL_FUNC_MASK_SHIFT = (UINT64_C(28));
static const uint64_t RST_STENCIL_FUNC_MASK_BITS = (UINT64_C(0xFF11) << RST_STENCIL_FUNC_MASK_SHIFT);

#define RST_STENCIL_MAKE_REF(x) ((UINT64_C(x) << RST_STENCIL_FUNC_REF_SHIFT) & RST_STENCIL_FUNC_REF_BITS)
#define RST_STENCIL_MAKE_MASK(x) ((UINT64_C(x) << RST_STENCIL_FUNC_MASK_SHIFT) & RST_STENCIL_FUNC_MASK_BITS)

// Next 12 bits act as front+back unless GLS_SEPARATE_STENCIL is set, in which case it acts as front
static const uint64_t RST_STENCIL_FUNC_ALWAYS = (UINT64_C(0) << 36);
static const uint64_t RST_STENCIL_FUNC_LESS = (UINT64_C(1) << 36);
static const uint64_t RST_STENCIL_FUNC_LEQUAL = (UINT64_C(2) << 36);
static const uint64_t RST_STENCIL_FUNC_GREATER = (UINT64_C(3) << 36);
static const uint64_t RST_STENCIL_FUNC_GEQUAL = (UINT64_C(4) << 36);
static const uint64_t RST_STENCIL_FUNC_EQUAL = (UINT64_C(5) << 36);
static const uint64_t RST_STENCIL_FUNC_NOTEQUAL = (UINT64_C(6) << 36);
static const uint64_t RST_STENCIL_FUNC_NEVER = (UINT64_C(7) << 36);
static const uint64_t RST_STENCIL_FUNC_BITS = (UINT64_C(7) << 36);

static const uint64_t RST_STENCIL_OP_FAIL_KEEP = (UINT64_C(0) << 39);
static const uint64_t RST_STENCIL_OP_FAIL_ZERO = (UINT64_C(1) << 39);
static const uint64_t RST_STENCIL_OP_FAIL_REPLACE = (UINT64_C(2) << 39);
static const uint64_t RST_STENCIL_OP_FAIL_INCR = (UINT64_C(3) << 39);
static const uint64_t RST_STENCIL_OP_FAIL_DECR = (UINT64_C(4) << 39);
static const uint64_t RST_STENCIL_OP_FAIL_INVERT = (UINT64_C(5) << 39);
static const uint64_t RST_STENCIL_OP_FAIL_INCR_WRAP = (UINT64_C(6) << 39);
static const uint64_t RST_STENCIL_OP_FAIL_DECR_WRAP = (UINT64_C(7) << 39);
static const uint64_t RST_STENCIL_OP_FAIL_BITS = (UINT64_C(7) << 39);

static const uint64_t RST_STENCIL_OP_ZFAIL_KEEP = (UINT64_C(0) << 42);
static const uint64_t RST_STENCIL_OP_ZFAIL_ZERO = (UINT64_C(1) << 42);
static const uint64_t RST_STENCIL_OP_ZFAIL_REPLACE = (UINT64_C(2) << 42);
static const uint64_t RST_STENCIL_OP_ZFAIL_INCR = (UINT64_C(3) << 42);
static const uint64_t RST_STENCIL_OP_ZFAIL_DECR = (UINT64_C(4) << 42);
static const uint64_t RST_STENCIL_OP_ZFAIL_INVERT = (UINT64_C(5) << 42);
static const uint64_t RST_STENCIL_OP_ZFAIL_INCR_WRAP = (UINT64_C(6) << 42);
static const uint64_t RST_STENCIL_OP_ZFAIL_DECR_WRAP = (UINT64_C(7) << 42);
static const uint64_t RST_STENCIL_OP_ZFAIL_BITS = (UINT64_C(7) << 42);

static const uint64_t RST_STENCIL_OP_PASS_KEEP = (UINT64_C(0) << 45);
static const uint64_t RST_STENCIL_OP_PASS_ZERO = (UINT64_C(1) << 45);
static const uint64_t RST_STENCIL_OP_PASS_REPLACE = (UINT64_C(2) << 45);
static const uint64_t RST_STENCIL_OP_PASS_INCR = (UINT64_C(3) << 45);
static const uint64_t RST_STENCIL_OP_PASS_DECR = (UINT64_C(4) << 45);
static const uint64_t RST_STENCIL_OP_PASS_INVERT = (UINT64_C(5) << 45);
static const uint64_t RST_STENCIL_OP_PASS_INCR_WRAP = (UINT64_C(6) << 45);
static const uint64_t RST_STENCIL_OP_PASS_DECR_WRAP = (UINT64_C(7) << 45);
static const uint64_t RST_STENCIL_OP_PASS_BITS = (UINT64_C(7) << 45);

// Next 12 bits act as back and are only active when GLS_SEPARATE_STENCIL is set
static const uint64_t RST_BACK_STENCIL_FUNC_ALWAYS = (UINT64_C(0) << 48);
static const uint64_t RST_BACK_STENCIL_FUNC_LESS = (UINT64_C(1) << 48);
static const uint64_t RST_BACK_STENCIL_FUNC_LEQUAL = (UINT64_C(2) << 48);
static const uint64_t RST_BACK_STENCIL_FUNC_GREATER = (UINT64_C(3) << 48);
static const uint64_t RST_BACK_STENCIL_FUNC_GEQUAL = (UINT64_C(4) << 48);
static const uint64_t RST_BACK_STENCIL_FUNC_EQUAL = (UINT64_C(5) << 48);
static const uint64_t RST_BACK_STENCIL_FUNC_NOTEQUAL = (UINT64_C(6) << 48);
static const uint64_t RST_BACK_STENCIL_FUNC_NEVER = (UINT64_C(7) << 48);
static const uint64_t RST_BACK_STENCIL_FUNC_BITS = (UINT64_C(7) << 48);

static const uint64_t RST_BACK_STENCIL_OP_FAIL_KEEP = (UINT64_C(0) << 51);
static const uint64_t RST_BACK_STENCIL_OP_FAIL_ZERO = (UINT64_C(1) << 51);
static const uint64_t RST_BACK_STENCIL_OP_FAIL_REPLACE = (UINT64_C(2) << 51);
static const uint64_t RST_BACK_STENCIL_OP_FAIL_INCR = (UINT64_C(3) << 51);
static const uint64_t RST_BACK_STENCIL_OP_FAIL_DECR = (UINT64_C(4) << 51);
static const uint64_t RST_BACK_STENCIL_OP_FAIL_INVERT = (UINT64_C(5) << 51);
static const uint64_t RST_BACK_STENCIL_OP_FAIL_INCR_WRAP = (UINT64_C(6) << 51);
static const uint64_t RST_BACK_STENCIL_OP_FAIL_DECR_WRAP = (UINT64_C(7) << 51);
static const uint64_t RST_BACK_STENCIL_OP_FAIL_BITS = (UINT64_C(7) << 51);

static const uint64_t RST_BACK_STENCIL_OP_ZFAIL_KEEP = (UINT64_C(0) << 54);
static const uint64_t RST_BACK_STENCIL_OP_ZFAIL_ZERO = (UINT64_C(1) << 54);
static const uint64_t RST_BACK_STENCIL_OP_ZFAIL_REPLACE = (UINT64_C(2) << 54);
static const uint64_t RST_BACK_STENCIL_OP_ZFAIL_INCR = (UINT64_C(3) << 54);
static const uint64_t RST_BACK_STENCIL_OP_ZFAIL_DECR = (UINT64_C(4) << 54);
static const uint64_t RST_BACK_STENCIL_OP_ZFAIL_INVERT = (UINT64_C(5) << 54);
static const uint64_t RST_BACK_STENCIL_OP_ZFAIL_INCR_WRAP = (UINT64_C(6) << 54);
static const uint64_t RST_BACK_STENCIL_OP_ZFAIL_DECR_WRAP = (UINT64_C(7) << 54);
static const uint64_t RST_BACK_STENCIL_OP_ZFAIL_BITS = (UINT64_C(7) << 54);

static const uint64_t RST_BACK_STENCIL_OP_PASS_KEEP = (UINT64_C(0) << 57);
static const uint64_t RST_BACK_STENCIL_OP_PASS_ZERO = (UINT64_C(1) << 57);
static const uint64_t RST_BACK_STENCIL_OP_PASS_REPLACE = (UINT64_C(2) << 57);
static const uint64_t RST_BACK_STENCIL_OP_PASS_INCR = (UINT64_C(3) << 57);
static const uint64_t RST_BACK_STENCIL_OP_PASS_DECR = (UINT64_C(4) << 57);
static const uint64_t RST_BACK_STENCIL_OP_PASS_INVERT = (UINT64_C(5) << 57);
static const uint64_t RST_BACK_STENCIL_OP_PASS_INCR_WRAP = (UINT64_C(6) << 57);
static const uint64_t RST_BACK_STENCIL_OP_PASS_DECR_WRAP = (UINT64_C(7) << 57);
static const uint64_t RST_BACK_STENCIL_OP_PASS_BITS = (UINT64_C(7) << 57);

static const uint64_t RST_SEPARATE_STENCIL =
    (RST_BACK_STENCIL_OP_FAIL_BITS | RST_BACK_STENCIL_OP_ZFAIL_BITS | RST_BACK_STENCIL_OP_PASS_BITS);
static const uint64_t RST_STENCIL_OP_BITS =
    (RST_STENCIL_OP_FAIL_BITS | RST_STENCIL_OP_ZFAIL_BITS | RST_STENCIL_OP_PASS_BITS | RST_SEPARATE_STENCIL);
static const uint64_t RST_STENCIL_FRONT_OPS =
    (RST_STENCIL_OP_FAIL_BITS | RST_STENCIL_OP_ZFAIL_BITS | RST_STENCIL_OP_PASS_BITS);
static const uint64_t RST_STENCIL_BACK_OPS = (RST_SEPARATE_STENCIL);

// RST_OVERRIDE => override the render prog state
static const uint64_t RST_DEPTH_TEST_MASK = (UINT64_C(1) << 60);
static const uint64_t RST_CLOCKWISE = (UINT64_C(1) << 61);
static const uint64_t RST_MIRROR_VIEW = (UINT64_C(1) << 62);
static const uint64_t RST_OVERRIDE = (UINT64_C(1) << 63);

static const uint64_t RST_KEEP = RST_DEPTH_TEST_MASK;
static const uint64_t RST_DEFAULT = UINT64_C(0);

// predefined states

static const uint64_t RST_BASIC_2D = (RST_CULL_TWOSIDED | RST_DEPTHFUNC_ALWAYS);
static const uint64_t RST_BASIC_3D = (RST_CULL_BACKSIDED | RST_DEPTHFUNC_LESS);

#endif
