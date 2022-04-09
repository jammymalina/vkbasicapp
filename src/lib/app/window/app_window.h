#ifndef APP_WINDOW_H
#define APP_WINDOW_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "../../core/string/string.h"

#define APP_WINDOW_MAX_TITLE_SIZE 128

typedef struct AppWindow {
    SDL_Window* handle;
    bool is_init;
} AppWindow;

static inline void app_window_clear(AppWindow* window) {
    window->handle = NULL;
    window->is_init = false;
}

void app_window_destroy(AppWindow* window);

#endif
