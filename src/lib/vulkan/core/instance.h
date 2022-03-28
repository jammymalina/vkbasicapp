#ifndef VULKAN_CORE_INSTANCE_H
#define VULKAN_CORE_INSTANCE_H

#include <stdbool.h>
#include <vulkan/vulkan.h>

#include "./functions.h"

typedef struct Instance {
    VkInstance handle;
    VkDebugUtilsMessengerEXT debug_messenger;
    VkSurfaceKHR surface;
    uint32_t api_version;
    bool loaded_instance_functions;
} Instance;

static inline void instance_clear(Instance* instance) {
    instance->handle = VK_NULL_HANDLE;
    instance->debug_messenger = VK_NULL_HANDLE;
    instance->surface = VK_NULL_HANDLE;
    instance->api_version = 0;
    instance->loaded_instance_functions = false;
}

static inline bool instance_is_init(const Instance* instance) {
    return instance->loaded_instance_functions && instance->handle != VK_NULL_HANDLE;
}

static inline void instance_destroy(Instance* instance) {
    if (!instance_is_init(instance)) {
        return;
    }
    if (instance->surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(instance->handle, instance->surface, NULL);
    }
#ifdef DEBUG
    if (instance->debug_messenger != VK_NULL_HANDLE) {
        vkDestroyDebugUtilsMessengerEXT(instance->handle, instance->debug_messenger, VK_NULL_HANDLE);
    }
#endif
    vkDestroyInstance(instance->handle, VK_NULL_HANDLE);
    instance_clear(instance);
}

#endif
