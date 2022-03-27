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

static inline void system_info_clear(SystemInfo* info) {
    info->available_layers = NULL;
    info->available_layer_count = 0;
    info->available_extensions = NULL;
    info->available_extension_count = 0;
}

bool system_info_init(SystemInfo* info);
bool system_info_is_layer_available(const SystemInfo* info, const char* layer_name);
bool system_info_is_extension_available(const SystemInfo* info, const char* extension_name);
bool system_info_has_debug_utils(const SystemInfo* info);
void system_info_destroy(SystemInfo* info);

#endif
