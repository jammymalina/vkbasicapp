#ifndef COMMAND_POOL_CACHE_H
#define COMMAND_POOL_CACHE_H

#include <stdint.h>

#include "./command_pool.h"

#define COMMAND_POOL_CACHE_MAX_POOLS 32
#define CACHE_KEY_AVAILABLE "_AVAIL"

typedef struct CommandPoolCache {
    CommandPool items[COMMAND_POOL_CACHE_MAX_POOLS];
} CommandPoolCache;

void command_pool_cache_clear(CommandPoolCache* cache);
bool command_pool_cache_add(CommandPoolCache* cache, const CommandPool* pool);
CommandPool* command_pool_get(CommandPoolCache* cache, const char* name);
bool command_pool_cache_has(const CommandPoolCache* cache, const char* name);
bool command_pool_cache_remove(CommandPoolCache* cache, const char* name);
void command_pool_cache_destroy(CommandPoolCache* cache);

#endif
