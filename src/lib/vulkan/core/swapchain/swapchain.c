#include "./swapchain.h"

#include <stdint.h>

#include "../functions.h"

bool swapchain_is_init(const Swapchain* swapchain) {
    return swapchain->device != NULL && swapchain->handle != VK_NULL_HANDLE &&
           swapchain->queue.handle != VK_NULL_HANDLE;
}

void swapchain_copy(const Swapchain* src, Swapchain* dst, bool destroy_dst) {
    if (destroy_dst) {
        swapchain_destroy(dst);
    }

    dst->handle = src->handle;
    dst->device = src->device;
    dst->image_count = src->image_count;
    dst->image_format = src->image_format;
    dst->extent = src->extent;
    dst->queue = src->queue;
    dst->image_index = src->image_index;

    dst->init_images = src->init_images;
    dst->init_image_view_count = src->init_image_view_count;
    for (uint32_t i = 0; i < src->image_count; i++) {
        dst->images[i] = src->images[i];
    }
    for (uint32_t i = 0; i < src->init_image_view_count; i++) {
        dst->image_views[i] = src->image_views[i];
    }
}

SwapchainError swapchain_load_images(Swapchain* swapchain) {
    if (!swapchain_is_init(swapchain)) {
        return FAILED_GET_SWAPCHAIN_IMAGES;
    }
    uint32_t count = 0;
    VkResult status;

    status = vkGetSwapchainImagesKHR(swapchain->device->handle, swapchain->handle, &count, NULL);
    ASSERT_VK_LOG(status, "Unable to get swapchain images", FAILED_GET_SWAPCHAIN_IMAGES);
    if (count > SWAPCHAIN_MAX_IMAGES) {
        return TOO_MANY_IMAGES_REQUESTED;
    }

    status = vkGetSwapchainImagesKHR(swapchain->device->handle, swapchain->handle, &count, swapchain->images);
    ASSERT_VK_LOG(status, "Unable to get swapchain images", FAILED_GET_SWAPCHAIN_IMAGES);

    swapchain->image_count = count;
    swapchain->init_images = true;
    return SWAPCHAIN_NO_ERROR;
}

SwapchainError swapchain_load_image_views(Swapchain* swapchain) {
    if (!swapchain_is_init(swapchain) || swapchain->image_count == 0) {
        return FAILED_CREATE_SWAPCHAIN_IMAGE_VIEWS;
    }

    swapchain->init_image_view_count = 0;
    VkResult status;
    for (uint32_t i = 0; i < swapchain->image_count; i++) {
        VkImageViewCreateInfo view_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = 0,
            .flags = 0,
            .image = swapchain->images[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = swapchain->image_format,
            .components =
                (VkComponentMapping){
                    .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .a = VK_COMPONENT_SWIZZLE_IDENTITY,
                },
            .subresourceRange =
                (VkImageSubresourceRange){
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
        };
        status = vkCreateImageView(swapchain->device->handle, &view_info, NULL, &swapchain->image_views[i]);
        ASSERT_VK_LOG(status, "Unable to get swapchain image view", FAILED_CREATE_SWAPCHAIN_IMAGE_VIEWS);
        swapchain->init_image_view_count += 1;
    }

    return SWAPCHAIN_NO_ERROR;
}

SwapchainError swapchain_acquire_next_frame(Swapchain* swapchain, VkSemaphore semaphore) {
    if (!swapchain_is_init(swapchain)) {
        return FAILED_SWAPCHAIN_ACQUIRE_IMAGE;
    }

    VkResult status = vkAcquireNextImageKHR(
        swapchain->device->handle, swapchain->handle, UINT64_MAX, semaphore, NULL, &swapchain->image_index);
    if (status == VK_SUCCESS) {
        return SWAPCHAIN_NO_ERROR;
    }
    if (status == VK_ERROR_OUT_OF_DATE_KHR || status == VK_SUBOPTIMAL_KHR) {
        return SWAPCHAIN_EXPIRED;
    }

    return FAILED_SWAPCHAIN_ACQUIRE_IMAGE;
}

void swapchain_destroy(Swapchain* swapchain) {
    if (!swapchain_is_init(swapchain)) {
        return;
    }
    for (uint32_t i = 0; i < swapchain->init_image_view_count; i++) {
        vkDestroyImageView(swapchain->device->handle, swapchain->image_views[i], NULL);
    }
    vkDestroySwapchainKHR(swapchain->device->handle, swapchain->handle, NULL);
}
