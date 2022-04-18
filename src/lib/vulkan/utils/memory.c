#include "./memory.h"

#include <stdint.h>

uint32_t memory_utils_find_memory_type_index(const Device* device, uint32_t memory_type_bits, VulkanMemoryUsage usage) {
    const VkPhysicalDeviceMemoryProperties* device_memory_props = &device->physical_device->memory_properties;

    VkMemoryPropertyFlags required = 0;
    VkMemoryPropertyFlags preferred = 0;

    switch (usage) {
        case VULKAN_MEMORY_USAGE_GPU_ONLY:
            preferred |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            break;
        case VULKAN_MEMORY_USAGE_CPU_ONLY:
            required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
            break;
        case VULKAN_MEMORY_USAGE_CPU_TO_GPU:
            required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            preferred |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
            break;
        case VULKAN_MEMORY_USAGE_GPU_TO_CPU:
            required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
            preferred |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
            break;
        default:
            return UINT32_MAX;
    }

    for (uint32_t i = 0; i < device_memory_props->memoryTypeCount; ++i) {
        if (((memory_type_bits >> i) & 1) == 0) {
            continue;
        }

        const VkMemoryPropertyFlags properties = device_memory_props->memoryTypes[i].propertyFlags;
        if ((properties & required) != required) {
            continue;
        }

        if ((properties & preferred) != preferred) {
            continue;
        }

        return i;
    }

    for (uint32_t i = 0; i < device_memory_props->memoryTypeCount; ++i) {
        if (((memory_type_bits >> i) & 1) == 0) {
            continue;
        }

        const VkMemoryPropertyFlags properties = device_memory_props->memoryTypes[i].propertyFlags;
        if ((properties & required) != required) {
            continue;
        }

        return i;
    }

    return UINT32_MAX;
}

VkDeviceSize memory_utils_get_buffer_granularity_from_device(const Device* device) {
    return device->physical_device->properties.limits.bufferImageGranularity;
}
