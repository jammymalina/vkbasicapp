#include "./app_window.h"

#include <SDL2/SDL.h>

#include "../core/logger/logger.h"

AppWindow app_window_builder_build(const AppWindowBuilder* builder) {
    AppWindow app_window = app_window_create();

    SDL_Init(SDL_INIT_VIDEO);

    app_window.handle =
        SDL_CreateWindow(builder->title, builder->x, builder->y, builder->width, builder->height, SDL_WINDOW_VULKAN);
    if (app_window.handle == NULL) {
        log_error("Unable to create the window: %s", SDL_GetError());
        return app_window;
    }

    app_window.is_init = true;
    return app_window;
}

void app_window_destroy(AppWindow* window) {
    if (window->handle) {
        SDL_DestroyWindow(window->handle);
    }
    window->handle = NULL;
    window->is_init = false;
    SDL_Quit();
}
