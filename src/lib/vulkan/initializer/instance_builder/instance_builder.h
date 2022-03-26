#ifndef INSTANCE_BUILDER_H
#define INSTANCE_BUILDER_H

#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../../core/errors.h"
#include "../../core/instance.h"
#include "../../core/system_info/system_info.h"
#include "../../utils/debug.h"

#define INSTANCE_BUILDER_MAX_EXTENSIONS 64

#define INSTANCE_BUILDER_DEFAULT_DEBUG_MESSAFE_SEVERITY                                                                \
    (VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |                  \
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)

#define INSTANCE_BUILDER_DEFAULT_DEBUG_MESSAGE_TYPE                                                                    \
    (VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |                    \
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)

typedef struct InstanceBuilder {
    void* window_handle;
    SystemInfo* system;

    const char* app_name;
    const char* engine_name;
    uint32_t application_version;
    uint32_t engine_version;

    bool debug_enabled;
    VkDebugUtilsMessageSeverityFlagsEXT debug_message_severity;
    VkDebugUtilsMessageTypeFlagsEXT debug_message_type;
    PFN_vkDebugUtilsMessengerCallbackEXT debug_callback;

    const char* extensions[INSTANCE_BUILDER_MAX_EXTENSIONS];
    unsigned int extension_count;
} InstanceBuilder;

static inline InstanceBuilder instance_builder_create() {
    InstanceBuilder builder = {
        .window_handle = NULL,
        .system = NULL,
        .app_name = "app",
        .engine_name = "engine",
        .application_version = 0,
        .engine_version = 0,
        .debug_enabled = false,
        .debug_message_severity = INSTANCE_BUILDER_DEFAULT_DEBUG_MESSAFE_SEVERITY,
        .debug_message_type = INSTANCE_BUILDER_DEFAULT_DEBUG_MESSAGE_TYPE,
        .debug_callback = default_debug_callback,
        .extensions = {},
        .extension_count = 0,
    };
    return builder;
}

InstanceError instance_builder_build(InstanceBuilder* builder, Instance* instance);

#endif
