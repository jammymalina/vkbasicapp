#include "./command_context.h"

#include "command_pool.h"

void command_context_clear(CommandContext* command_context) {
    command_context->context = NULL;
    command_pool_cache_clear(&command_context->command_pool_cache);
}

void command_context_init(CommandContext* command_context, const Context* context) {
    command_context_clear(command_context);
    command_context->context = context;
}

bool command_context_is_init(const CommandContext* command_context) { return command_context->context != NULL; }

bool command_context_add_command_pool(CommandContext* command_context, const CommandPoolInitInfo* info) {
    if (!command_context_is_init(command_context)) {
        return false;
    }

    CommandPool pool;
    bool status = command_pool_init(&pool, command_context->context, info);
    if (!status) {
        return false;
    }

    status = command_pool_cache_add(&command_context->command_pool_cache, &pool);
    if (!status) {
        command_pool_destroy(&pool);
    }

    return status;
}

bool command_context_start_recording(CommandContext* command_context, const char* pool_name, uint32_t buffer_index) {
    return true;
}

bool command_context_start_secondary_recording(
    CommandContext* command_context, const char* pool_name, uint32_t buffer_index) {
    return true;
}

bool command_context_end_recording(CommandContext* command_context, const char* pool_name, uint32_t buffer_index) {
    return true;
}

bool command_context_end_secondary_recording(
    CommandContext* command_context, const char* pool_name, uint32_t buffer_index) {
    return true;
}

void command_context_destroy(CommandContext* command_context) {
    command_pool_cache_destroy(&command_context->command_pool_cache);
}
