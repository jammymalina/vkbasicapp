#ifndef INSTANCE_BUILDER_H
#define INSTANCE_BUILDER_H

#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

#include "../../core/errors.h"
#include "../../core/instance.h"

// VKAPI_ATTR VkBool32 VKAPI_CALL default_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
//     VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
//     void* pUserData);

typedef struct InstanceBuilder {
    void* window_handle;
    const char* app_name;
    const char* engine_name;
    uint32_t application_version;
    uint32_t engine_version;
    PFN_vkDebugUtilsMessengerCallbackEXT debug_callback;

    const char** available_extensions;
    unsigned int available_extension_count;
} InstanceBuilder;

static inline InstanceBuilder instance_builder_create() {
    InstanceBuilder builder = {
        .window_handle = NULL,
        .app_name = "app",
        .engine_name = "engine",
        .application_version = 0,
        .engine_version = 0,
        .debug_callback = NULL,

        .available_extensions = NULL,
        .available_extension_count = 0,
    };
    return builder;
}

InstanceError instance_builder_build(InstanceBuilder* builder, Instance* instance);

#endif
