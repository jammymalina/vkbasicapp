#include "./rendering_context.h"

#include <stdint.h>

#include "../../../core/utils/macro.h"
#include "../../../renderer/core/color/color_transformer.h"
#include "../../initializer/swapchain_builder/swapchain_builder.h"

static VkDevice rendering_context_get_device(const RenderingContext* rendering_context) {
    return rendering_context->command_context->context->device.handle;
}

static RenderingContextError rendering_context_validate_config(
    const RenderingContext* rendering_context, RenderingContextConfig* config) {
    if (config->frames_in_flight > RENDERING_CONTEXT_MAX_FRAMES_IN_FLIGHT) {
        return TOO_MANY_FRAMES_REQUESTED;
    }
    if (config->frames_in_flight == 0) {
        config->frames_in_flight = rendering_context->swapchain.image_count;
    }
    return RENDERING_CONTEXT_NO_ERROR;
}

static SwapchainError rendering_context_create_swapchain(RenderingContext* rendering_context) {
    swapchain_destroy(&rendering_context->swapchain);

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

static RenderingContextError rendering_context_create_command_buffers(RenderingContext* rendering_context) {
    command_context_remove_command_pool(rendering_context->command_context, "_render");

    CommandPoolInitInfo pool_info = {
        .primary_buffer_count = rendering_context->swapchain.image_count,
        .secondary_buffer_count = 0,
        .queue_family_index = rendering_context->swapchain.queue.family_index,
        .reset_enabled = true,
        .transient = false,
    };
    string_copy("_render", pool_info.name, COMMAND_POOL_NAME_SIZE);
    bool context_status = command_context_add_command_pool(rendering_context->command_context, &pool_info);
    if (!context_status) {
        return RENDERING_CONTEXT_COMMAND_CONTEXT_ERROR;
    }

    return RENDERING_CONTEXT_NO_ERROR;
}

static RenderingContextError rendering_context_recreate_swapchain(RenderingContext* rendering_context) {
    vkDeviceWaitIdle(rendering_context_get_device(rendering_context));

    SwapchainError swapchain_status = rendering_context_create_swapchain(rendering_context);
    ASSERT_NO_ERROR(swapchain_status, RENDERING_CONTEXT_SWAPCHAIN_ERROR);

    RenderingContextError status = rendering_context_create_command_buffers(rendering_context);
    ASSERT_NO_ERROR(status, status);

    return RENDERING_CONTEXT_NO_ERROR;
}

RenderingContextError rendering_context_init(
    RenderingContext* rendering_context, CommandContext* context, RenderingContextConfig config) {
    rendering_context_clear(rendering_context);
    rendering_context->command_context = context;

    VkDevice device = rendering_context_get_device(rendering_context);

    SwapchainError swapchain_status = rendering_context_create_swapchain(rendering_context);
    ASSERT_NO_ERROR(swapchain_status, RENDERING_CONTEXT_SWAPCHAIN_ERROR);

    RenderingContextError status = rendering_context_validate_config(rendering_context, &config);
    ASSERT_NO_ERROR(status, status);
    rendering_context->config = config;

    status = rendering_context_create_command_buffers(rendering_context);
    ASSERT_NO_ERROR(status, status);

    VkSemaphoreCreateInfo semaphore_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
    };
    VkFenceCreateInfo fence_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = NULL,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };
    VkResult create_status;
    for (uint32_t i = 0; i < rendering_context->config.frames_in_flight; i++) {
        RenderFrameResources* resources = &rendering_context->frame_resources[i];

        create_status = vkCreateSemaphore(device, &semaphore_info, NULL, &resources->render_semaphore);
        ASSERT_VK_LOG(create_status, "Unable to create semaphore", RENDERING_CONTEXT_INIT_ERROR);

        create_status = vkCreateSemaphore(device, &semaphore_info, NULL, &resources->present_semaphore);
        ASSERT_VK_LOG(create_status, "Unable to create semaphore", RENDERING_CONTEXT_INIT_ERROR);

        create_status = vkCreateFence(device, &fence_info, NULL, &resources->render_fence);
        ASSERT_VK_LOG(create_status, "Unable to create semaphore", RENDERING_CONTEXT_INIT_ERROR);
    }

    return RENDERING_CONTEXT_NO_ERROR;
}

RenderingContextError rendering_context_start_frame(RenderingContext* rendering_context) {
    VkDevice device = rendering_context_get_device(rendering_context);
    uint32_t current_frame = rendering_context->current_frame;
    Swapchain* swapchain = &rendering_context->swapchain;
    RenderFrameResources* resources = &rendering_context->frame_resources[current_frame];

    VkResult status = vkWaitForFences(
        device, 1, &resources->render_fence, true, TIME_MS_TO_NS(rendering_context->config.render_timeout_ms));
    ASSERT_VK_LOG(status, "Render timed out", RENDERING_CONTEXT_RENDER_TIMEOUT);
    status = vkResetFences(device, 1, &resources->render_fence);
    ASSERT_VK_LOG(status, "Unable to reset the fence", RENDERING_CONTEXT_RESET_FENCE_FAILED);

    SwapchainError swapchain_status = swapchain_acquire_next_frame(swapchain, resources->render_semaphore);
    if (swapchain_status == SWAPCHAIN_EXPIRED) {
        ASSERT_NO_ERROR(rendering_context_recreate_swapchain(rendering_context), RENDERING_CONTEXT_SWAPCHAIN_ERROR);
        return rendering_context_start_frame(rendering_context);
    }

    CommandBufferInfo buffer_info = {
        .buffer_index = swapchain->image_index,
        .secondary = false,
    };
    VkCommandBuffer command_buffer =
        command_context_get_command_buffer(rendering_context->command_context, "_render", &buffer_info);
    if (command_buffer == VK_NULL_HANDLE) {
        return RENDERING_CONTEXT_COMMAND_BUFFER_ERROR;
    }
    VkCommandBufferBeginInfo info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = NULL,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = NULL,
    };
    ASSERT_VK(vkResetCommandBuffer(command_buffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT),
        RENDERING_CONTEXT_COMMAND_BUFFER_ERROR);
    ASSERT_VK(vkBeginCommandBuffer(command_buffer, &info), RENDERING_CONTEXT_COMMAND_BUFFER_ERROR);

    VkClearColorValue clear_color;
    color_to_float(&rendering_context->config.clear_color, clear_color.float32);

    VkRenderingAttachmentInfoKHR color_attachment_info = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR,
        .pNext = NULL,
        .imageView = swapchain->image_views[swapchain->image_index],
        .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR,
        .resolveMode = 0,
        .resolveImageView = VK_NULL_HANDLE,
        .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue.color = clear_color,
    };

    VkRenderingInfoKHR rendering_info = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR,
        .pNext = NULL,
        .flags = 0,
        .renderArea =
            {
                .offset = {0, 0},
                .extent = swapchain->extent,
            },

        .layerCount = 1,
        .viewMask = 0,
        .colorAttachmentCount = 1,
        .pColorAttachments = &color_attachment_info,
        .pDepthAttachment = NULL,
        .pStencilAttachment = NULL,
    };

    VkImageMemoryBarrier image_memory_barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = NULL,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = swapchain->images[swapchain->image_index],
        .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
    };

    vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0, 0, NULL, 0, NULL, 1, &image_memory_barrier);

    vkCmdBeginRenderingKHR(command_buffer, &rendering_info);

    return RENDERING_CONTEXT_NO_ERROR;
}

RenderingContextError rendering_context_end_frame(RenderingContext* rendering_context) {
    Swapchain* swapchain = &rendering_context->swapchain;
    CommandBufferInfo buffer_info = {
        .buffer_index = swapchain->image_index,
        .secondary = false,
    };
    VkCommandBuffer command_buffer =
        command_context_get_command_buffer(rendering_context->command_context, "_render", &buffer_info);
    if (command_buffer == VK_NULL_HANDLE) {
        return RENDERING_CONTEXT_COMMAND_BUFFER_ERROR;
    }

    VkImageMemoryBarrier image_memory_barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = NULL,
        .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dstAccessMask = 0,
        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = swapchain->images[swapchain->image_index],
        .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
    };

    vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0, NULL, 1, &image_memory_barrier);
    vkCmdEndRenderingKHR(command_buffer);

    ASSERT_VK(vkEndCommandBuffer(command_buffer), RENDERING_CONTEXT_COMMAND_BUFFER_ERROR);

    uint32_t current_frame = rendering_context->current_frame;
    RenderFrameResources* resources = &rendering_context->frame_resources[current_frame];

    VkPipelineStageFlags pipeline_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &resources->render_semaphore,
        .pWaitDstStageMask = &pipeline_flags,
        .commandBufferCount = 1,
        .pCommandBuffers = &command_buffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &resources->present_semaphore,
    };
    VkResult status =
        vkQueueSubmit(rendering_context->swapchain.queue.handle, 1, &submit_info, resources->render_fence);
    ASSERT_VK(status, RENDERING_CONTEXT_QUEUE_SUBMIT_FAILED);

    VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = NULL,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &resources->present_semaphore,
        .swapchainCount = 1,
        .pSwapchains = &rendering_context->swapchain.handle,
        .pImageIndices = &rendering_context->swapchain.image_index,
        .pResults = NULL,
    };
    status = vkQueuePresentKHR(rendering_context->swapchain.queue.handle, &present_info);

    rendering_context->current_frame =
        (rendering_context->current_frame + 1) % rendering_context->config.frames_in_flight;

    if (status == VK_ERROR_OUT_OF_DATE_KHR || status == VK_SUBOPTIMAL_KHR) {
        return rendering_context_recreate_swapchain(rendering_context);
    }
    ASSERT_VK(status, RENDERING_CONTEXT_PRESENT_FAILED);

    return RENDERING_CONTEXT_NO_ERROR;
}

void rendering_context_destroy(RenderingContext* rendering_context) {
    if (rendering_context->command_context == NULL) {
        return;
    }
    VkDevice device = rendering_context_get_device(rendering_context);
    vkDeviceWaitIdle(device);
    for (uint32_t i = 0; i < RENDERING_CONTEXT_MAX_FRAMES_IN_FLIGHT; i++) {
        RenderFrameResources* resources = &rendering_context->frame_resources[i];
        if (resources->render_semaphore != VK_NULL_HANDLE) {
            vkDestroySemaphore(device, resources->render_semaphore, NULL);
        }
        if (resources->present_semaphore != VK_NULL_HANDLE) {
            vkDestroySemaphore(device, resources->present_semaphore, NULL);
        }
        if (resources->render_fence != VK_NULL_HANDLE) {
            vkDestroyFence(device, resources->render_fence, NULL);
        }
    }
    swapchain_destroy(&rendering_context->swapchain);
    rendering_context_clear(rendering_context);
}
