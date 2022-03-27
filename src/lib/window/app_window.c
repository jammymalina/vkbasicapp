#include "./app_window.h"

#include <SDL2/SDL.h>

#include "../core/logger/logger.h"

void app_window_builder_build(const AppWindowBuilder* builder, AppWindow* window) {
    app_window_clear(window);

    SDL_Init(SDL_INIT_VIDEO);

    window->handle =
        SDL_CreateWindow(builder->title, builder->x, builder->y, builder->width, builder->height, SDL_WINDOW_VULKAN);
    if (window->handle == NULL) {
        log_error("Unable to create the window: %s", SDL_GetError());
        return;
    }

    window->is_init = true;
}

void app_window_destroy(AppWindow* window) {
    if (window->handle) {
        SDL_DestroyWindow(window->handle);
    }
    app_window_clear(window);
    SDL_Quit();
}
