#include "./memory_context.h"

#include "../../../core/memory/memory.h"
#include "allocator.h"
#include "memory_allocation_cache/memory_allocation_cache.h"

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
