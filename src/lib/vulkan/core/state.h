#ifndef VULKAN_CORE_APP_H
#define VULKAN_CORE_APP_H

#include <stdbool.h>

#include "../core/instance.h"
#include "../core/library.h"
#include "../core/system_info/system_info.h"

typedef struct VulkanState {
    Library library;
    SystemInfo system;
    Instance instance;
    bool is_init;
} VulkanState;

static inline VulkanState vulkan_state_create() {
    VulkanState state = {
        .library = library_create(),
        .system = system_info_create(),
        .instance = instance_create(),
        .is_init = false,
    };
    return state;
}

#endif
