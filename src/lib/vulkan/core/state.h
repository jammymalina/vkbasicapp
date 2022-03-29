#ifndef STATE_H
#define STATE_H

#include <stdbool.h>

#include "../core/device/device.h"
#include "../core/instance.h"
#include "../core/library.h"
#include "../core/physical_device/physical_device.h"
#include "../core/swapchain/swapchain.h"
#include "../core/system_info/system_info.h"

typedef struct VulkanState {
    Library library;
    SystemInfo system;
    Instance instance;
    PhysicalDevice physical_device;
    Device device;
    Swapchain swapchain;
    bool is_init;
} VulkanState;

static inline void vulkan_state_clear(VulkanState* state) {
    library_clear(&state->library);
    system_info_clear(&state->system);
    instance_clear(&state->instance);
    physical_device_destroy(&state->physical_device);
    device_clear(&state->device);
    swapchain_clear(&state->swapchain);
    state->is_init = false;
}

#endif
