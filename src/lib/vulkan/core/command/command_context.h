#ifndef COMMAND_CONTEXT_H
#define COMMAND_CONTEXT_H

#include <stdbool.h>

#include "../context/context.h"
#include "./command_pool_cache.h"

typedef struct CommandContext {
    const Context* context;

    CommandPoolCache command_pool_cache;
} CommandContext;

void command_context_clear(CommandContext* command_context);
bool command_context_is_init(const CommandContext* command_context);
void command_context_destroy(CommandContext* command_context);

#endif
