#ifndef SWAPCHAIN_BUILDER_H
#define SWAPCHAIN_BUILDER_H

#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../../core/device/device.h"
#include "../../core/errors.h"
#include "../../core/swapchain/surface_support_details.h"
#include "../../core/swapchain/swapchain.h"

typedef struct SwapchainBuilder {
    const Device* device;

    VkSwapchainKHR old_swapchain;
    VkSwapchainCreateFlagBitsKHR flags;

    uint32_t desired_width, desired_height;
    uint32_t array_layer_count;

    VkSurfaceFormatKHR desired_formats[SURFACE_MAX_FORMATS];
    uint32_t desired_format_count;

    VkPresentModeKHR desired_present_modes[SURFACE_MAX_PRESENT_MODES];
    uint32_t desired_present_mode_count;

    VkImageUsageFlags image_usage_flags;
    VkFormatFeatureFlags format_feature_flags;
    VkSurfaceTransformFlagBitsKHR pre_transform;
    VkCompositeAlphaFlagBitsKHR composite_alpha;

    uint32_t combined_queue;

    bool clipped;
} SwapchainBuilder;

static inline void swapchain_builder_clear(SwapchainBuilder* builder) {
    builder->device = NULL;

    builder->old_swapchain = VK_NULL_HANDLE;
    builder->flags = 0;

    builder->desired_width = 256;
    builder->desired_height = 256;
    builder->array_layer_count = 1;

    builder->desired_format_count = 0;
    builder->desired_present_mode_count = 0;

    builder->image_usage_flags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    builder->format_feature_flags = VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT;
    builder->pre_transform = (VkSurfaceTransformFlagBitsKHR){0};
    builder->composite_alpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    builder->combined_queue = UINT32_MAX;

    builder->clipped = true;
}

SwapchainError swapchain_builder_build(SwapchainBuilder* builder, Swapchain* swapchain);
bool swapchain_builder_add_desired_format(SwapchainBuilder* builder, VkSurfaceFormatKHR format);
bool swapchain_builder_add_default_formats(SwapchainBuilder* builder);
bool swapchain_builder_add_desired_present_mode(SwapchainBuilder* builder, VkPresentModeKHR present_mode);
bool swapchain_builder_add_default_present_modes(SwapchainBuilder* builder);

#endif
