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
#define INSTANCE_BUILDER_MAX_LAYERS 16

#define INSTANCE_BUILDER_DEFAULT_DEBUG_MESSAGE_SEVERITY                                                                \
    (VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |                  \
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)

#define INSTANCE_BUILDER_DEFAULT_DEBUG_MESSAGE_TYPE                                                                    \
    (VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |                    \
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)

typedef struct InstanceBuilder {
    void* window_handle;
    const SystemInfo* system;

    const char* app_name;
    const char* engine_name;
    uint32_t application_version;
    uint32_t engine_version;

    bool debug_enabled;
    VkDebugUtilsMessageSeverityFlagsEXT debug_message_severity;
    VkDebugUtilsMessageTypeFlagsEXT debug_message_type;
    PFN_vkDebugUtilsMessengerCallbackEXT debug_callback;

    const char* layers[INSTANCE_BUILDER_MAX_LAYERS];
    uint32_t layer_count;

    const char* extensions[INSTANCE_BUILDER_MAX_EXTENSIONS];
    uint32_t extension_count;
} InstanceBuilder;

static inline void instance_builder_clear(InstanceBuilder* builder) {
    builder->window_handle = NULL;
    builder->system = NULL;
    builder->app_name = "app";
    builder->engine_name = "engine";
    builder->application_version = 0;
    builder->engine_version = 0;
    builder->debug_enabled = false;
    builder->debug_message_severity = INSTANCE_BUILDER_DEFAULT_DEBUG_MESSAGE_SEVERITY;
    builder->debug_message_type = INSTANCE_BUILDER_DEFAULT_DEBUG_MESSAGE_TYPE;
    builder->debug_callback = default_debug_callback;
    builder->extension_count = 0;
}

InstanceError instance_builder_build(InstanceBuilder* builder, Instance* instance);
bool instance_builder_add_layer(InstanceBuilder* builder, const char* layer_name);
bool instance_builder_add_extension(InstanceBuilder* builder, const char* extension_name);

#endif
