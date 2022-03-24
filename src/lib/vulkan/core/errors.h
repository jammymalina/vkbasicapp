#ifndef VULKAN_CORE_ERRORS_H
#define VULKAN_CORE_ERRORS_H

#define ASSERT_NO_ERROR(err, err_type)                                                                                 \
    do {                                                                                                               \
        err_type cached_error = err;                                                                                   \
        if (cached_error) {                                                                                            \
            log_error("%s occured: %d", #err_type, cached_error);                                                      \
            return;                                                                                                    \
        }                                                                                                              \
    } while (0)

typedef enum FunctionLoaderError {
    FUNCTION_LOADER_NO_ERROR,
    FUNCTION_LOADER_ERROR,
} FunctionLoaderError;

typedef enum LibraryError {
    LIBRARY_NO_ERROR,
    LIBRARY_UNAVAILABLE,
    LIBRARY_GET_INSTANCE_PROC_ADDR_ERROR,
} LibraryError;

typedef enum InstanceError {
    INSTANCE_NO_ERROR,
    VULKAN_VERSION_UNAVAILABLE,
    FAILED_CREATE_INSTANCE,
    FAILED_CREATE_DEBUG_MESSENGER,
    REQUESTED_INSTANCE_LAYERS_NOT_PRESENT,
    REQUESTED_INSTANCE_EXTENSIONS_NOT_PRESENT,
    WINDOWING_EXTENSIONS_NOT_PRESENT
} InstanceError;

#endif
