#include "./memory_context.h"

void vulkan_memory_context_destroy(VulkanMemoryContext* context) {
    vulkan_memory_allocator_destroy(&context->allocator);
    vulkan_memory_context_clear(context);
}
