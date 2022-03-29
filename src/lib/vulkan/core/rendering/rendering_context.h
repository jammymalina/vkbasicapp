#ifndef RENDERING_CONTEXT_H
#define RENDERING_CONTEXT_H

#include <stdbool.h>

#include "../context/context.h"
#include "../errors.h"
#include "../swapchain/swapchain.h"

typedef struct RenderingContext {
    const Context* context;
    Swapchain swapchain;
    bool is_init;
} RenderingContext;

static inline void rendering_context_clear(RenderingContext* rendering_context) {
    rendering_context->context = NULL;
    rendering_context->is_init = false;
    swapchain_clear(&rendering_context->swapchain);
}

RenderingContextError rendering_context_init(RenderingContext* rendering_context, const Context* context);
SwapchainError rendering_context_create_swapchain(RenderingContext* rendering_context);
void rendering_context_destroy(RenderingContext* rendering_context);

#endif
