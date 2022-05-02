#ifndef MEMORY_ALLOCATION_CACHE_H
#define MEMORY_ALLOCATION_CACHE_H

#define MEMORY_ALLOCATION_NAME_MAX_SIZE 128
#define MEMORY_ALLOCATION_FREE_NAME "_AVAIL"

#include <stdbool.h>
#include <stddef.h>

#include "../../../../core/string/string.h"
#include "../allocation_blocks.h"
#include "../buffer/buffer_object.h"

typedef enum MemoryAllocationCacheRecordType {
    MEMORY_ALLOCACTION_CACHE_BUFFER_RECORD,
    MEMORY_ALLOCACTION_CACHE_UKNOWN_RECORD,
} MemoryAllocationCacheRecordType;

typedef struct MemoryAllocationCacheRecord {
    char name[MEMORY_ALLOCATION_NAME_MAX_SIZE];

    MemoryAllocationCacheRecordType type;
    VulkanAllocation allocation;
    union {
        VulkanBufferObject buffer_object;
    };
} MemoryAllocationCacheRecord;

static inline void memory_allocation_cache_record_clear(MemoryAllocationCacheRecord* record) {
    string_copy(MEMORY_ALLOCATION_FREE_NAME, record->name, MEMORY_ALLOCATION_NAME_MAX_SIZE);
    record->type = MEMORY_ALLOCACTION_CACHE_UKNOWN_RECORD;
    vulkan_allocation_clear(&record->allocation);
}

static inline bool memory_allocation_cache_record_is_empty(const MemoryAllocationCacheRecord* record) {
    return string_equals(record->name, MEMORY_ALLOCATION_FREE_NAME);
}

void memory_allocation_cache_record_destroy(MemoryAllocationCacheRecord* record);

typedef struct MemoryAllocationCache {
    MemoryAllocationCacheRecord* records;
    size_t records_size;
} MemoryAllocationCache;

static inline void memory_allocation_cache_clear(MemoryAllocationCache* cache) {
    cache->records = NULL;
    cache->records_size = 0;
}

static inline bool memory_allocation_cache_is_full(const MemoryAllocationCache* cache) {
    for (size_t i = 0; i < cache->records_size; ++i) {
        if (memory_allocation_cache_record_is_empty(&cache->records[i])) {
            return false;
        }
    }
    return true;
}

bool memory_allocation_cache_init(MemoryAllocationCache* cache, size_t cache_size);

const MemoryAllocationCacheRecord* memory_allocation_cache_get_record(
    const MemoryAllocationCache* cache, const char* name, MemoryAllocationCacheRecordType type);

bool memory_allocation_cache_add_buffer_record(MemoryAllocationCache* cache, const char* name,
    const VulkanBufferObject* buffer_object, const VulkanAllocation* allocation);

void memory_allocation_cache_destroy(MemoryAllocationCache* cache);

#endif
