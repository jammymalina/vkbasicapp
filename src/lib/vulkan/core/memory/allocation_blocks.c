#include "./allocation_blocks.h"

#include "../../../core/memory/memory.h"
#include "../../../core/utils/macro.h"
#include "../../core/functions.h"

static bool vulkan_memory_block_is_on_same_page(
    VkDeviceSize ra_offset, VkDeviceSize ra_size, VkDeviceSize rb_offset, VkDeviceSize page_size) {
    if (ra_offset + ra_size > rb_offset || ra_size <= 0 || page_size <= 0) {
        return false;
    }

    VkDeviceSize ra_end = ra_offset + ra_size - 1;
    VkDeviceSize ra_end_page = ra_end & ~(page_size - 1);
    VkDeviceSize rb_start = rb_offset;
    VkDeviceSize rb_start_page = rb_start & ~(page_size - 1);

    return ra_end_page == rb_start_page;
}

static bool vulkan_memory_block_has_granularity_conflict(VulkanAllocationType type1, VulkanAllocationType type2) {
    if (type1 > type2) {
        SWAP_VALUES(type1, type2);
    }

    switch (type1) {
        case VULKAN_ALLOCATION_TYPE_FREE:
            return false;
        case VULKAN_ALLOCATION_TYPE_BUFFER:
            return type2 == VULKAN_ALLOCATION_TYPE_IMAGE || type2 == VULKAN_ALLOCATION_TYPE_IMAGE_OPTIMAL;
        case VULKAN_ALLOCATION_TYPE_IMAGE:
            return type2 == VULKAN_ALLOCATION_TYPE_IMAGE || type2 == VULKAN_ALLOCATION_TYPE_IMAGE_LINEAR ||
                   type2 == VULKAN_ALLOCATION_TYPE_IMAGE_OPTIMAL;
        case VULKAN_ALLOCATION_TYPE_IMAGE_LINEAR:
            return type2 == VULKAN_ALLOCATION_TYPE_IMAGE_OPTIMAL;
        case VULKAN_ALLOCATION_TYPE_IMAGE_OPTIMAL:
            return false;
        default:
            return true;
    }
}

MemoryContextError vulkan_memory_block_init(VulkanMemoryBlock* block, const VulkanMemoryBlockInfo* info) {
    vulkan_memory_block_clear(block);
    block->device = info->device;
    block->memory_type_index = info->memory_type_index;
    block->usage = info->usage;
    block->size = info->size;

    if (block->memory_type_index == UINT32_MAX) {
        return MEMORY_CONTEXT_INVALID_MEMORY_INDEX;
    }

    VkMemoryAllocateInfo mem_info = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = NULL,
        .allocationSize = block->size,
        .memoryTypeIndex = block->memory_type_index,
    };

    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkResult status;

    status = vkAllocateMemory(block->device->handle, &mem_info, NULL, &memory);
    ASSERT_VK_LOG(status, "Unable to allocate memory for the block", MEMORY_CONTEXT_ALLOCATION_ERROR);

    if (memory == VK_NULL_HANDLE) {
        log_error("Unable to allocate memory for the block - received null handle");
        return MEMORY_CONTEXT_ALLOCATION_ERROR;
    }
    block->device_memory_handle = memory;

    if (vulkan_memory_block_is_host_visible(block)) {
        status = vkMapMemory(block->device->handle, memory, 0, block->size, 0, (void**)&block->data);
        ASSERT_VK_LOG(status, "Unable to map the block memory", MEMORY_CONTEXT_MAPPING_ERROR);
        block->is_data_mapped = true;
    }

    block->head = mem_alloc(sizeof(VulkanMemoryBlockChunk));
    ASSERT_ALLOC(block->head, "Unable to create memory block's head", MEMORY_CONTEXT_INIT_ERROR);
    vulkan_memory_block_chunk_clear(block->head);
    block->head->id = block->next_block_id++;
    block->head->size = block->size;

    return MEMORY_CONTEXT_SUCCESS;
}

MemoryContextError vulkan_memory_block_allocate(
    VulkanMemoryBlock* block, const VulkanAllocationInfo* alloc_info, VulkanAllocation* allocation) {
    vulkan_allocation_clear(allocation);

    VkDeviceSize free_size = block->size - block->allocated;

    if (free_size < alloc_info->size) {
        return MEMORY_CONTEXT_LOW_FREE_BLOCK_SPACE;
    }

    VulkanMemoryBlockChunk* iter = NULL;
    VulkanMemoryBlockChunk* best_fit = NULL;
    VulkanMemoryBlockChunk* prev = NULL;

    VkDeviceSize padding = 0;
    VkDeviceSize offset = 0;
    VkDeviceSize aligned_size = 0;

    for (iter = block->head; iter != NULL; prev = iter, iter = iter->next) {
        if (iter->type != VULKAN_ALLOCATION_TYPE_FREE) {
            continue;
        }
        if (iter->size < alloc_info->size) {
            continue;
        }

        offset = ALIGN(iter->offset, alloc_info->align);

        // Check for linear/optimal granularity conflict with previous allocation
        if (prev != NULL && alloc_info->granularity > 1) {
            if (vulkan_memory_block_is_on_same_page(prev->offset, prev->size, offset, alloc_info->granularity)) {
                if (vulkan_memory_block_has_granularity_conflict(prev->type, alloc_info->allocation_type)) {
                    offset = ALIGN(offset, alloc_info->granularity);
                }
            }
        }

        padding = offset - iter->offset;
        aligned_size = alloc_info->size + padding;

        if (iter->size < aligned_size) {
            continue;
        }
        if (block->size <= aligned_size + block->allocated) {
            return MEMORY_CONTEXT_LOW_FREE_BLOCK_SPACE;
        }

        if (alloc_info->granularity > 1 && iter->next != NULL) {
            if (vulkan_memory_block_is_on_same_page(
                    offset, alloc_info->size, iter->next->offset, alloc_info->granularity)) {
                if (vulkan_memory_block_has_granularity_conflict(alloc_info->allocation_type, iter->next->type)) {
                    continue;
                }
            }
        }

        best_fit = iter;
        break;
    }

    if (best_fit == NULL) {
        return MEMORY_CONTEXT_UNSUITABLE_BLOCK;
    }

    // Split the free space into a new block chunk
    if (best_fit->size > alloc_info->size) {
        VulkanMemoryBlockChunk* chunk = mem_alloc(sizeof(VulkanMemoryBlockChunk));
        ASSERT_ALLOC(chunk, "Unable to allocate memory to split the block", MEMORY_CONTEXT_INIT_ERROR);
        vulkan_memory_block_chunk_clear(chunk);

        VulkanMemoryBlockChunk* next = best_fit->next;

        chunk->id = block->next_block_id++;
        chunk->prev = best_fit;
        best_fit->next = chunk;

        chunk->next = next;
        if (next != NULL) {
            next->prev = chunk;
        }

        chunk->size = best_fit->size - aligned_size;
        chunk->offset = offset + alloc_info->size;
        chunk->type = VULKAN_ALLOCATION_TYPE_FREE;
    }

    best_fit->type = alloc_info->allocation_type;
    best_fit->size = alloc_info->size;

    block->allocated += aligned_size;

    allocation->size = best_fit->size;
    allocation->id = best_fit->id;
    allocation->device_memory_handle = block->device_memory_handle;
    if (vulkan_memory_block_is_host_visible(block)) {
        allocation->data = block->data + offset;
    }
    allocation->offset = offset;
    allocation->block = block;

    return MEMORY_CONTEXT_SUCCESS;
}

void vulkan_memory_block_free_allocation(VulkanMemoryBlock* block, VulkanAllocation* allocation) {
    VulkanMemoryBlockChunk* iter = NULL;
    for (iter = block->head; iter != NULL; iter = iter->next) {
        if (iter->id == allocation->id) {
            break;
        }
    }

    if (iter == NULL) {
        log_warning("Tried to free an unknown allocation");
        return;
    }

    iter->type = VULKAN_ALLOCATION_TYPE_FREE;

    // Merge with the previous block chunk if it's empty
    if (iter->prev != NULL && iter->prev->type == VULKAN_ALLOCATION_TYPE_FREE) {
        VulkanMemoryBlockChunk* prev = iter->prev;

        prev->next = iter->next;
        if (iter->next != NULL) {
            iter->next->prev = prev;
        }
        prev->size += iter->size;
        mem_free(iter);
        iter = prev;
    }

    // Merge with the next block chunk if it's empty
    if (iter->next != NULL && iter->next == VULKAN_ALLOCATION_TYPE_FREE) {
        VulkanMemoryBlockChunk* next = iter->next;
        if (next->next != NULL) {
            next->next->prev = iter;
        }
        iter->next = next->next;
        iter->size += next->size;
        mem_free(next);
    }

    block->allocated -= allocation->size;
}

void vulkan_memory_block_destroy(VulkanMemoryBlock* block) {
    if (block->device == NULL) {
        return;
    }

    if (block->is_data_mapped) {
        vkUnmapMemory(block->device->handle, block->device_memory_handle);
    }

    if (block->device_memory_handle != VK_NULL_HANDLE) {
        vkFreeMemory(block->device->handle, block->device_memory_handle, NULL);
    }

    VulkanMemoryBlockChunk* prev = NULL;
    VulkanMemoryBlockChunk* current = block->head;

    while (current != NULL) {
        prev = current;
        current = current->next;
        mem_free(prev);
    }

    vulkan_memory_block_clear(block);
}
