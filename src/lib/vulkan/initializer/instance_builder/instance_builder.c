#include "./instance_builder.h"

#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan_core.h>

#include "../../../core/logger/logger.h"
#include "../../../core/memory/memory.h"
#include "../../../core/string/string.h"
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

static InstanceError instance_builder_load_surface(InstanceBuilder* builder, Instance* instance) {
    VkSurfaceKHR surface;
    if (!SDL_Vulkan_CreateSurface(builder->window_handle, instance->handle, &surface)) {
        log_error("SDL_Vulkan_CreateSurface(): %s", SDL_GetError());
        return WINDOWING_EXTENSIONS_NOT_PRESENT;
    }
    instance->surface = surface;
    return INSTANCE_NO_ERROR;
}

static bool instance_builder_load_default_extensions(InstanceBuilder* builder) {
    bool status = true;
#ifdef DEBUG
    if (builder->debug_enabled && system_info_has_debug_utils(builder->system)) {
        status &= instance_builder_add_extension(builder, VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    return status;
#endif
}

static bool instance_builder_check_layers(const InstanceBuilder* builder) {
    for (uint32_t i = 0; i < builder->layer_count; i++) {
        if (!system_info_is_layer_available(builder->system, builder->layers[i])) {
            return false;
        }
    }
    return true;
}

static bool instance_builder_check_extensions(const InstanceBuilder* builder) {
    for (uint32_t i = 0; i < builder->extension_count; i++) {
        if (!system_info_is_extension_available(builder->system, builder->extensions[i])) {
            return false;
        }
    }
    return true;
}

#ifdef DEBUG
static VkResult instance_builder_create_debug_messenger(
    const InstanceBuilder* builder, VkDebugUtilsMessengerEXT* messenger, VkInstance instance) {
    VkDebugUtilsMessengerCreateInfoEXT messenger_info = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = NULL,
        .flags = 0,
        .messageSeverity = builder->debug_message_severity,
        .messageType = builder->debug_message_type,
        .pfnUserCallback = builder->debug_callback,
        .pUserData = NULL,
    };
    VkResult status = vkCreateDebugUtilsMessengerEXT(instance, &messenger_info, NULL, messenger);
    return status;
}
#endif

InstanceError instance_builder_build(InstanceBuilder* builder, Instance* instance) {
    InstanceError status = instance_builder_validate(builder);
    ASSERT_NO_ERROR(status, status);

    status = instance_builder_load_window_extensions(builder);
    ASSERT_NO_ERROR(status, status);
    if (!instance_builder_load_default_extensions(builder)) {
        return TOO_MANY_INSTANCE_EXTENSIONS_REQUESTED;
    }

    if (!instance_builder_check_extensions(builder)) {
        return REQUESTED_INSTANCE_EXTENSIONS_NOT_PRESENT;
    }
    if (!instance_builder_check_layers(builder)) {
        return REQUESTED_INSTANCE_LAYERS_NOT_PRESENT;
    }

    instance->api_version = VK_API_VERSION_1_3;
    VkApplicationInfo vk_application_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = builder->app_name,
        .applicationVersion = builder->application_version,
        .pEngineName = builder->engine_name,
        .engineVersion = builder->engine_version,
        .apiVersion = instance->api_version,
    };

    VkInstanceCreateInfo vk_instance_create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = NULL,
        .flags = 0,
        .pApplicationInfo = &vk_application_info,
        .enabledLayerCount = builder->layer_count,
        .ppEnabledLayerNames = builder->layers,
        .enabledExtensionCount = builder->extension_count,
        .ppEnabledExtensionNames = builder->extensions,
    };

    VkInstance handle = VK_NULL_HANDLE;
    VkResult result = vkCreateInstance(&vk_instance_create_info, NULL, &handle);
    if (result != VK_SUCCESS || instance == VK_NULL_HANDLE) {
        handle = VK_NULL_HANDLE;
        status = FAILED_CREATE_INSTANCE;
    }

    instance->handle = handle;
    builder->extension_count = 0;
    ASSERT_NO_ERROR(status, status);

    FunctionLoaderError load_status = function_loader_load_instance_vulkan_functions(instance->handle);
    if (load_status != FUNCTION_LOADER_NO_ERROR) {
        return FAILED_TO_LOAD_INSTANCE_FUNCTIONS;
    }

    status = instance_builder_load_surface(builder, instance);
    ASSERT_NO_ERROR(status, status);

#ifdef DEBUG
    VkResult messenger_status =
        instance_builder_create_debug_messenger(builder, &instance->debug_messenger, instance->handle);
    if (messenger_status != VK_SUCCESS) {
        return FAILED_CREATE_DEBUG_MESSENGER;
    }
#endif

    return status;
}

bool instance_builder_add_layer(InstanceBuilder* builder, const char* layer_name) {
    if (builder->layer_count >= INSTANCE_BUILDER_MAX_LAYERS) {
        return false;
    }
    builder->layers[builder->layer_count] = layer_name;
    builder->layer_count += 1;
    return true;
}

bool instance_builder_add_extension(InstanceBuilder* builder, const char* extension_name) {
    if (builder->extension_count >= INSTANCE_BUILDER_MAX_EXTENSIONS) {
        return false;
    }
    builder->extensions[builder->extension_count] = extension_name;
    builder->extension_count += 1;
    return true;
}
