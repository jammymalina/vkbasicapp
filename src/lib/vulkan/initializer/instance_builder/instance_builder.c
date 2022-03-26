#include "./instance_builder.h"

#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan_core.h>

#include "../../../core/logger/logger.h"
#include "../../../core/memory/memory.h"
#include "../../core/errors.h"
#include "../../core/functions.h"
#include "../../initializer/function_loader/function_loader.h"

static InstanceError instance_builder_validate(const InstanceBuilder* builder) {
    if (builder->window_handle == NULL) {
        return WINDOWING_EXTENSIONS_NOT_PRESENT;
    }
    if (builder->system == NULL) {
        return NO_SYSTEM_INFO_PRESENT;
    }
    return INSTANCE_NO_ERROR;
}

static InstanceError instance_builder_load_window_extensions(InstanceBuilder* builder) {
    uint32_t count = 0;

    if (!SDL_Vulkan_GetInstanceExtensions(builder->window_handle, &count, NULL)) {
        log_error("SDL_Vulkan_GetInstanceExtensions(): %s", SDL_GetError());
        return WINDOWING_EXTENSIONS_NOT_PRESENT;
    }

    if (count + builder->extension_count > INSTANCE_BUILDER_MAX_EXTENSIONS) {
        return TOO_MANY_INSTANCE_EXTENSIONS_REQUESTED;
    }

    if (!SDL_Vulkan_GetInstanceExtensions(
            builder->window_handle, &count, &builder->extensions[builder->extension_count])) {
        log_error("SDL_Vulkan_GetInstanceExtensions(): %s", SDL_GetError());
        return WINDOWING_EXTENSIONS_NOT_PRESENT;
    }

    builder->extension_count += count;
    return INSTANCE_NO_ERROR;
}

static VkResult instance_builder_create_debug_messenger(
    const InstanceBuilder* builder, VkDebugUtilsMessengerEXT* messenger, VkInstance instance) {
    VkDebugUtilsMessengerCreateInfoEXT messenger_info = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = NULL,
        .flags = 0,
        .messageSeverity = builder->debug_message_severity,
        .messageType = builder->debug_message_severity,
        .pfnUserCallback = builder->debug_callback,
        .pUserData = NULL,
    };
    VkResult status = vkCreateDebugUtilsMessengerEXT(instance, &messenger_info, NULL, messenger);
    return status;
}

InstanceError instance_builder_build(InstanceBuilder* builder, Instance* instance) {
    InstanceError status = instance_builder_validate(builder);
    if (status != INSTANCE_NO_ERROR) {
        return status;
    }

    status = instance_builder_load_window_extensions(builder);
    if (status != INSTANCE_NO_ERROR) {
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
        .enabledExtensionCount = builder->extension_count,
        .ppEnabledExtensionNames = builder->extensions,
    };

    VkInstance handle = VK_NULL_HANDLE;
    VkResult result = vkCreateInstance(&vk_instance_create_info, NULL, &handle);
    if (result != VK_SUCCESS || instance == VK_NULL_HANDLE) {
        handle = VK_NULL_HANDLE;
        status = FAILED_CREATE_INSTANCE;
        log_error("Unable to create the vulkan instance");
    }

    instance->handle = handle;
    builder->extension_count = 0;

    if (status != INSTANCE_NO_ERROR) {
        return status;
    }

    FunctionLoaderError load_status = function_loader_load_instance_vulkan_functions(instance->handle);
    if (load_status != FUNCTION_LOADER_NO_ERROR) {
        return FAILED_TO_LOAD_INSTANCE_FUNCTIONS;
    }

    VkResult messenger_status =
        instance_builder_create_debug_messenger(builder, &instance->debug_messenger, instance->handle);
    if (messenger_status != VK_SUCCESS) {
        return FAILED_CREATE_DEBUG_MESSENGER;
    }

    return status;
}
