#ifndef DEVICE_H
#define DEVICE_H

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include "../physical_device/physical_device.h"

typedef struct Device {
    VkDevice handle;
    const PhysicalDevice* physical_device;
} Device;

static inline void device_clear(Device* device) {
    device->handle = VK_NULL_HANDLE;
    device->physical_device = NULL;
}

void device_destroy(Device* device);

#endif
