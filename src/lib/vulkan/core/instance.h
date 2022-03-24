#ifndef VULKAN_CORE_INSTANCE_H
#define VULKAN_CORE_INSTANCE_H

#include <vulkan/vulkan.h>

#include "./errors.h"

typedef struct Instance {
    VkInstance handle;
} Instance;

static inline Instance instance_create() {
    Instance instance = {.handle = VK_NULL_HANDLE};
    return instance;
}

static inline InstanceError instance_init(Instance* instance) { return INSTANCE_NO_ERROR; }

static inline void instance_destroy(Instance* instance) {
    if (instance->handle == VK_NULL_HANDLE) {
        return;
    }
    // vkDestroyInstance(instance->handle, NULL);
    instance->handle = VK_NULL_HANDLE;
}

#endif
