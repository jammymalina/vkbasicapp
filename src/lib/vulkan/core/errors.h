#ifndef VULKAN_CORE_ERRORS_H
#define VULKAN_CORE_ERRORS_H

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

#endif
