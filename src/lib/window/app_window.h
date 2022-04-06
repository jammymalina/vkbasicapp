#ifndef APP_WINDOW_H
#define APP_WINDOW_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "../core/string/string.h"

#define APP_WINDOW_MAX_TITLE_SIZE 128
typedef struct AppWindow {
    SDL_Window* handle;
    bool is_init;
} AppWindow;

typedef struct AppWindowBuilder {
    char title[APP_WINDOW_MAX_TITLE_SIZE];
    int x, y;
    int width, height;
    bool fullscreen;
    bool resizable;
} AppWindowBuilder;

static inline void app_window_clear(AppWindow* window) {
    window->handle = NULL;
    window->is_init = false;
}

static inline void app_window_builder_clear(AppWindowBuilder* builder) {
    string_copy("", builder->title, APP_WINDOW_MAX_TITLE_SIZE);
    builder->x = SDL_WINDOWPOS_UNDEFINED;
    builder->y = SDL_WINDOWPOS_UNDEFINED;
    builder->width = 0;
    builder->height = 0;
    builder->fullscreen = false;
    builder->resizable = false;
}

void app_window_builder_build(AppWindowBuilder* builder, AppWindow* app_window);
void app_window_destroy(AppWindow* window);

#endif
