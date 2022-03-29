#ifndef COMMAND_POOL_H
#define COMMAND_POOL_H

#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../device/device.h"
#include "./command_buffer.h"

#define COMMAND_POOL_NAME_SIZE 256
#define COMMAND_POOL_MAX_BUFFERS 10

typedef struct CommandPool {
    VkCommandPool handle;
    const Device* device;

    char name[COMMAND_POOL_NAME_SIZE];

    CommandBuffer buffers[COMMAND_POOL_MAX_BUFFERS];
    uint32_t buffer_count;
} CommandPool;

void command_pool_clear(CommandPool* pool);
void command_pool_copy(const CommandPool* src, CommandPool* dst, bool destroy_dst);
bool command_pool_is_init(const CommandPool* pool);
bool command_pool_reset(CommandPool* pool);
void command_pool_destroy(CommandPool* pool);

#endif
