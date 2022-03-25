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

static inline AppWindow app_window_create() {
    AppWindow window = {.handle = NULL, .is_init = false};
    return window;
}

static inline AppWindowBuilder app_window_builder_create() {
    AppWindowBuilder builder = {
        .title = "",
        .x = SDL_WINDOWPOS_UNDEFINED,
        .y = SDL_WINDOWPOS_UNDEFINED,
        .width = 600,
        .height = 400,
    };
    return builder;
}

AppWindow app_window_builder_build(const AppWindowBuilder* builder);

void app_window_destroy(AppWindow* window);

#endif
