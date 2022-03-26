#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../../../core/logger/logger.h"

typedef struct SystemInfo {
    VkLayerProperties* available_layers;
    uint32_t available_layer_count;

    VkExtensionProperties* available_extensions;
    uint32_t available_extension_count;
} SystemInfo;

static inline SystemInfo system_info_create() {
    SystemInfo info = {
        .available_layers = NULL,
        .available_layer_count = 0,
        .available_extensions = NULL,
        .available_extension_count = 0,
    };
    return info;
}

static inline void system_info_display(const SystemInfo* info) {
    log_info("Available layers: %d", info->available_layer_count);
    for (uint32_t i = 0; i < info->available_layer_count; i++) {
        log_info("  %s", info->available_layers[i].layerName);
    }

    log_info("Available extensions: %d", info->available_extension_count);
    for (uint32_t i = 0; i < info->available_extension_count; i++) {
        log_info("  %s", info->available_extensions[i].extensionName);
    }
}

bool system_info_init(SystemInfo* info);
bool system_info_is_layer_available(const SystemInfo* info, const char* layer_name);
bool system_info_is_extension_available(const SystemInfo* info, const char* extension_name);
bool system_info_has_debug_utils(const SystemInfo* info);
void system_info_destroy(SystemInfo* info);

#endif
