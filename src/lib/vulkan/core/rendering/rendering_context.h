#ifndef RENDERING_CONTEXT_H
#define RENDERING_CONTEXT_H

#include <stdbool.h>
#include <vulkan/vulkan.h>

#include "../../../renderer/core/rendering_context_config.h"
#include "../command/command_context.h"
#include "../errors.h"
#include "../swapchain/swapchain.h"

#define RENDERING_CONTEXT_MAX_FRAMES_IN_FLIGHT 8

typedef struct RenderFrameResources {
    VkFence render_fence;
    VkSemaphore render_semaphore;
    VkSemaphore present_semaphore;
} RenderFrameResources;

typedef struct RenderingContext {
    CommandContext* command_context;
    Swapchain swapchain;

    uint32_t current_frame;
    RenderFrameResources frame_resources[RENDERING_CONTEXT_MAX_FRAMES_IN_FLIGHT];

    RenderingContextConfig config;
} RenderingContext;

static inline void rendering_context_clear(RenderingContext* rendering_context) {
    rendering_context->command_context = NULL;
    swapchain_clear(&rendering_context->swapchain);
    rendering_context->config = (RenderingContextConfig){0};
    rendering_context->current_frame = 0;
    for (uint32_t i = 0; i < RENDERING_CONTEXT_MAX_FRAMES_IN_FLIGHT; i++) {
        rendering_context->frame_resources[i].render_semaphore = VK_NULL_HANDLE;
        rendering_context->frame_resources[i].present_semaphore = VK_NULL_HANDLE;
        rendering_context->frame_resources[i].render_fence = VK_NULL_HANDLE;
    }
}

RenderingContextError rendering_context_init(
    RenderingContext* rendering_context, CommandContext* context, RenderingContextConfig config);

RenderingContextError rendering_context_start_frame(RenderingContext* rendering_context);
RenderingContextError rendering_context_end_frame(RenderingContext* rendering_context);

void rendering_context_destroy(RenderingContext* rendering_context);

#endif
