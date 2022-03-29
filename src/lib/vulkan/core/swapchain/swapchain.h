#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../device/device.h"
#include "./surface_support_details.h"

typedef struct Swapchain {
    VkSwapchainKHR handle;
    const Device* device;

    uint32_t image_count;
    VkFormat image_format;
    VkExtent2D extent;
} Swapchain;

static inline void swapchain_clear(Swapchain* swapchain) {
    swapchain->handle = VK_NULL_HANDLE;
    swapchain->device = NULL;

    swapchain->image_count = 0;
    swapchain->image_format = VK_FORMAT_UNDEFINED;
    swapchain->extent = (VkExtent2D){.width = 0, .height = 0};
}

bool swapchain_is_init(const Swapchain* swapchain);
void swapchain_destroy(Swapchain* swapchain);

#endif
