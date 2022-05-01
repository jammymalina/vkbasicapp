#include "./memory_allocation_cache.h"

#include "../../../../core/memory/memory.h"

void memory_allocation_cache_record_destroy(MemoryAllocationCacheRecord* record) {
    if (memory_allocation_cache_record_is_empty(record)) {
        return;
    }
    if (record->type == MEMORY_ALLOCACTION_CACHE_BUFFER_RECORD) {
        vulkan_buffer_object_destroy(&record->buffer_object);
    }

    memory_allocation_cache_record_clear(record);
}

bool memory_allocation_cache_init(MemoryAllocationCache* cache, size_t cache_size) {
    MemoryAllocationCacheRecord* records = mem_alloc(sizeof(MemoryAllocationCacheRecord) * cache_size);
    ASSERT_ALLOC(records, "Unable to allocate vulkan memory allocation cache", false);

    cache->records = records;
    cache->records_size = cache_size;

    for (size_t i = 0; i < cache->records_size; ++i) {
        memory_allocation_cache_record_clear(&cache->records[i]);
    }

    return true;
}

const MemoryAllocationCacheRecord* memory_allocation_cache_get_record(
    const MemoryAllocationCache* cache, const char* name, MemoryAllocationCacheRecordType type) {
    for (size_t i = 0; i < cache->records_size; ++i) {
        if (string_equals(cache->records[i].name, name) && cache->records[i].type == type) {
            return &cache->records[i];
        }
    }
    return NULL;
}

bool memory_allocation_cache_add_buffer_record(MemoryAllocationCache* cache, const char* name,
    const VulkanBufferObject* buffer_object, const VulkanAllocation* allocation) {
    size_t i;
    for (i = 0; i < cache->records_size; ++i) {
        if (memory_allocation_cache_record_is_empty(&cache->records[i])) {
            break;
        }
    }

    if (i >= cache->records_size) {
        return false;
    }

    string_copy(name, cache->records[i].name, MEMORY_ALLOCATION_NAME_MAX_SIZE);
    cache->records[i].type = MEMORY_ALLOCACTION_CACHE_BUFFER_RECORD;
    vulkan_allocation_copy(allocation, &cache->records[i].allocation);
    vulkan_buffer_object_copy(buffer_object, &cache->records[i].buffer_object);

    return true;
}

void memory_allocation_cache_destroy(MemoryAllocationCache* cache) {
    if (cache->records != NULL) {
        mem_free(cache->records);
    }

    memory_allocation_cache_clear(cache);
}
