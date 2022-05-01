#include "./memory_context_builder.h"

#include <stddef.h>
#include <vulkan/vulkan.h>

#include "../../../core/logger/logger.h"
#include "../../../core/parsers/ini_parser.h"
#include "../../../core/string/string.h"

int memory_context_builder_set_config_value(MemoryContextBuilder* builder, const char* name, const char* value) {
    if (string_equals(name, "device_local_block_size_MB")) {
        INI_PARSER_ASSERT_INT("memory_context", name, value, false, 1);
        builder->allocator_info.device_local_block_size_MB = string_to_int(value, VkDeviceSize);
        return 1;
    }

    if (string_equals(name, "host_visible_block_size_MB")) {
        INI_PARSER_ASSERT_INT("memory_context", name, value, false, 1);
        builder->allocator_info.host_visible_block_size_MB = string_to_int(value, VkDeviceSize);
        return 1;
    }

    if (string_equals(name, "garbage_list_count")) {
        INI_PARSER_ASSERT_INT("memory_context", name, value, false, 1);
        builder->allocator_info.garbage_list_count = string_to_int(value, size_t);
        return 1;
    }

    if (string_equals(name, "allocation_cache_size")) {
        INI_PARSER_ASSERT_INT("allocation_cache_size", name, value, false, 1);
        size_t cache_size = string_to_int(value, size_t);
        if (cache_size == 0) {
            log_warning("Memory allocation cache size must be greater than 0");
            return 1;
        }
        builder->allocation_cache_size = cache_size;
        return 1;
    }

    return 1;
}

MemoryContextError memory_context_builder_build(MemoryContextBuilder* builder, MemoryContext* context) {
    memory_context_clear(context);

    if (builder->device == NULL) {
        return MEMORY_CONTEXT_DEVICE_NOT_PROVIDED;
    }

    builder->allocator_info.device = builder->device;
    MemoryContextError status = vulkan_memory_allocator_init(&context->allocator, &builder->allocator_info);
    ASSERT_SUCCESS(status, status);

    if (!memory_allocation_cache_init(&context->allocation_cache, builder->allocation_cache_size)) {
        return MEMORY_CONTEXT_INIT_ERROR;
    }

    return MEMORY_CONTEXT_SUCCESS;
}
