#include "./function_loader.h"

#include <stdbool.h>

#include "../../../core/logger/logger.h"
#include "../../core/functions.h"

#define EXPORTED_VULKAN_FUNCTION(name) PFN_##name name = NULL;
#define GLOBAL_LEVEL_VK_FUNCTION(name) PFN_##name name = NULL;
#define INSTANCE_LEVEL_VK_FUNCTION(name) PFN_##name name = NULL;
#define INSTANCE_LEVEL_VK_FUNCTION_FROM_EXTENSION(name) PFN_##name name = NULL;
#define DEVICE_LEVEL_VK_FUNCTION(name) PFN_##name name = NULL;
#define DEVICE_LEVEL_VK_FUNCTION_FROM_EXTENSION(name, extension) PFN_##name name = NULL;

#include "../../core/function_list.h"

void function_loader_load_external_function(PFN_vkGetInstanceProcAddr vk_get_proc) {
    vkGetInstanceProcAddr = vk_get_proc;
}

FunctionLoaderError function_loader_load_global_functions() {
#define GLOBAL_LEVEL_VK_FUNCTION(name)                                                                                 \
    name = (PFN_##name)vkGetInstanceProcAddr(NULL, #name);                                                             \
    if (name == NULL) {                                                                                                \
        log_error("Could not load global level function: " #name);                                                     \
        return FUNCTION_LOADER_ERROR;                                                                                  \
    }

#include "../../core/function_list.h"
    return FUNCTION_LOADER_SUCCESS;
}

FunctionLoaderError function_loader_load_instance_vulkan_functions(VkInstance instance) {
#define INSTANCE_LEVEL_VK_FUNCTION(name)                                                                               \
    name = (PFN_##name)vkGetInstanceProcAddr(instance, #name);                                                         \
    if (name == NULL) {                                                                                                \
        log_error("Could not load instance level function: " #name);                                                   \
        return FUNCTION_LOADER_ERROR;                                                                                  \
    }

#define INSTANCE_LEVEL_VK_FUNCTION_FROM_EXTENSION(name)                                                                \
    name = (PFN_##name)vkGetInstanceProcAddr(instance, #name);                                                         \
    if (name == NULL) {                                                                                                \
        log_error("Could not load instance level function: " #name);                                                   \
        return FUNCTION_LOADER_ERROR;                                                                                  \
    }

#include "../../core/function_list.h"
    return FUNCTION_LOADER_SUCCESS;
}

FunctionLoaderError function_loader_load_device_level_functions(VkDevice device) {
#define DEVICE_LEVEL_VK_FUNCTION(name)                                                                                 \
    name = (PFN_##name)vkGetDeviceProcAddr(device, #name);                                                             \
    if (name == NULL) {                                                                                                \
        log_error("Could not load device level function: " #name);                                                     \
        return FUNCTION_LOADER_ERROR;                                                                                  \
    }                                                                                                                  \
    log_debug("Successfully loaded device level function: " #name);

#define DEVICE_LEVEL_VK_FUNCTION_FROM_EXTENSION(name, extension)                                                       \
    name = (PFN_##name)vkGetDeviceProcAddr(device, #name);                                                             \
    if (name == NULL) {                                                                                                \
        log_error("Could not load device level function: " #name);                                                     \
        return FUNCTION_LOADER_ERROR;                                                                                  \
    }

#include "../../core/function_list.h"
    return FUNCTION_LOADER_SUCCESS;
}
