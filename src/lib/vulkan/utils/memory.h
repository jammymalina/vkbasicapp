#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../core/device/device.h"
#include "../core/memory/allocation_blocks.h"

uint32_t memory_utils_find_memory_type_index(const Device* device, uint32_t memory_type_bits, VulkanMemoryUsage usage);
VkDeviceSize memory_utils_get_buffer_granularity_from_device(const Device* device);

#endif
