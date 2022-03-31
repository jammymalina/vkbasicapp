#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../device/device.h"
#include "../errors.h"
#include "../queue/queue.h"
#include "./surface_support_details.h"

#define SWAPCHAIN_MAX_IMAGES 16

typedef struct Swapchain {
    VkSwapchainKHR handle;
    const Device* device;

    uint32_t image_count;
    VkFormat image_format;
    VkExtent2D extent;

    VkImage images[SWAPCHAIN_MAX_IMAGES];
    bool init_images;
    VkImageView image_views[SWAPCHAIN_MAX_IMAGES];
    uint32_t init_image_view_count;

    Queue queue;

    uint32_t image_index;
} Swapchain;

static inline void swapchain_clear(Swapchain* swapchain) {
    swapchain->handle = VK_NULL_HANDLE;
    swapchain->device = NULL;

    swapchain->image_count = 0;
    swapchain->image_format = VK_FORMAT_UNDEFINED;
    swapchain->extent = (VkExtent2D){.width = 0, .height = 0};
    swapchain->init_images = false;
    swapchain->init_image_view_count = 0;

    queue_clear(&swapchain->queue);

    swapchain->image_index = 0;
}

bool swapchain_is_init(const Swapchain* swapchain);
void swapchain_copy(const Swapchain* src, Swapchain* dst);

SwapchainError swapchain_load_images(Swapchain* swapchain);
SwapchainError swapchain_load_image_views(Swapchain* swapchain);

SwapchainError swapchain_acquire_next_image(Swapchain* swapchain, VkSemaphore semaphore);

void swapchain_destroy(Swapchain* swapchain);

#endif
