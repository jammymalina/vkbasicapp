#include "./debug.h"

#include "../../core/utils/flags.h"

LogSeverity vulkan_debug_severity_to_log(VkDebugUtilsMessageSeverityFlagBitsEXT severity) {
    switch (severity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            return LOG_SEVERITY_ERROR;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            return LOG_SEVERITY_WARNING;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            return LOG_SEVERITY_INFO;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            return LOG_SEVERITY_DEBUG;
        default:
            return LOG_SEVERITY_INFO;
    }
}

const char* vulkan_debug_message_type_to_string(VkDebugUtilsMessageTypeFlagsEXT message_type) {
    VkDebugUtilsMessageTypeFlagsEXT all_flags = 0;
    FLAGS_SET_FLAG(all_flags, VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT);
    FLAGS_SET_FLAG(all_flags, VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT);
    FLAGS_SET_FLAG(all_flags, VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT);

    if (FLAGS_CHECK_ALL_BITS(message_type, all_flags)) {
        return "General | Validation | Performance";
    }

    VkDebugUtilsMessageTypeFlagsEXT current = all_flags;
    FLAGS_CLEAR_FLAG(current, VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT);
    if (FLAGS_CHECK_ALL_BITS(message_type, current)) {
        return "Validation | Performance";
    }

    current = all_flags;
    FLAGS_CLEAR_FLAG(current, VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT);
    if (FLAGS_CHECK_ALL_BITS(message_type, current)) {
        return "General | Performance";
    }

    current = all_flags;
    FLAGS_CLEAR_FLAG(current, VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT);
    if (FLAGS_CHECK_ALL_BITS(message_type, current)) {
        return "General | Validation";
    }

    if (FLAGS_CHECK_FLAG(message_type, VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)) {
        return "General";
    }

    if (FLAGS_CHECK_FLAG(message_type, VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)) {
        return "Validation";
    }

    if (FLAGS_CHECK_FLAG(message_type, VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)) {
        return "Performance";
    }

    return "Unknown";
}

inline VKAPI_ATTR VkBool32 VKAPI_CALL default_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data) {
    LogSeverity log_severity = vulkan_debug_severity_to_log(severity);
    const char* type = vulkan_debug_message_type_to_string(message_type);

    log(log_severity, "(%s) - %s", type, callback_data->pMessage);
    return VK_FALSE;
}

const char* vulkan_result_to_string(VkResult result) {
    switch (result) {
        case VK_SUCCESS:
            return "VK_SUCCESS";
        case VK_NOT_READY:
            return "VK_NOT_READY";
        case VK_TIMEOUT:
            return "VK_TIMEOUT";
        case VK_EVENT_SET:
            return "VK_EVENT_SET";
        case VK_EVENT_RESET:
            return "VK_EVENT_RESET";
        case VK_INCOMPLETE:
            return "VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED:
            return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST:
            return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED:
            return "VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT:
            return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT:
            return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS:
            return "VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_FRAGMENTED_POOL:
            return "VK_ERROR_FRAGMENTED_POOL";
        case VK_ERROR_SURFACE_LOST_KHR:
            return "VK_ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR:
            return "VK_SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR:
            return "VK_ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case VK_ERROR_VALIDATION_FAILED_EXT:
            return "VK_ERROR_VALIDATION_FAILED_EXT";
        case VK_ERROR_OUT_OF_POOL_MEMORY_KHR:
            return "VK_ERROR_OUT_OF_POOL_MEMORY_KHR";
        case VK_ERROR_INVALID_SHADER_NV:
            return "VK_ERROR_INVALID_SHADER_NV";
        case VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR:
            return "VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR";
        case VK_ERROR_NOT_PERMITTED_EXT:
            return "VK_ERROR_NOT_PERMITTED_EXT";
        case VK_RESULT_MAX_ENUM:
            break;
        default:
            break;
    }
    if (result < 0) return "VK_ERROR_<Unknown>";
    return "VK_<Unknown>";
}
