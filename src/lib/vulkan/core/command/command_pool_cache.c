#include "./command_pool_cache.h"

#include "../../../core/string/string.h"

void command_pool_cache_clear(CommandPoolCache* cache) {
    for (uint32_t i = 0; i < COMMAND_POOL_CACHE_MAX_POOLS; i++) {
        command_pool_clear(&cache->items[i]);
    }
}

bool command_pool_cache_add(CommandPoolCache* cache, const CommandPool* pool) {
    if (command_pool_cache_has(cache, pool->name)) {
        return false;
    }
    for (uint32_t i = 0; i < COMMAND_POOL_CACHE_MAX_POOLS; i++) {
        if (string_equal(cache->items[i].name, COMMAND_POOL_AVAILABLE_KEY)) {
            command_pool_copy(pool, &cache->items[i], false);
            return true;
        }
    }
    return false;
}

CommandPool* command_pool_cache_get(CommandPoolCache* cache, const char* name) {
    for (uint32_t i = 0; i < COMMAND_POOL_CACHE_MAX_POOLS; i++) {
        if (string_equal(cache->items[i].name, name)) {
            return &cache->items[i];
        }
    }
    return NULL;
}

bool command_pool_cache_has(const CommandPoolCache* cache, const char* name) {
    for (uint32_t i = 0; i < COMMAND_POOL_CACHE_MAX_POOLS; i++) {
        if (string_equal(cache->items[i].name, name)) {
            return true;
        }
    }
    return false;
}

bool command_pool_cache_remove(CommandPoolCache* cache, const char* name) {
    for (uint32_t i = 0; i < COMMAND_POOL_CACHE_MAX_POOLS; i++) {
        if (string_equal(cache->items[i].name, name)) {
            command_pool_destroy(&cache->items[i]);
            return true;
        }
    }
    return false;
}

void command_pool_cache_destroy(CommandPoolCache* cache) {
    for (uint32_t i = 0; i < COMMAND_POOL_CACHE_MAX_POOLS; i++) {
        command_pool_destroy(&cache->items[i]);
    }
}
