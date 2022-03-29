#ifndef COMMAND_CONTEXT_H
#define COMMAND_CONTEXT_H

#include <stdbool.h>
#include <stdint.h>

#include "../context/context.h"
#include "./command_pool.h"
#include "./command_pool_cache.h"

typedef struct CommandContext {
    const Context* context;

    CommandPoolCache command_pool_cache;
} CommandContext;

void command_context_clear(CommandContext* command_context);
void command_context_init(CommandContext* command_context, const Context* context);
bool command_context_is_init(const CommandContext* command_context);

bool command_context_add_command_pool(CommandContext* command_context, const CommandPoolInitInfo* info);

bool command_context_start_recording(CommandContext* command_context, const char* pool_name, uint32_t buffer_index);
bool command_context_start_secondary_recording(
    CommandContext* command_context, const char* pool_name, uint32_t buffer_index);
bool command_context_end_recording(CommandContext* command_context, const char* pool_name, uint32_t buffer_index);
bool command_context_end_secondary_recording(
    CommandContext* command_context, const char* pool_name, uint32_t buffer_index);

void command_context_destroy(CommandContext* command_context);

#endif
