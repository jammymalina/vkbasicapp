#ifndef VULKAN_DEBUG_H
#define VULKAN_DEBUG_H

#include <vulkan/vulkan.h>

#include "../../core/logger/logger.h"

LogSeverity vulkan_debug_severity_to_log(VkDebugUtilsMessageSeverityFlagBitsEXT severity);
const char* vulkan_debug_message_type_to_string(VkDebugUtilsMessageTypeFlagsEXT message_type);

VKAPI_ATTR VkBool32 VKAPI_CALL default_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data);

const char* vulkan_result_to_string(VkResult result);

#endif
