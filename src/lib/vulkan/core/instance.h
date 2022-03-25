#ifndef VULKAN_CORE_INSTANCE_H
#define VULKAN_CORE_INSTANCE_H

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "../core/functions.h"

typedef struct Instance {
    VkInstance handle;
    VkDebugUtilsMessengerEXT debug_messenger;
} Instance;

static inline Instance instance_create() {
    Instance instance = {.handle = VK_NULL_HANDLE, .debug_messenger = VK_NULL_HANDLE};
    return instance;
}

static inline void instance_destroy(Instance* instance) {
    if (instance->handle == VK_NULL_HANDLE) {
        return;
    }
#ifdef DEBUG
    if (instance->debug_messenger != VK_NULL_HANDLE) {
        // vkDestroyDebugUtilsMessengerEXT(instance->handle, instance->debug_messenger, VK_NULL_HANDLE);
    }
#endif
    vkDestroyInstance(instance->handle, VK_NULL_HANDLE);
    instance->handle = VK_NULL_HANDLE;
}

#endif
