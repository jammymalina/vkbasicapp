#ifndef VULKAN_CORE_ERRORS_H
#define VULKAN_CORE_ERRORS_H

#include "../../core/logger/logger.h"
#include "../utils/debug.h"

#define ASSERT_VULKAN_STATUS(status, message, ...)                                                                     \
    do {                                                                                                               \
        VkResult reneger_strummy = status;                                                                             \
        if (reneger_strummy != VK_SUCCESS) {                                                                           \
            log_error("VK error: %s - %s", message, vulkan_result_to_string(reneger_strummy));                         \
            return __VA_ARGS__;                                                                                        \
        }                                                                                                              \
    } while (0)

#define ASSERT_NO_ERROR(err, ...)                                                                                      \
    do {                                                                                                               \
        if (err) {                                                                                                     \
            return __VA_ARGS__;                                                                                        \
        }                                                                                                              \
    } while (0)

#define ASSERT_NO_ERROR_LOG(err, err_type, error_str_func, ...)                                                        \
    do {                                                                                                               \
        err_type cached_error = err;                                                                                   \
        if (cached_error) {                                                                                            \
            log_error("%s occured: %s", #err_type, error_str_func(cached_error));                                      \
            return __VA_ARGS__;                                                                                        \
        }                                                                                                              \
    } while (0)

typedef enum FunctionLoaderError {
    FUNCTION_LOADER_NO_ERROR,
    FUNCTION_LOADER_ERROR,
} FunctionLoaderError;

static inline const char* function_loader_error_to_string(FunctionLoaderError err) {
    switch (err) {
        case FUNCTION_LOADER_ERROR:
            return "FUNCTION_LOADER_ERROR";
        default:
            return "Uknown";
    }
}

typedef enum LibraryError {
    LIBRARY_NO_ERROR,
    LIBRARY_UNAVAILABLE,
    LIBRARY_GET_INSTANCE_PROC_ADDR_ERROR,
} LibraryError;

static inline const char* library_error_to_string(LibraryError err) {
    switch (err) {
        case LIBRARY_UNAVAILABLE:
            return "LIBRARY_UNAVAILABLE";
        case LIBRARY_GET_INSTANCE_PROC_ADDR_ERROR:
            return "LIBRARY_GET_INSTANCE_PROC_ADDR_ERROR";
        default:
            return "Uknown";
    }
}

typedef enum InstanceError {
    INSTANCE_NO_ERROR,
    NO_SYSTEM_INFO_PRESENT,
    VULKAN_VERSION_UNAVAILABLE,
    FAILED_CREATE_INSTANCE,
    FAILED_CREATE_DEBUG_MESSENGER,
    FAILED_TO_LOAD_INSTANCE_FUNCTIONS,
    REQUESTED_INSTANCE_LAYERS_NOT_PRESENT,
    REQUESTED_INSTANCE_EXTENSIONS_NOT_PRESENT,
    WINDOWING_EXTENSIONS_NOT_PRESENT,
    TOO_MANY_INSTANCE_EXTENSIONS_REQUESTED,
} InstanceError;

static inline const char* instance_error_to_string(InstanceError err) {
    switch (err) {
        case NO_SYSTEM_INFO_PRESENT:
            return "NO_SYSTEM_INFO_PRESENT";
        case VULKAN_VERSION_UNAVAILABLE:
            return "VULKAN_VERSION_UNAVAILABLE";
        case FAILED_CREATE_INSTANCE:
            return "FAILED_CREATE_INSTANCE";
        case FAILED_CREATE_DEBUG_MESSENGER:
            return "FAILED_CREATE_DEBUG_MESSENGER";
        case FAILED_TO_LOAD_INSTANCE_FUNCTIONS:
            return "FAILED_TO_LOAD_INSTANCE_FUNCTIONS";
        case REQUESTED_INSTANCE_LAYERS_NOT_PRESENT:
            return "REQUESTED_INSTANCE_LAYERS_NOT_PRESENT";
        case REQUESTED_INSTANCE_EXTENSIONS_NOT_PRESENT:
            return "REQUESTED_INSTANCE_EXTENSIONS_NOT_PRESENT";
        case WINDOWING_EXTENSIONS_NOT_PRESENT:
            return "WINDOWING_EXTENSIONS_NOT_PRESENT";
        case TOO_MANY_INSTANCE_EXTENSIONS_REQUESTED:
            return "TOO_MANY_INSTANCE_EXTENSIONS_REQUESTED";
        default:
            return "Uknown";
    }
}

typedef enum PhysicalDeviceError {
    PHYSICAL_DEVICE_NO_ERROR,
    NO_INSTANCE_PROVIDED,
    NO_SURFACE_PROVIDED,
    FAILED_ENUMERATE_PHYSICAL_DEVICES,
    FAILED_ENUMERATE_PHYSICAL_DEVICE_EXTENSIONS,
    NO_AVAILABLE_QUEUES,
    NO_PHYSICAL_DEVICES_FOUND,
    NO_SUITABLE_DEVICE,
    QUEUE_FAMILY_COUNT_TOO_LARGE,
    TOO_MANY_PHYSICAL_DEVICE_EXTENSIONS_REQUESTED,
} PhysicalDeviceError;

static inline const char* physical_device_error_to_string(PhysicalDeviceError err) {
    switch (err) {
        case NO_INSTANCE_PROVIDED:
            return "NO_INSTANCE_PROVIDED";
        case NO_SURFACE_PROVIDED:
            return "NO_SURFACE_PROVIDED";
        case FAILED_ENUMERATE_PHYSICAL_DEVICES:
            return "FAILED_ENUMERATE_PHYSICAL_DEVICES";
        case FAILED_ENUMERATE_PHYSICAL_DEVICE_EXTENSIONS:
            return "FAILED_ENUMERATE_PHYSICAL_DEVICE_EXTENSIONS";
        case NO_AVAILABLE_QUEUES:
            return "NO_AVAILABLE_QUEUES";
        case NO_PHYSICAL_DEVICES_FOUND:
            return "NO_PHYSICAL_DEVICES_FOUND";
        case NO_SUITABLE_DEVICE:
            return "NO_SUITABLE_DEVICE";
        case QUEUE_FAMILY_COUNT_TOO_LARGE:
            return "QUEUE_FAMILY_COUNT_TOO_LARGE";
        case TOO_MANY_PHYSICAL_DEVICE_EXTENSIONS_REQUESTED:
            return "TOO_MANY_PHYSICAL_DEVICE_EXTENSIONS_REQUESTED";
        default:
            return "Uknown";
    }
}

typedef enum SurfaceSupportError {
    SURFACE_SUPPORT_NO_ERROR,
    SURFACE_HANDLE_NULL,
    FAILED_GET_SURFACE_CAPABILITIES,
    FAILED_ENUMERATE_SURFACE_FORMATS,
    FAILED_ENUMERATE_PRESENT_MODES,
    TOO_MANY_SURFACE_FORMATS_REQUESTED,
    TOO_MANY_SURFACE_PRESENT_MODES_REQUESTED,
} SurfaceSupportError;

static inline const char* surface_support_error_to_string(SurfaceSupportError err) {
    switch (err) {
        case SURFACE_HANDLE_NULL:
            return "SURFACE_HANDLE_NULL";
        case FAILED_GET_SURFACE_CAPABILITIES:
            return "FAILED_GET_SURFACE_CAPABILITIES";
        case FAILED_ENUMERATE_SURFACE_FORMATS:
            return "FAILED_ENUMERATE_SURFACE_FORMATS";
        case FAILED_ENUMERATE_PRESENT_MODES:
            return "FAILED_ENUMERATE_PRESENT_MODES";
        case TOO_MANY_SURFACE_FORMATS_REQUESTED:
            return "TOO_MANY_SURFACE_FORMATS_REQUESTED";
        case TOO_MANY_SURFACE_PRESENT_MODES_REQUESTED:
            return "TOO_MANY_SURFACE_PRESENT_MODES_REQUESTED";
        default:
            return "Uknown";
    }
}

typedef enum DeviceError {
    DEVICE_NO_ERROR,
    FAILED_CREATE_DEVICE,
    NO_PHYSICAL_DEVICE_PROVIDED,
    TOO_MANY_QUEUE_DESCRIPTORS_REQUESTED,
    FAILED_TO_LOAD_DEVICE_FUNCTIONS,
} DeviceError;

static inline const char* device_error_to_string(DeviceError err) {
    switch (err) {
        case FAILED_CREATE_DEVICE:
            return "FAILED_CREATE_DEVICE";
        case NO_PHYSICAL_DEVICE_PROVIDED:
            return "NO_PHYSICAL_DEVICE_PROVIDED";
        case TOO_MANY_QUEUE_DESCRIPTORS_REQUESTED:
            return "TOO_MANY_QUEUE_DESCRIPTORS_REQUESTED";
        case FAILED_TO_LOAD_DEVICE_FUNCTIONS:
            return "FAILED_TO_LOAD_DEVICE_FUNCTIONS";
        default:
            return "Uknown";
    }
}

typedef enum SwapchainError {
    SWAPCHAIN_NO_ERROR,
    NO_DEVICE_PROVIDED_SWAPCHAIN,
    FAILED_QUERY_SURFACE_SUPPORT_DETAILS,
    FAILED_CREATE_SWAPCHAIN,
    FAILED_GET_SWAPCHAIN_IMAGES,
    FAILED_CREATE_SWAPCHAIN_IMAGE_VIEWS,
    TOO_MANY_IMAGES_REQUESTED,
} SwapchainError;

static inline const char* swapchain_error_to_string(SwapchainError err) {
    switch (err) {
        case NO_DEVICE_PROVIDED_SWAPCHAIN:
            return "NO_DEVICE_PROVIDED_SWAPCHAIN";
        case FAILED_QUERY_SURFACE_SUPPORT_DETAILS:
            return "FAILED_QUERY_SURFACE_SUPPORT_DETAILS";
        case FAILED_CREATE_SWAPCHAIN:
            return "FAILED_CREATE_SWAPCHAIN";
        case FAILED_GET_SWAPCHAIN_IMAGES:
            return "FAILED_GET_SWAPCHAIN_IMAGES";
        case FAILED_CREATE_SWAPCHAIN_IMAGE_VIEWS:
            return "FAILED_CREATE_SWAPCHAIN_IMAGE_VIEWS";
        case TOO_MANY_IMAGES_REQUESTED:
            return "TOO_MANY_IMAGES_REQUESTED";
        default:
            return "Uknown";
    }
}

typedef enum ContextError {
    CONTEXT_NO_ERROR,
    CONTEXT_INIT_ERROR,
} ContextError;

static inline const char* context_error_to_string(ContextError err) {
    switch (err) {
        case CONTEXT_INIT_ERROR:
            return "CONTEXT_INIT_ERROR";
        default:
            return "Uknown";
    }
}

typedef enum RenderingContextError {
    RENDERING_CONTEXT_NO_ERROR,
    RENDERING_CONTEXT_SWAPCHAIN_ERROR,
    RENDEERING_CONTEXT_COMMAND_CONTEXT_ERROR,
    RENDERING_CONTEXT_INIT_ERROR,
} RenderingContextError;

static inline const char* rendering_context_error_to_string(RenderingContextError err) {
    switch (err) {
        case RENDERING_CONTEXT_SWAPCHAIN_ERROR:
            return "RENDERING_CONTEXT_SWAPCHAIN_ERROR";
        case RENDERING_CONTEXT_INIT_ERROR:
            return "RENDERING_CONTEXT_INIT_ERROR";
        case RENDEERING_CONTEXT_COMMAND_CONTEXT_ERROR:
            return "RENDEERING_CONTEXT_COMMAND_CONTEXT_ERROR";
        default:
            return "Uknown";
    }
}

#endif
