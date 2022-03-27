#include "./queue.h"

#include <stdint.h>

#include "../core/functions.h"

uint32_t queue_utils_get_first_queue_index(
    const VkQueueFamilyProperties* families, uint32_t family_count, VkQueueFlags desired_flags) {
    for (uint32_t i = 0; i < family_count; i++) {
        if (families[i].queueFlags & desired_flags) return i;
    }
    return UINT32_MAX;
}

uint32_t queue_utils_get_dedicated_queue_index(const VkQueueFamilyProperties* families, uint32_t family_count,
    VkQueueFlags desired_flags, VkQueueFlags undesired_flags) {
    for (uint32_t i = 0; i < family_count; i++) {
        if ((families[i].queueFlags & desired_flags) && (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0 &&
            (families[i].queueFlags & undesired_flags) == 0)
            return i;
    }
    return UINT32_MAX;
}

uint32_t queue_utils_get_separate_queue_index(const VkQueueFamilyProperties* families, uint32_t family_count,
    VkQueueFlags desired_flags, VkQueueFlags undesired_flags) {
    uint32_t index = UINT32_MAX;
    for (uint32_t i = 0; i < family_count; i++) {
        if ((families[i].queueFlags & desired_flags) && ((families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)) {
            if ((families[i].queueFlags & undesired_flags) == 0) {
                return i;
            } else {
                index = i;
            }
        }
    }
    return index;
}

uint32_t queue_utils_get_present_queue_index(const PhysicalDevice* device, const Instance* instance) {
    for (uint32_t i = 0; i < device->queue_family_count; i++) {
        VkBool32 present_support = false;
        if (instance->surface != VK_NULL_HANDLE) {
            VkResult res = vkGetPhysicalDeviceSurfaceSupportKHR(device->handle, i, instance->surface, &present_support);
            if (res != VK_SUCCESS) {
                return UINT32_MAX;
            }
        }
        if (present_support) {
            return i;
        }
    }
    return UINT32_MAX;
}
