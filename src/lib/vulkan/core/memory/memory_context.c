#include "./memory_context.h"

#include "../../../core/memory/memory.h"
#include "../errors.h"
#include "allocation_blocks.h"
#include "allocator.h"
#include "buffer/buffer_object.h"
#include "memory_allocation_cache/memory_allocation_cache.h"

MemoryContextError memory_context_allocate_buffer(
    MemoryContext* context, const char* name, const VulkanBufferObjectInfo* buffer_info) {
    if (memory_allocation_cache_is_full(&context->allocation_cache)) {
        return MEMORY_CONTEXT_CACHE_FULL;
    }

    VulkanBufferObject buffer;
    MemoryContextError status = vulkan_buffer_object_init(&buffer, buffer_info);
    if (status != MEMORY_CONTEXT_SUCCESS) {
        vulkan_buffer_object_destroy(&buffer);
        return status;
    }

    VulkanMemoryUsage usage = VULKAN_MEMORY_USAGE_CPU_TO_GPU;
    if (FLAGS_CHECK_FLAG(buffer.property_flags, VKBO_STATIC_USAGE_BIT)) {
        usage = VULKAN_MEMORY_USAGE_GPU_ONLY;
    }
    VulkanAllocation allocation;
    VulkanMemoryAllocatorRequest request = {
        .allocation_type = VULKAN_ALLOCATION_TYPE_BUFFER,
        .memory_type_bits = buffer.memory_requirements.memoryRequirements.memoryTypeBits,
        .align = buffer.memory_requirements.memoryRequirements.alignment,
        .size = buffer.memory_requirements.memoryRequirements.size,
        .usage = usage,
    };

    status = vulkan_memory_allocator_allocate(&context->allocator, &request, &allocation);
    if (status != MEMORY_CONTEXT_SUCCESS) {
        vulkan_buffer_object_destroy(&buffer);
        return status;
    }

    memory_allocation_cache_add_buffer_record(&context->allocation_cache, name, &buffer, &allocation);

    return MEMORY_CONTEXT_SUCCESS;
}

const VulkanBufferObject* memory_context_get_buffer(const MemoryContext* context, const char* name) {
    const MemoryAllocationCacheRecord* record =
        memory_allocation_cache_get_record(&context->allocation_cache, name, MEMORY_ALLOCACTION_CACHE_BUFFER_RECORD);
    if (record == NULL) {
        return NULL;
    }
    return &record->buffer_object;
}

void memory_context_destroy(MemoryContext* context) {
    for (size_t i = 0; i < context->allocation_cache.records_size; ++i) {
        MemoryAllocationCacheRecord* record = &context->allocation_cache.records[i];
        if (memory_allocation_cache_record_is_empty(record)) {
            continue;
        }
        vulkan_memory_allocator_free(&context->allocator, record->allocation);
        memory_allocation_cache_record_destroy(record);
    }

    memory_allocation_cache_destroy(&context->allocation_cache);
    vulkan_memory_allocator_destroy(&context->allocator);
    memory_context_clear(context);
}
