#include "./command_pool.h"

#include "../../../core/string/string.h"
#include "../errors.h"
#include "../functions.h"

void command_pool_clear(CommandPool* pool) {
    pool->handle = VK_NULL_HANDLE;
    pool->device = NULL;
    string_copy("", pool->name, COMMAND_POOL_NAME_SIZE);
}

void command_pool_copy(const CommandPool* src, CommandPool* dst, bool destroy_dst) {
    if (destroy_dst) {
        command_pool_destroy(dst);
    }
    dst->handle = src->handle;
    dst->device = src->device;
    string_copy(src->name, dst->name, COMMAND_POOL_NAME_SIZE);
}

bool command_pool_is_init(const CommandPool* pool) { return pool->handle != VK_NULL_HANDLE && pool->device != NULL; }

bool command_pool_reset(CommandPool* pool) {
    if (!command_pool_is_init(pool)) {
        return false;
    }

    VkResult status = vkResetCommandPool(pool->device->handle, pool->handle, 0);
    ASSERT_VULKAN_STATUS(status, "Unable to reset command pool", false);

    return true;
}

void command_pool_destroy(CommandPool* pool) {
    if (!command_pool_is_init(pool)) {
        return;
    }
    vkDestroyCommandPool(pool->device->handle, pool->handle, NULL);
    command_pool_clear(pool);
}
