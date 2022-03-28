#ifndef SWAPCHAIN_BUILDER_H
#define SWAPCHAIN_BUILDER_H

#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../../core/device/device.h"

typedef struct SwapchainBuilder {
    const Device* device;

    VkSwapchainKHR old_swapchain;
    VkSwapchainCreateFlagBitsKHR flags;

    VkImageUsageFlags image_usage_flags;
    VkFormatFeatureFlags format_feature_flags;

    uint32_t desired_width, desired_height;
    uint32_t array_layer_count;
} SwapchainBuilder;

void swapchain_builder_clear(SwapchainBuilder* builder) {
    builder->device = NULL;

    builder->old_swapchain = VK_NULL_HANDLE;
    builder->flags = 0;

    builder->image_usage_flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    builder->format_feature_flags = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;

    builder->desired_width = 256;
    builder->desired_height = 256;
    builder->array_layer_count = 1;
}

#endif
