#include "./renderer.h"

#include "../vulkan/core/errors.h"

void renderer_clear(Renderer* renderer) { renderer->context = NULL; }

void renderer_init(Renderer* renderer, RenderingContext* context) { renderer->context = context; }

bool renderer_resize(Renderer* renderer) {
    RenderingContextError status = rendering_context_resize(renderer->context);
    ASSERT_SUCCESS_LOG(status, RenderingContextError, rendering_context_error_to_string, false);

    return true;
}

bool renderer_render(Renderer* renderer) {
    RenderingContext* context = renderer->context;
    RenderingContextError status = rendering_context_start_frame(context);
    if (status == RENDERING_CONTEXT_REFRESHING) {
        return true;
    }

    // TODO: DRAW STUFF
    rendering_context_render(context);

    status = rendering_context_end_frame(context);
    if (status == RENDERING_CONTEXT_REFRESHING) {
        return true;
    }
    ASSERT_SUCCESS_LOG(status, RenderingContextError, rendering_context_error_to_string, false);

    return true;
}
