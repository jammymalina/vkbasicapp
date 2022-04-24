#include "./memory_context.h"

#include "../../../core/memory/memory.h"

void memory_context_destroy(MemoryContext* context) {
    for (size_t i = 0; i < context->buffer_object_count; ++i) {
        vulkan_buffer_object_destroy(&context->buffer_objects[i]);
    }
    if (context->buffer_objects != NULL) {
        mem_free(context->buffer_objects);
    }
    vulkan_memory_allocator_destroy(&context->allocator);
    memory_context_clear(context);
}
