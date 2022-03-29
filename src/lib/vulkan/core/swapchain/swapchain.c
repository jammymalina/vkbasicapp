#include "./swapchain.h"

#include "../functions.h"

bool swapchain_is_init(const Swapchain* swapchain) {
    return swapchain->device != NULL && swapchain->handle != VK_NULL_HANDLE;
}

void swapchain_destroy(Swapchain* swapchain) {
    if (!swapchain_is_init(swapchain)) {
        return;
    }
    vkDestroySwapchainKHR(swapchain->device->handle, swapchain->handle, NULL);
}
