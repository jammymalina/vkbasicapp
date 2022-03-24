#ifndef INSTANCE_BUILDER_H
#define INSTANCE_BUILDER_H

#include <stdbool.h>
#include <stdint.h>
#include <vulkan/vulkan.h>

VKAPI_ATTR VkBool32 VKAPI_CALL default_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);

typedef struct InstanceBuilder {
    bool debug;
    const char* app_name;
    const char* engine_name;
    uint32_t application_version;
    uint32_t engine_version;
    PFN_vkDebugUtilsMessengerCallbackEXT debug_callback;
} InstanceBuilder;

static inline InstanceBuilder instance_builder_create() {
    InstanceBuilder builder = {
        .debug = false,
        .app_name = NULL,
        .engine_name = NULL,
        .application_version = 0,
        .engine_version = 0,
        .debug_callback = default_debug_callback,
    };
    return builder;
}

#endif
