#ifndef DEVICE_H
#define DEVICE_H

#include <vulkan/vulkan.h>

#include "../functions.h"
#include "../physical_device/physical_device.h"

typedef struct Device {
    VkDevice handle;
    const PhysicalDevice* physical_device;
    bool loaded_device_functions;
} Device;

static inline void device_clear(Device* device) {
    device->handle = VK_NULL_HANDLE;
    device->physical_device = NULL;
    device->loaded_device_functions = false;
}

static inline bool device_is_init(const Device* device) {
    return device->loaded_device_functions && device->handle != VK_NULL_HANDLE;
}

static inline void device_destroy(Device* device) {
    if (!device_is_init(device)) {
        return;
    }
    vkDestroyDevice(device->handle, NULL);
    device_clear(device);
}

#endif
