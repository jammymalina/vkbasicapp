#ifndef MEMORY_CONTEXT_BUILDER_H
#define MEMORY_CONTEXT_BUILDER_H

#include "../../core/memory/allocator.h"
#include "../../core/memory/memory_context.h"

typedef struct MemoryContextBuilder {
    const Device* device;

    VulkanMemoryAllocatorInfo allocator_info;
} MemoryContextBuilder;

static inline void memory_context_builder_clear(MemoryContextBuilder* builder) {
    builder->device = NULL;
    builder->allocator_info = vulkan_memory_allocator_info_get_default();
}

int memory_context_builder_set_config_value(MemoryContextBuilder* builder, const char* name, const char* value);
MemoryContextError memory_context_builder_build(MemoryContextBuilder* builder, MemoryContext* context);

#endif
