#ifndef COMMAND_CONTEXT_H
#define COMMAND_CONTEXT_H

#include <stdbool.h>
#include <stdint.h>

#include "../context/context.h"
#include "./command_pool.h"
#include "./command_pool_cache.h"

typedef struct CommandBufferInfo {
    uint32_t buffer_index;
    bool secondary;
} CommandBufferInfo;

typedef struct CommandContext {
    const Context* context;

    CommandPoolCache command_pool_cache;
} CommandContext;

void command_context_clear(CommandContext* command_context);
void command_context_init(CommandContext* command_context, const Context* context);
bool command_context_is_init(const CommandContext* command_context);

bool command_context_add_command_pool(CommandContext* command_context, const CommandPoolInitInfo* info);
bool command_context_remove_command_pool(CommandContext* command_context, const char* name);

VkCommandBuffer command_context_get_command_buffer(
    CommandContext* command_context, const char* pool_name, const CommandBufferInfo* info);

void command_context_destroy(CommandContext* command_context);

#endif
