#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>
#include <stdint.h>

#include "../vulkan/core/rendering/rendering_context.h"

typedef struct Renderer {
    RenderingContext* context;
} Renderer;

void renderer_clear(Renderer* renderer);
void renderer_init(Renderer* renderer, RenderingContext* context);
bool renderer_render(Renderer* renderer);

#endif
