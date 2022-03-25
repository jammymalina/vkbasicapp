#include "./instance_builder.h"

#include <SDL2/SDL_vulkan.h>

#include "../../../core/logger/logger.h"
#include "../../../core/memory/memory.h"
#include "../../core/functions.h"

static InstanceError instance_builder_get_extensions(InstanceBuilder* builder) {
    if (builder->window_handle == NULL) {
        log_error("Unable to get available instance extensions, builder does not have window set");
        return WINDOWING_EXTENSIONS_NOT_PRESENT;
    }

    if (!SDL_Vulkan_GetInstanceExtensions(builder->window_handle, &builder->available_extension_count, NULL)) {
        log_error("SDL_Vulkan_GetInstanceExtensions(): %s", SDL_GetError());
        return WINDOWING_EXTENSIONS_NOT_PRESENT;
    }

    builder->available_extensions = mem_alloc(sizeof(const char*) * builder->available_extension_count);
    ASSERT_ALLOC(builder->available_extensions, "Failed to allocate memory for vulkan extensions",
        WINDOWING_EXTENSIONS_NOT_PRESENT);

    if (!SDL_Vulkan_GetInstanceExtensions(
            builder->window_handle, &builder->available_extension_count, builder->available_extensions)) {
        log_error("SDL_Vulkan_GetInstanceExtensions(): %s", SDL_GetError());
        return WINDOWING_EXTENSIONS_NOT_PRESENT;
    }

    log_info("Available instance extensions: %d", builder->available_extension_count);
    for (size_t i = 0; i < builder->available_extension_count; i++) {
        log_info("%s", builder->available_extensions[i]);
    }

    return INSTANCE_NO_ERROR;
}

InstanceError instance_builder_build(InstanceBuilder* builder, Instance* instance) {
    InstanceError status = instance_builder_get_extensions(builder);
    if (status) {
        return status;
    }

    VkApplicationInfo vk_application_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = builder->app_name,
        .applicationVersion = builder->application_version,
        .pEngineName = builder->engine_name,
        .engineVersion = builder->engine_version,
        .apiVersion = VK_API_VERSION_1_3,
    };

    VkInstanceCreateInfo vk_instance_create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .pApplicationInfo = &vk_application_info,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL,
        .enabledExtensionCount = 0,
        .ppEnabledExtensionNames = NULL,
    };

    VkInstance handle = VK_NULL_HANDLE;
    VkResult result = vkCreateInstance(&vk_instance_create_info, NULL, &handle);
    if (result != VK_SUCCESS || instance == VK_NULL_HANDLE) {
        handle = VK_NULL_HANDLE;
        status = FAILED_CREATE_INSTANCE;
        log_error("Unable to create the vulkan instance");
    }

    instance->handle = handle;

    mem_free(builder->available_extensions);
    builder->available_extensions = NULL;
    builder->available_extension_count = 0;

    return status;
}
