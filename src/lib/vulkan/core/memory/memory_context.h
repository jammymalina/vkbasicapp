#ifndef VULKAN_MEMORY_CONTEXT_H
#define VULKAN_MEMORY_CONTEXT_H

#include "../device/device.h"
#include "./allocator.h"
#include "./buffer/buffer_object.h"

typedef struct MemoryContext {
    const Device* device;

    VulkanMemoryAllocator allocator;

    VulkanBufferObject* buffer_objects;
    size_t buffer_object_count;
} MemoryContext;

static inline void memory_context_clear(MemoryContext* context) {
    vulkan_memory_allocator_clear(&context->allocator);
    context->buffer_objects = NULL;
    context->buffer_object_count = 0;
}

void memory_context_destroy(MemoryContext* context);

#endif
