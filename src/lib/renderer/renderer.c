#include "./renderer.h"

#include "../vulkan/core/errors.h"

void renderer_clear(Renderer* renderer) { renderer->context = NULL; }

void renderer_init(Renderer* renderer, RenderingContext* context) { renderer->context = context; }

bool renderer_render(Renderer* renderer) {
    RenderingContext* context = renderer->context;
    RenderingContextError status = rendering_context_start_frame(context);
    ASSERT_NO_ERROR_LOG(status, RenderingContextError, rendering_context_error_to_string, false);

    // TODO: DRAW STUFF

    status = rendering_context_end_frame(context);
    ASSERT_NO_ERROR_LOG(status, RenderingContextError, rendering_context_error_to_string, false);

    return true;
}
