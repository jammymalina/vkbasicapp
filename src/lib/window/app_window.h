#ifndef APP_WINDOW_H
#define APP_WINDOW_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct AppWindow {
    SDL_Window* handle;
    bool is_init;
} AppWindow;

typedef struct AppWindowBuilder {
    const char* title;
    int x, y;
    int width, height;
} AppWindowBuilder;

static inline void app_window_clear(AppWindow* window) {
    window->handle = NULL;
    window->is_init = false;
}

static inline void app_window_builder_clear(AppWindowBuilder* builder) {
    builder->title = "";
    builder->x = SDL_WINDOWPOS_UNDEFINED;
    builder->y = SDL_WINDOWPOS_UNDEFINED;
    builder->width = 600;
    builder->height = 400;
}

void app_window_builder_build(const AppWindowBuilder* builder, AppWindow* app_window);
void app_window_destroy(AppWindow* window);

#endif
