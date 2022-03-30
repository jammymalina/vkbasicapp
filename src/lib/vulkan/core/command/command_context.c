#include "./command_context.h"

static inline VkCommandBuffer command_context_get_buffer(
    CommandContext* command_context, const char* pool_name, uint32_t buffer_index, bool secondary) {
    if (!command_context_is_init(command_context)) {
        return VK_NULL_HANDLE;
    }

    CommandPool* pool = command_pool_cache_get(&command_context->command_pool_cache, pool_name);
    if (!pool) {
        return VK_NULL_HANDLE;
    }

    if (secondary) {
        return command_pool_get_secondary_buffer(pool, buffer_index);
    }
    return command_pool_get_primary_buffer(pool, buffer_index);
}

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

bool command_context_remove_command_pool(CommandContext* command_context, const char* name) {
    if (!command_context_is_init(command_context)) {
        return false;
    }

    return command_pool_cache_remove(&command_context->command_pool_cache, name);
}

VkCommandBuffer command_context_get_command_buffer(
    CommandContext* command_context, const char* pool_name, const CommandBufferInfo* info) {
    VkCommandBuffer buffer =
        command_context_get_buffer(command_context, pool_name, info->buffer_index, info->secondary);
    return buffer;
}

void command_context_destroy(CommandContext* command_context) {
    command_pool_cache_destroy(&command_context->command_pool_cache);
}
