#include "./rendering_context.h"

#include "../../initializer/swapchain_builder/swapchain_builder.h"

RenderingContextError rendering_context_init(RenderingContext* rendering_context, CommandContext* context) {
    rendering_context->command_context = context;
    SwapchainError swapchain_status = rendering_context_create_swapchain(rendering_context);
    ASSERT_NO_ERROR_LOG(swapchain_status, SwapchainError, swapchain_error_to_string, RENDERING_CONTEXT_INIT_ERROR);

    CommandPoolInitInfo pool_info = {
        .primary_buffer_count = 3,
        .secondary_buffer_count = 0,
        .queue_family_index = rendering_context->swapchain.queue_index,
        .reset_enabled = true,
        .transient = false,
    };
    string_copy("render_pool", pool_info.name, COMMAND_POOL_NAME_SIZE);
    bool context_status = command_context_add_command_pool(rendering_context->command_context, &pool_info);
    if (!context_status) {
        return RENDEERING_CONTEXT_COMMAND_CONTEXT_ERROR;
    }

    return RENDERING_CONTEXT_NO_ERROR;
}

SwapchainError rendering_context_create_swapchain(RenderingContext* rendering_context) {
    if (rendering_context->command_context == NULL) {
        return NO_DEVICE_PROVIDED_SWAPCHAIN;
    }

    Swapchain swapchain;

    SwapchainBuilder swapchain_builder = {0};
    swapchain_builder.old_swapchain = rendering_context->swapchain.handle;
    swapchain_builder_clear(&swapchain_builder);
    swapchain_builder.device = &rendering_context->command_context->context->device;

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
