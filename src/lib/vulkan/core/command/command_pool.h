#ifndef COMMAND_POOL_H
#define COMMAND_POOL_H

#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../context/context.h"

#define COMMAND_POOL_NAME_SIZE 256
#define COMMAND_POOL_MAX_BUFFERS 10
#define COMMAND_POOL_AVAILABLE "_AVAIL"

typedef struct CommandPoolInitInfo {
    char name[COMMAND_POOL_NAME_SIZE];
    uint32_t primary_buffer_count, secondary_buffer_count;
    uint32_t queue_family_index;
    bool transient;
    bool reset_enabled;
    bool protected;
} CommandPoolInitInfo;

typedef struct CommandPool {
    VkCommandPool handle;
    const Context* context;

    char name[COMMAND_POOL_NAME_SIZE];
    uint32_t queue_family_index;

    VkCommandBuffer primary_buffers[COMMAND_POOL_MAX_BUFFERS];
    uint32_t primary_buffer_count;

    VkCommandBuffer secondary_buffers[COMMAND_POOL_MAX_BUFFERS];
    uint32_t secondary_buffer_count;
} CommandPool;

void command_pool_clear(CommandPool* pool);
bool command_pool_init(CommandPool* pool, const Context* context, const CommandPoolInitInfo* info);
bool command_pool_is_init(const CommandPool* pool);
void command_pool_copy(const CommandPool* src, CommandPool* dst, bool destroy_dst);
bool command_pool_add_primary_buffers(CommandPool* pool, uint32_t count);
bool command_pool_add_secondary_buffers(CommandPool* pool, uint32_t count);
void command_pool_free_buffers(CommandPool* pool);
bool command_pool_reset(CommandPool* pool, bool release_resources);
void command_pool_destroy(CommandPool* pool);

#endif
