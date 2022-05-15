#ifndef MEMORY_ALLOCATION_CACHE_H
#define MEMORY_ALLOCATION_CACHE_H

#include <sys/types.h>
#define MEMORY_ALLOCATION_NAME_MAX_SIZE 128
#define MEMORY_ALLOCATION_FREE_NAME "_AVAIL"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../../../../core/string/string.h"
#include "../allocation_blocks.h"
#include "../buffer/buffer_object.h"

typedef enum MemoryAllocationCacheRecordType {
    MEMORY_ALLOCACTION_CACHE_BUFFER_RECORD,
    MEMORY_ALLOCACTION_CACHE_UKNOWN_RECORD,
} MemoryAllocationCacheRecordType;

typedef struct MemoryAllocationCacheRecord {
    char name[MEMORY_ALLOCATION_NAME_MAX_SIZE];
    uint32_t page_index;

    MemoryAllocationCacheRecordType type;
    VulkanAllocation allocation;
    union {
        VulkanBufferObject buffer_object;
    };
} MemoryAllocationCacheRecord;

static inline void memory_allocation_cache_record_clear(MemoryAllocationCacheRecord* record) {
    string_copy(MEMORY_ALLOCATION_FREE_NAME, record->name, MEMORY_ALLOCATION_NAME_MAX_SIZE);
    record->page_index = 0;
    record->type = MEMORY_ALLOCACTION_CACHE_UKNOWN_RECORD;
    vulkan_allocation_clear(&record->allocation);
}

static inline bool memory_allocation_cache_record_is_empty(const MemoryAllocationCacheRecord* record) {
    return string_equals(record->name, MEMORY_ALLOCATION_FREE_NAME);
}

static inline bool memory_allocation_cache_record_page_equals(const MemoryAllocationCacheRecord* record,
    const char* name, MemoryAllocationCacheRecordType type, uint32_t page_index) {
    return string_equals(record->name, name) && record->type == type && record->page_index == page_index;
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

static inline uint32_t memory_allocation_cache_get_page_count(
    const MemoryAllocationCache* cache, const char* name, MemoryAllocationCacheRecordType type) {
    uint32_t page_count = 0;
    for (size_t i = 0; i < cache->records_size; ++i) {
        const MemoryAllocationCacheRecord* record = &cache->records[i];
        if (string_equals(record->name, name) && record->type == type) {
            ++page_count;
        }
    }
    return page_count;
}

bool memory_allocation_cache_init(MemoryAllocationCache* cache, size_t cache_size);

const MemoryAllocationCacheRecord* memory_allocation_cache_get_record(
    const MemoryAllocationCache* cache, const char* name, MemoryAllocationCacheRecordType type, uint32_t page_index);
bool memory_allocation_cache_remove_record(
    MemoryAllocationCache* cache, const char* name, MemoryAllocationCacheRecordType type, uint32_t page_index);

static inline const MemoryAllocationCacheRecord* memory_allocation_cache_get_buffer(
    const MemoryAllocationCache* cache, const char* name, uint32_t page_index) {
    return memory_allocation_cache_get_record(cache, name, MEMORY_ALLOCACTION_CACHE_BUFFER_RECORD, page_index);
}

bool memory_allocation_cache_add_buffer_record(MemoryAllocationCache* cache, const char* name,
    const VulkanBufferObject* buffer_object, const VulkanAllocation* allocation);

void memory_allocation_cache_destroy(MemoryAllocationCache* cache);

#endif
