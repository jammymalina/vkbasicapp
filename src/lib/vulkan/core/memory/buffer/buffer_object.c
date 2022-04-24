#include "./buffer_object.h"

#include "../../../../core/memory/memory.h"
#include "../../functions.h"

MemoryContextError vulkan_buffer_object_init(VulkanBufferObject* buff, const VulkanBufferObjectInfo* info) {
    if (info->size == 0 || is_16_byte_aligned_size(info->size)) {
        return MEMORY_CONTEXT_INVALID_BUFFER_SIZE;
    }

    VkBufferCreateInfo buffer_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .size = info->size,
        .usage = 0,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = NULL,
    };

    return MEMORY_CONTEXT_SUCCESS;
}

void vulkan_buffer_object_destroy(VulkanBufferObject* buff) {
    if (buff->device == NULL) {
        return;
    }
    vkDestroyBuffer(buff->device->handle, buff->handle, NULL);
    vulkan_buffer_object_clear(buff);
}
