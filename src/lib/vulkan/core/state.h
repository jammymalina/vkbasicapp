#ifndef VULKAN_CORE_APP_H
#define VULKAN_CORE_APP_H

#include <stdbool.h>

#include "../core/instance.h"
#include "../core/library.h"
#include "../core/physical_device/physical_device.h"
#include "../core/system_info/system_info.h"

typedef struct VulkanState {
    Library library;
    SystemInfo system;
    Instance instance;
    PhysicalDevice physical_device;
    bool is_init;
} VulkanState;

static inline void vulkan_state_clear(VulkanState* state) {
    library_clear(&state->library);
    system_info_clear(&state->system);
    instance_clear(&state->instance);
    physical_device_clear(&state->physical_device);
    state->is_init = false;
}

#endif
