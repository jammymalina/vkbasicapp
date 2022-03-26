#ifndef VULKAN_CORE_INSTANCE_H
#define VULKAN_CORE_INSTANCE_H

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "../core/functions.h"

typedef struct Instance {
    VkInstance handle;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkSurfaceKHR surface;
    uint32_t api_version;
} Instance;

static inline Instance instance_create() {
    Instance instance = {
        .handle = VK_NULL_HANDLE,
        .debug_messenger = VK_NULL_HANDLE,
        .surface = VK_NULL_HANDLE,
        .api_version = 0,
    };
    return instance;
}

static inline void instance_destroy(Instance* instance) {
    if (instance->handle == VK_NULL_HANDLE) {
        return;
    }
    if (instance->surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(instance->handle, instance->surface, NULL);
        instance->surface = VK_NULL_HANDLE;
    }
#ifdef DEBUG
    if (instance->debug_messenger != VK_NULL_HANDLE) {
        vkDestroyDebugUtilsMessengerEXT(instance->handle, instance->debug_messenger, VK_NULL_HANDLE);
        instance->debug_messenger = VK_NULL_HANDLE;
    }
#endif
    vkDestroyInstance(instance->handle, VK_NULL_HANDLE);
    instance->handle = VK_NULL_HANDLE;
}

#endif
