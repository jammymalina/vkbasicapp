#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include <stdbool.h>
#include <vulkan/vulkan.h>

typedef struct CommandBuffer {
    VkCommandBuffer handle;
} CommandBuffer;

void command_buffer_clear(CommandBuffer* buffer);
void command_buffer_copy(const CommandBuffer* src, CommandBuffer* dst, bool destroy_dst);
bool command_buffer_is_init(const CommandBuffer* buffer);
bool command_buffer_reset(CommandBuffer* buffer);

#endif
