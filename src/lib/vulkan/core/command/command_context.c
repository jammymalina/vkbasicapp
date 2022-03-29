#include "./command_context.h"

void command_context_clear(CommandContext* command_context) {
    command_context->context = NULL;
    command_pool_cache_clear(&command_context->command_pool_cache);
}

bool command_context_is_init(const CommandContext* command_context) { return command_context->context != NULL; }

void command_context_destroy(CommandContext* command_context) {
    command_pool_cache_destroy(&command_context->command_pool_cache);
}
