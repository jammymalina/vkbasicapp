#include "./allocator.h"

#include "../../../core/memory/memory.h"
#include "../../utils/memory.h"

static void vulkan_memory_allocator_empty_garbage_index(VulkanMemoryAllocator* allocator, size_t index) {
    VulkanAllocationList* garbage = &allocator->garbage_lists[index];

    for (size_t i = 0; i < garbage->size; ++i) {
        VulkanAllocation* allocation = &garbage->data[i];

        vulkan_memory_block_free_allocation(allocation->block, allocation);

        if (allocation->block->allocated == 0) {
            VulkanMemoryBlockList* blocks = &allocator->blocks[allocation->block->memory_type_index];
            ssize_t index;
            vector_index_of(blocks, allocation->block, &index);
            if (index != -1) {
                vector_swap_remove(blocks, index);
                mem_free(allocation->block);
            }
        }

        vulkan_allocation_clear(allocation);
    }

    vector_empty_noshrink(garbage);
}

MemoryContextError vulkan_memory_allocator_init(
    VulkanMemoryAllocator* allocator, const VulkanMemoryAllocatorInfo* allocator_info) {
    vulkan_memory_allocator_clear(allocator);

    if (allocator_info->device == NULL) {
        return MEMORY_CONTEXT_DEVICE_NOT_PROVIDED;
    }

    size_t reserve_size = 10;
    bool status = true;

    for (uint32_t i = 0; i < VK_MAX_MEMORY_TYPES && status; ++i) {
        status = vector_reserve(&allocator->blocks[i], reserve_size);
    }
    if (!status) {
        return MEMORY_CONTEXT_INIT_ERROR;
    }

    if (allocator_info->garbage_list_count > 0) {
        VulkanAllocationList* garbage_lists =
            mem_alloc(sizeof(VulkanAllocationList) * allocator_info->garbage_list_count);
        ASSERT_ALLOC(garbage_lists, "Unable to create garbage block lists", MEMORY_CONTEXT_INIT_ERROR);

        for (size_t i = 0; i < allocator_info->garbage_list_count && status; ++i) {
            status = vector_reserve(&garbage_lists[i], reserve_size);
        }
        if (!status) {
            mem_free(garbage_lists);
            return MEMORY_CONTEXT_INIT_ERROR;
        }

        allocator->garbage_list_count = allocator_info->garbage_list_count;
        allocator->garbage_lists = garbage_lists;
    }

    allocator->device = allocator_info->device;
    allocator->device_local_block_size_bytes = MB_TO_BYTES(allocator_info->device_local_block_size_MB);
    allocator->host_visible_block_size_bytes = MB_TO_BYTES(allocator_info->host_visible_block_size_MB);

    return MEMORY_CONTEXT_SUCCESS;
}

MemoryContextError vulkan_memory_allocator_allocate(
    VulkanMemoryAllocator* allocator, const VulkanMemoryAllocatorRequest* req, VulkanAllocation* allocation) {
    vulkan_allocation_clear(allocation);

    uint32_t memory_type_index =
        memory_utils_find_memory_type_index(allocator->device, req->memory_type_bits, req->usage);
    if (memory_type_index == UINT32_MAX) {
        return MEMORY_CONTEXT_NO_SUITABLE_MEMORY_INDEX;
    }

    VulkanAllocationInfo allocation_info = {
        .size = req->size,
        .align = req->align,
        .granularity = memory_utils_get_buffer_granularity_from_device(allocator->device),
        .allocation_type = req->allocation_type,
    };

    VulkanMemoryBlockList* blocks = &allocator->blocks[memory_type_index];
    MemoryContextError status;
    for (size_t i = 0; i < blocks->size; ++i) {
        VulkanMemoryBlock* block = blocks->data[i];

        if (block->memory_type_index != memory_type_index) {
            continue;
        }

        status = vulkan_memory_block_allocate(block, &allocation_info, allocation);
        if (status == MEMORY_CONTEXT_SUCCESS) {
            return MEMORY_CONTEXT_SUCCESS;
        }

        if (status != MEMORY_CONTEXT_LOW_FREE_BLOCK_SPACE && status != MEMORY_CONTEXT_UNSUITABLE_BLOCK) {
            return status;
        }
    }

    VkDeviceSize block_size = allocator->host_visible_block_size_bytes;
    if (req->usage == VULKAN_MEMORY_USAGE_GPU_ONLY) {
        block_size = allocator->device_local_block_size_bytes;
    }

    VulkanMemoryBlock* block = mem_alloc(sizeof(VulkanMemoryBlock));
    ASSERT_ALLOC(block, "Unable to create an allocation block", MEMORY_CONTEXT_INIT_ERROR);

    VulkanMemoryBlockInfo block_info = {
        .device = allocator->device,
        .memory_type_index = memory_type_index,
        .size = block_size,
        .usage = req->usage,
    };
    status = vulkan_memory_block_init(block, &block_info);
    if (status != MEMORY_CONTEXT_SUCCESS) {
        mem_free(block);
        return status;
    }
    if (!vector_push(blocks, block)) {
        vulkan_memory_block_destroy(block);
        mem_free(block);
        return MEMORY_CONTEXT_UNABLE_TO_ADD_BLOCK;
    }

    status = vulkan_memory_block_allocate(block, &allocation_info, allocation);

    return status;
}

bool vulkan_memory_allocator_free(VulkanMemoryAllocator* allocator, VulkanAllocation allocation) {
    if (allocator->garbage_list_count <= 0) {
        return false;
    }
    return vector_push(&allocator->garbage_lists[allocator->garbage_list_index], allocation);
}

void vulkan_memory_allocator_empty_garbage(VulkanMemoryAllocator* allocator) {
    if (allocator->garbage_list_count <= 0) {
        return;
    }

    allocator->garbage_list_index = (allocator->garbage_list_index + 1) % allocator->garbage_list_count;

    vulkan_memory_allocator_empty_garbage_index(allocator, allocator->garbage_list_index);
}

void vulkan_memory_allocator_destroy(VulkanMemoryAllocator* allocator) {
    for (size_t i = 0; i < allocator->garbage_list_count; ++i) {
        vulkan_memory_allocator_empty_garbage_index(allocator, i);
        vector_clear(&allocator->garbage_lists[i]);
    }
    mem_free(allocator->garbage_lists);
    allocator->garbage_list_count = 0;

    for (uint32_t memory_index = 0; memory_index < VK_MAX_MEMORY_TYPES; ++memory_index) {
        VulkanMemoryBlockList* blocks = &allocator->blocks[memory_index];
        for (size_t i = 0; i < blocks->size; ++i) {
            vulkan_memory_block_destroy(blocks->data[i]);
            mem_free(blocks->data[i]);
        }
        vector_clear(blocks);
    }

    vulkan_memory_allocator_clear(allocator);
}
