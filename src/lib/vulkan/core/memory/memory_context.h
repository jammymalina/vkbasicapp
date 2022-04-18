#ifndef VULKAN_MEMORY_CONTEXT_H
#define VULKAN_MEMORY_CONTEXT_H

#include "../device/device.h"
#include "./allocator.h"

typedef struct VulkanMemoryContext {
    const Device* device;
    VulkanMemoryAllocator allocator;
} VulkanMemoryContext;

static inline void vulkan_memory_context_clear(VulkanMemoryContext* context) {
    vulkan_memory_allocator_clear(&context->allocator);
}

void vulkan_memory_context_destroy(VulkanMemoryContext* context);

#endif
