#ifndef MEMORY_CONTEXT_H
#define MEMORY_CONTEXT_H

#include "../device/device.h"
#include "./allocator.h"
#include "./buffer/buffer_object.h"
#include "./memory_allocation_cache/memory_allocation_cache.h"

typedef struct MemoryContext {
    const Device* device;

    VulkanMemoryAllocator allocator;
    MemoryAllocationCache allocation_cache;
} MemoryContext;

static inline void memory_context_clear(MemoryContext* context) {
    context->device = NULL;
    vulkan_memory_allocator_clear(&context->allocator);
    memory_allocation_cache_clear(&context->allocation_cache);
}

MemoryContextError memory_context_allocate_buffer(
    MemoryContext* context, const char* name, VulkanBufferObjectInfo* buffer_info);
const VulkanBufferObject* memory_context_get_buffer(const MemoryContext* context, const char* name);

void memory_context_destroy(MemoryContext* context);

#endif
