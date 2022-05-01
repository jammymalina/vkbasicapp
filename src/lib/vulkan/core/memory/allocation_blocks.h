#ifndef ALLOCATION_BLOCKS_H
#define ALLOCATION_BLOCKS_H

#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../../../core/collections/vector.h"
#include "../../../core/memory/memory.h"
#include "../../core/device/device.h"
#include "../../core/errors.h"

typedef enum VulkanMemoryUsage {
    VULKAN_MEMORY_USAGE_UNKNOWN,
    VULKAN_MEMORY_USAGE_GPU_ONLY,
    VULKAN_MEMORY_USAGE_CPU_ONLY,
    VULKAN_MEMORY_USAGE_CPU_TO_GPU,
    VULKAN_MEMORY_USAGE_GPU_TO_CPU,
    VULKAN_MEMORY_USAGES_TOTAL,
} VulkanMemoryUsage;

typedef enum VulkanAllocationType {
    VULKAN_ALLOCATION_TYPE_FREE,
    VULKAN_ALLOCATION_TYPE_BUFFER,
    VULKAN_ALLOCATION_TYPE_IMAGE,
    VULKAN_ALLOCATION_TYPE_IMAGE_LINEAR,
    VULKAN_ALLOCATION_TYPE_IMAGE_OPTIMAL,
    VULKAN_ALLOCATION_TYPES_TOTAL,
} VulkanAllocationType;

typedef struct VulkanMemoryBlock VulkanMemoryBlock;
typedef struct VulkanMemoryBlockChunk VulkanMemoryBlockChunk;

typedef struct VulkanAllocationInfo {
    uint32_t size;
    uint32_t align;
    VkDeviceSize granularity;
    VulkanAllocationType allocation_type;
} VulkanAllocationInfo;

typedef struct VulkanAllocation {
    uint32_t id;

    VulkanMemoryBlock* block;

    VkDeviceMemory device_memory_handle;
    VkDeviceSize offset;
    VkDeviceSize size;

    byte* data;
} VulkanAllocation;

typedef struct VulkanAllocationList VECTOR(VulkanAllocation) VulkanAllocationList;

static inline void vulkan_allocation_clear(VulkanAllocation* allocation) {
    allocation->id = 0;
    allocation->block = NULL;
    allocation->device_memory_handle = VK_NULL_HANDLE;
    allocation->offset = 0;
    allocation->size = 0;
    allocation->data = NULL;
}

static inline void vulkan_allocation_copy(const VulkanAllocation* src, VulkanAllocation* dst) {
    vulkan_allocation_clear(dst);
    dst->id = src->id;
    dst->block = src->block;
    dst->device_memory_handle = src->device_memory_handle;
    dst->offset = src->offset;
    dst->size = src->size;
    dst->data = src->data;
}

typedef struct VulkanMemoryBlockInfo {
    const Device* device;
    uint32_t memory_type_index;
    VkDeviceSize size;
    VulkanMemoryUsage usage;
} VulkanMemoryBlockInfo;

struct VulkanMemoryBlockChunk {
    uint32_t id;

    VkDeviceSize size;
    VkDeviceSize offset;

    VulkanMemoryBlockChunk* prev;
    VulkanMemoryBlockChunk* next;

    VulkanAllocationType type;
};

static inline void vulkan_memory_block_chunk_clear(VulkanMemoryBlockChunk* chunk) {
    chunk->id = 0;
    chunk->size = 0;
    chunk->offset = 0;
    chunk->prev = NULL;
    chunk->next = NULL;
    chunk->type = VULKAN_ALLOCATION_TYPE_FREE;
}

struct VulkanMemoryBlock {
    const Device* device;
    VulkanMemoryBlockChunk* head;
    uint32_t next_block_id;

    uint32_t memory_type_index;
    VulkanMemoryUsage usage;
    VkDeviceMemory device_memory_handle;

    VkDeviceSize size;
    VkDeviceSize allocated;
    bool is_data_mapped;
    byte* data;
};

typedef struct VulkanMemoryBlockList VECTOR(VulkanMemoryBlock*) VulkanMemoryBlockList;

static inline void vulkan_memory_block_clear(VulkanMemoryBlock* block) {
    block->device = NULL;
    block->head = NULL;
    block->next_block_id = 0;
    block->memory_type_index = 0;
    block->usage = VULKAN_MEMORY_USAGE_UNKNOWN;
    block->device_memory_handle = VK_NULL_HANDLE;
    block->size = 0;
    block->allocated = 0;
    block->is_data_mapped = false;
    block->data = NULL;
}

MemoryContextError vulkan_memory_block_init(VulkanMemoryBlock* block, const VulkanMemoryBlockInfo* info);
MemoryContextError vulkan_memory_block_allocate(
    VulkanMemoryBlock* block, const VulkanAllocationInfo* alloc_info, VulkanAllocation* allocation);
void vulkan_memory_block_free_allocation(VulkanMemoryBlock* block, VulkanAllocation* allocation);

static inline bool vulkan_memory_block_is_host_visible(const VulkanMemoryBlock* block) {
    return block->usage != VULKAN_MEMORY_USAGE_GPU_ONLY;
}

void vulkan_memory_block_destroy(VulkanMemoryBlock* block);

#endif
