#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdbool.h>
#include <stddef.h>
#include <vulkan/vulkan.h>

#include "../../../core/collections/vector.h"
#include "../../core/device/device.h"
#include "../../core/errors.h"
#include "./allocation_blocks.h"

typedef struct VulkanMemoryAllocatorInfo {
    const Device* device;

    VkDeviceSize device_local_block_size_MB;
    VkDeviceSize host_visible_block_size_MB;

    size_t garbage_list_count;
} VulkanMemoryAllocatorInfo;

static inline VulkanMemoryAllocatorInfo vulkan_memory_allocator_info_get_default(const Device* device) {
    return (VulkanMemoryAllocatorInfo){
        .device = device,
        .device_local_block_size_MB = 256,
        .host_visible_block_size_MB = 256,
        .garbage_list_count = 1,
    };
}

typedef struct VulkanMemoryAllocatorRequest {
    uint32_t size;
    uint32_t align;
    uint32_t memory_type_bits;
    VulkanMemoryUsage usage;
    VulkanAllocationType allocation_type;
} VulkanMemoryAllocatorRequest;

typedef struct VulkanMemoryAllocator {
    const Device* device;

    VkDeviceSize device_local_block_size_bytes;
    VkDeviceSize host_visible_block_size_bytes;

    VulkanMemoryBlockList blocks[VK_MAX_MEMORY_TYPES];

    size_t garbage_list_index;
    size_t garbage_list_count;
    VulkanAllocationList* garbage_lists;
} VulkanMemoryAllocator;

static inline void vulkan_memory_allocator_clear(VulkanMemoryAllocator* allocator) {
    allocator->device = NULL;
    allocator->garbage_list_index = 0;
    allocator->device_local_block_size_bytes = 0;
    allocator->host_visible_block_size_bytes = 0;
    allocator->garbage_list_count = 0;
    allocator->garbage_lists = NULL;

    for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES; ++i) {
        vector_init(&allocator->blocks[i]);
    }
}

MemoryContextError vulkan_memory_allocator_init(
    VulkanMemoryAllocator* allocator, const VulkanMemoryAllocatorInfo* allocator_info);

MemoryContextError vulkan_memory_allocator_allocate(
    VulkanMemoryAllocator* allocator, const VulkanMemoryAllocatorRequest* req, VulkanAllocation* allocation);
bool vulkan_memory_allocator_free(VulkanMemoryAllocator* allocator, VulkanAllocation allocation);
void vulkan_memory_allocator_empty_garbage(VulkanMemoryAllocator* allocator);

void vulkan_memory_allocator_destroy(VulkanMemoryAllocator* allocator);

#endif
