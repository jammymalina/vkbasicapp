#ifndef RENDER_CONTEXT_CONFIG_H
#define RENDER_CONTEXT_CONFIG_H

#include <stdbool.h>
#include <stdint.h>

#include "./color/color.h"

typedef struct RenderingContextConfig {
    uint32_t frames_in_flight;
    Color clear_color;
    bool depth_enabled;
    uint64_t render_timeout_ms;
} RenderingContextConfig;

static inline RenderingContextConfig rendering_context_config_default() {
    return (RenderingContextConfig){
        .frames_in_flight = 3,
        .clear_color = color_black(),
        .depth_enabled = false,
        .render_timeout_ms = 10000,
    };
}

#endif
