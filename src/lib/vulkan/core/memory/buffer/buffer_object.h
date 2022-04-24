#ifndef BUFFER_OBJECT_H
#define BUFFER_OBJECT_H

#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../../../../core/utils/flags.h"
#include "../../device/device.h"
#include "../../errors.h"

typedef uint32_t VKBOPropertyFlags;

typedef enum VKBOPropertyFlagBits {
    VKBO_DYNAMIC_USAGE_BIT = FLAG_CREATE(1),
    VKBO_STATIC_USAGE_BIT = FLAG_CREATE(2),
    VKBO_VERTEX_BIT = FLAG_CREATE(3),
    VKBO_UNIFORM_BIT = FLAG_CREATE(4),
    VKBO_INDEX_BIT = FLAG_CREATE(5),
} VKBOPropertyFlagBits;

typedef struct VulkanBufferObjectInfo {
    const Device* device;
    VkDeviceSize size;
    VKBOPropertyFlagBits flags;
} VulkanBufferObjectInfo;

typedef struct VulkanBufferObject {
    const Device* device;

    VkBuffer handle;

    VKBOPropertyFlags property_flags;
    VkDeviceSize size;
} VulkanBufferObject;

void vulkan_buffer_object_clear(VulkanBufferObject* buff) {
    buff->device = NULL;
    buff->handle = VK_NULL_HANDLE;
}

MemoryContextError vulkan_buffer_object_init(VulkanBufferObject* buff, const VulkanBufferObjectInfo* info);

void vulkan_buffer_object_destroy(VulkanBufferObject* buff);

#endif
