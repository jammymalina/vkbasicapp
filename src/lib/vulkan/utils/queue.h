#ifndef QUEUE_UTILS_H
#define QUEUE_UTILS_H

#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../core/instance.h"
#include "../core/physical_device/physical_device.h"

/**
 * Finds the first queue which supports the desired operations. Returns UINT32_MAX if none is found
 */
uint32_t queue_utils_get_first_queue_index(const PhysicalDevice* device, VkQueueFlags desired_flags);

/**
 * Finds the first queue which supports only the desired flag (not graphics or transfer). Returns UINT32_MAX if none is
 * found
 */
uint32_t queue_utils_get_dedicated_queue_index(
    const PhysicalDevice* device, VkQueueFlags desired_flags, VkQueueFlags undesired_flags);

/**
 * Finds the queue which is separate from the graphics queue and has the desired flag and not the undesired flag, but
 * will select it if no better options are available compute support. Returns UINT32_MAX if none is found
 */
uint32_t queue_utils_get_separate_queue_index(
    const PhysicalDevice* device, VkQueueFlags desired_flags, VkQueueFlags undesired_flags);

/**
 * Finds the first queue which supports presenting and graphics. Returns UINT32_MAX
 * if none is found
 */
uint32_t queue_utils_get_present_queue_index(const PhysicalDevice* device);

#endif
