#ifndef VULKAN_CORE_LIBRARY_H
#define VULKAN_CORE_LIBRARY_H

#include <SDL2/SDL_vulkan.h>
#include <stdbool.h>
#include <vulkan/vulkan.h>

#include "../../core/logger/logger.h"
#include "errors.h"

typedef struct Library {
    bool loaded;
    void* load_function;
} Library;

static inline void library_clear(Library* library) {
    library->loaded = false;
    library->load_function = NULL;
}

static inline LibraryError library_load(Library* library) {
    if (library->loaded) {
        log_warning("Vulkan library is already loaded");
        return LIBRARY_NO_ERROR;
    }

    if (SDL_Vulkan_LoadLibrary(NULL) != 0) {
        library->loaded = false;
        log_error("Error while loading Vulkan library: %s", SDL_GetError());
        return LIBRARY_UNAVAILABLE;
    }
    library->loaded = true;

    library->load_function = SDL_Vulkan_GetVkGetInstanceProcAddr();
    if (!library->load_function) {
        log_error("SDL_Vulkan_GetVkGetInstanceProcAddr error: %s", SDL_GetError());
        return LIBRARY_GET_INSTANCE_PROC_ADDR_ERROR;
    }

    return LIBRARY_NO_ERROR;
}

static inline void library_unload(Library* library) {
    if (!library->loaded) {
        return;
    }
    SDL_Vulkan_UnloadLibrary();
    library->loaded = false;
}

#endif
