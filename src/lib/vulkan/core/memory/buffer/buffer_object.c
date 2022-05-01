#include "./buffer_object.h"

#include "../../../../core/memory/memory.h"
#include "../../functions.h"

static VkBufferUsageFlags vulkan_buffer_get_usage_flags(VKBOPropertyFlags flags) {
    VkBufferUsageFlags usage = 0;
    if (FLAGS_CHECK_FLAG(flags, VKBO_STATIC_USAGE_BIT)) {
        usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }
    if (FLAGS_CHECK_FLAG(flags, VKBO_VERTEX_BIT)) {
        usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    }
    if (FLAGS_CHECK_FLAG(flags, VKBO_UNIFORM_BIT)) {
        usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    }
    if (FLAGS_CHECK_FLAG(flags, VKBO_INDEX_BIT)) {
        usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    }
    return usage;
}

MemoryContextError vulkan_buffer_object_init(VulkanBufferObject* buff, const VulkanBufferObjectInfo* info) {
    vulkan_buffer_object_clear(buff);

    if (info->device == NULL) {
        return MEMORY_CONTEXT_DEVICE_NOT_PROVIDED;
    }
    buff->device = info->device;

    if (info->size == 0 || is_16_byte_aligned_size(info->size)) {
        return MEMORY_CONTEXT_INVALID_BUFFER_SIZE;
    }

    VkResult status = VK_SUCCESS;

    VkBuffer buffer_handle;
    VkBufferCreateInfo buffer_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .size = info->size,
        .usage = vulkan_buffer_get_usage_flags(info->flags),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = NULL,
    };
    status = vkCreateBuffer(buff->device->handle, &buffer_info, NULL, &buffer_handle);
    ASSERT_VK_LOG(status, "Unable to create buffer", MEMORY_CONTEXT_BUFFER_INIT_ERROR);

    buff->handle = buffer_handle;
    buff->size = info->size;
    buff->property_flags = info->flags;

    VkBufferMemoryRequirementsInfo2 memory_req_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2,
        .pNext = NULL,
        .buffer = buff->handle,
    };
    vkGetBufferMemoryRequirements2(buff->device->handle, &memory_req_info, &buff->memory_requirements);

    return MEMORY_CONTEXT_SUCCESS;
}

void vulkan_buffer_object_copy(const VulkanBufferObject* src, VulkanBufferObject* dst) {
    vulkan_buffer_object_clear(dst);
    dst->device = src->device;
    dst->handle = src->handle;
    dst->memory_requirements = src->memory_requirements;
    dst->property_flags = src->property_flags;
    dst->size = src->size;
}

void vulkan_buffer_object_destroy(VulkanBufferObject* buff) {
    if (buff->device == NULL) {
        return;
    }
    if (buff->handle != VK_NULL_HANDLE) {
        vkDestroyBuffer(buff->device->handle, buff->handle, NULL);
    }
    vulkan_buffer_object_clear(buff);
}
