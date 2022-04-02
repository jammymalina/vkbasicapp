#include "./command_pool.h"

#include "../../../core/string/string.h"
#include "../errors.h"
#include "../functions.h"

void command_pool_clear(CommandPool* pool) {
    pool->handle = VK_NULL_HANDLE;
    pool->context = NULL;
    string_copy(COMMAND_POOL_AVAILABLE_KEY, pool->name, COMMAND_POOL_NAME_SIZE);
    pool->queue_family_index = UINT32_MAX;
    pool->primary_buffer_count = 0;
    pool->secondary_buffer_count = 0;
    for (uint32_t i = 0; i < COMMAND_POOL_MAX_BUFFERS; ++i) {
        pool->primary_buffers[i] = VK_NULL_HANDLE;
    }
    for (uint32_t i = 0; i < COMMAND_POOL_MAX_BUFFERS; ++i) {
        pool->secondary_buffers[i] = VK_NULL_HANDLE;
    }
}

bool command_pool_init(CommandPool* pool, const Context* context, const CommandPoolInitInfo* info) {
    command_pool_clear(pool);
    string_copy(info->name, pool->name, COMMAND_POOL_NAME_SIZE);
    pool->context = context;
    pool->queue_family_index = info->queue_family_index;

    VkCommandPoolCreateFlags flags = 0;
    if (info->transient) {
        flags |= VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    }
    if (info->reset_enabled) {
        flags |= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    }
    if (info->protected) {
        flags |= VK_COMMAND_POOL_CREATE_PROTECTED_BIT;
    }

    VkCommandPoolCreateInfo pool_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = NULL,
        .flags = flags,
        .queueFamilyIndex = pool->queue_family_index,
    };

    VkCommandPool handle;
    VkResult status = vkCreateCommandPool(pool->context->device.handle, &pool_info, NULL, &handle);
    ASSERT_VK_LOG(status, "Unable to create command pool", false);
    pool->handle = handle;

    bool buffer_status = true;
    if (info->primary_buffer_count > 0) {
        buffer_status &= command_pool_add_primary_buffers(pool, info->primary_buffer_count);
    }
    if (buffer_status && info->secondary_buffer_count > 0) {
        buffer_status &= command_pool_add_secondary_buffers(pool, info->secondary_buffer_count);
    }

    return buffer_status;
}

bool command_pool_is_init(const CommandPool* pool) { return pool->handle != VK_NULL_HANDLE && pool->context != NULL; }

void command_pool_copy(const CommandPool* src, CommandPool* dst, bool destroy_dst) {
    if (destroy_dst) {
        command_pool_destroy(dst);
    }
    dst->handle = src->handle;
    dst->context = src->context;
    string_copy(src->name, dst->name, COMMAND_POOL_NAME_SIZE);
    dst->queue_family_index = src->queue_family_index;
    dst->primary_buffer_count = src->primary_buffer_count;
    dst->secondary_buffer_count = src->secondary_buffer_count;
    for (uint32_t i = 0; i < src->primary_buffer_count; ++i) {
        dst->primary_buffers[i] = src->primary_buffers[i];
    }
    for (uint32_t i = 0; i < dst->secondary_buffer_count; ++i) {
        dst->secondary_buffers[i] = src->secondary_buffers[i];
    }
}

VkCommandBuffer command_pool_get_primary_buffer(const CommandPool* pool, uint32_t index) {
    if (index >= pool->primary_buffer_count) {
        return VK_NULL_HANDLE;
    }
    return pool->primary_buffers[index];
}

VkCommandBuffer command_pool_get_secondary_buffer(const CommandPool* pool, uint32_t index) {
    if (index >= pool->secondary_buffer_count) {
        return VK_NULL_HANDLE;
    }
    return pool->secondary_buffers[index];
}

bool command_pool_add_primary_buffers(CommandPool* pool, uint32_t count) {
    if (!command_pool_is_init(pool)) {
        return false;
    }
    if (count == 0 || pool->primary_buffer_count + count > COMMAND_POOL_MAX_BUFFERS) {
        return false;
    }

    VkCommandBufferAllocateInfo buffer_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = pool->handle,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = count,
    };
    VkResult status = vkAllocateCommandBuffers(
        pool->context->device.handle, &buffer_info, &pool->primary_buffers[pool->primary_buffer_count]);
    ASSERT_VK_LOG(status, "Unable to allocate primary command buffers", false);
    pool->primary_buffer_count += count;

    return true;
}

bool command_pool_add_secondary_buffers(CommandPool* pool, uint32_t count) {
    if (!command_pool_is_init(pool)) {
        return false;
    }
    if (count == 0 || pool->secondary_buffer_count + count > COMMAND_POOL_MAX_BUFFERS) {
        return false;
    }

    VkCommandBufferAllocateInfo buffer_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = NULL,
        .commandPool = pool->handle,
        .level = VK_COMMAND_BUFFER_LEVEL_SECONDARY,
        .commandBufferCount = count,
    };
    VkResult status = vkAllocateCommandBuffers(pool->context->device.handle, &buffer_info, pool->secondary_buffers);
    ASSERT_VK_LOG(status, "Unable to allocate secondary command buffers", false);
    pool->secondary_buffer_count += count;

    return true;
}

void command_pool_free_buffers(CommandPool* pool) {
    if (!command_pool_is_init(pool)) {
        return;
    }

    VkDevice device = pool->context->device.handle;
    if (pool->secondary_buffer_count > 0) {
        vkFreeCommandBuffers(device, pool->handle, pool->secondary_buffer_count, pool->secondary_buffers);
        pool->secondary_buffer_count = 0;
        for (uint32_t i = 0; i < COMMAND_POOL_MAX_BUFFERS; ++i) {
            pool->secondary_buffers[i] = VK_NULL_HANDLE;
        }
    }

    if (pool->primary_buffer_count > 0) {
        vkFreeCommandBuffers(device, pool->handle, pool->primary_buffer_count, pool->primary_buffers);
        pool->primary_buffer_count = 0;
        for (uint32_t i = 0; i < COMMAND_POOL_MAX_BUFFERS; ++i) {
            pool->primary_buffers[i] = VK_NULL_HANDLE;
        }
    }
}

bool command_pool_reset(CommandPool* pool, bool release_resources) {
    if (!command_pool_is_init(pool)) {
        return false;
    }

    VkCommandPoolResetFlags flags = 0;
    if (release_resources) {
        flags |= VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT;
    }

    VkResult status = vkResetCommandPool(pool->context->device.handle, pool->handle, flags);
    ASSERT_VK_LOG(status, "Unable to reset command pool", false);

    return true;
}

void command_pool_destroy(CommandPool* pool) {
    if (!command_pool_is_init(pool)) {
        return;
    }
    vkDestroyCommandPool(pool->context->device.handle, pool->handle, NULL);
    command_pool_clear(pool);
}
