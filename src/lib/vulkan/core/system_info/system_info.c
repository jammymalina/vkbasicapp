#include "./system_info.h"

#include <string.h>
#include <vulkan/vulkan.h>

#include "../../../core/logger/logger.h"
#include "../../../core/memory/memory.h"
#include "../../../core/string/string.h"
#include "../../utils/debug.h"
#include "../functions.h"

static bool system_info_load_layers(SystemInfo* info) {
    info->available_layers = NULL;
    info->available_layer_count = 0;

    uint32_t count = 0;
    VkResult status;

    status = vkEnumerateInstanceLayerProperties(&count, NULL);
    if (status != VK_SUCCESS) {
        log_error("Unable to get validation layers: %s", vulkan_result_to_string(status));
        return false;
    }

    if (count == 0) {
        return true;
    }

    VkLayerProperties* validation_layers = mem_alloc(count * sizeof(VkLayerProperties));
    ASSERT_ALLOC(validation_layers, "Unable to allocate validation layers", false);

    status = vkEnumerateInstanceLayerProperties(&count, validation_layers);

    if (status != VK_SUCCESS) {
        log_error("Unable to get validation layers: %s", vulkan_result_to_string(status));
        mem_free(validation_layers);
        return false;
    }

    info->available_layers = validation_layers;
    info->available_layer_count = count;

    return true;
}

static bool system_info_load_extensions(SystemInfo* info) {
    info->available_extensions = NULL;
    info->available_extension_count = 0;

    uint32_t count;
    VkResult status;

    VkExtensionProperties* extensions = NULL;
    do {
        status = vkEnumerateInstanceExtensionProperties(NULL, &count, NULL);
        if (status != VK_SUCCESS || count == 0) {
            break;
        }

        extensions = (VkExtensionProperties*)mem_realloc(extensions, count * sizeof(VkExtensionProperties));
        ASSERT_ALLOC(extensions, "Unable to allocate extensions", false);
        status = vkEnumerateInstanceExtensionProperties(NULL, &count, extensions);
    } while (status == VK_INCOMPLETE);

    if (status != VK_SUCCESS) {
        log_error("Unable to get extensions: %s", vulkan_result_to_string(status));
        if (extensions != NULL) {
            mem_free(extensions);
        }
        return false;
    }

    info->available_extensions = extensions;
    info->available_extension_count = count;

    return true;
}

bool system_info_init(SystemInfo* info) { return system_info_load_layers(info) && system_info_load_extensions(info); }

bool system_info_is_layer_available(const SystemInfo* info, const char* layer_name) {
    for (uint32_t i = 0; i < info->available_layer_count; ++i) {
        if (string_equals(layer_name, info->available_layers[i].layerName)) {
            return true;
        }
    }
    return false;
}

bool system_info_is_extension_available(const SystemInfo* info, const char* extension_name) {
    for (uint32_t i = 0; i < info->available_extension_count; ++i) {
        if (string_equals(extension_name, info->available_extensions[i].extensionName)) {
            return true;
        }
    }
    return false;
}

bool system_info_has_debug_utils(const SystemInfo* info) {
    return system_info_is_extension_available(info, VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
}

void system_info_destroy(SystemInfo* info) {
    if (info->available_layer_count > 0) {
        mem_free(info->available_layers);
    }
    if (info->available_extension_count > 0) {
        mem_free(info->available_extensions);
    }
    system_info_clear(info);
}
