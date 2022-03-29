#include "./rendering_context.h"

#include "../../initializer/swapchain_builder/swapchain_builder.h"

RenderingContextError rendering_context_init(RenderingContext* rendering_context, const Context* context) {
    rendering_context->context = context;
    SwapchainError swapchain_status = rendering_context_create_swapchain(rendering_context);
    ASSERT_NO_ERROR_LOG(swapchain_status, SwapchainError, swapchain_error_to_string, RENDERING_CONTEXT_INIT_ERROR);

    return RENDERING_CONTEXT_NO_ERROR;
}

SwapchainError rendering_context_create_swapchain(RenderingContext* rendering_context) {
    if (rendering_context->context == NULL) {
        return NO_DEVICE_PROVIDED_SWAPCHAIN;
    }

    Swapchain swapchain;

    SwapchainBuilder swapchain_builder = {0};
    swapchain_builder.old_swapchain = rendering_context->swapchain.handle;
    swapchain_builder_clear(&swapchain_builder);
    swapchain_builder.device = &rendering_context->context->device;

    SwapchainError status = swapchain_builder_build(&swapchain_builder, &swapchain);
    ASSERT_NO_ERROR(status, status);

    swapchain_copy(&swapchain, &rendering_context->swapchain, true);

    return SWAPCHAIN_NO_ERROR;
}

void rendering_context_destroy(RenderingContext* rendering_context) {
    if (rendering_context == NULL) {
        return;
    }
    swapchain_destroy(&rendering_context->swapchain);
    rendering_context_clear(rendering_context);
}
